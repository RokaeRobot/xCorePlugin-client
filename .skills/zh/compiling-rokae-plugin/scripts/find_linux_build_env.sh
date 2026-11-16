#!/bin/sh
# 在 Linux 的 /usr、/usr/local、/opt 中查找 Rokae+ 插件可用的 qmake 与 GCC。
# 标准输出只列出与官方推荐相符的候选。不写入记录文件。
# 不查找开发包，不查找 MSVC。

set -u

die() {
    printf '%s\n' "$1" >&2
    exit 1
}

require_linux() {
    if [ "$(uname -s)" != Linux ]; then
        die "只在 Linux 上运行。"
    fi
}

# $1 大于或等于 $2 时返回 0。比较使用 GNU sort 的版本序。
version_ge() {
    printf '%s\n%s\n' "$2" "$1" | sort -C -V && return 0
    return 1
}

# $1 的主版本号与次版本号等于 $2 时返回 0，例如 5.4 与 5.4.0。
version_eq_mm() {
    case $1 in
        "$2"|"$2".*)
            return 0
            ;;
    esac
    return 1
}

gcc_full_version() {
    ver=$("$1" -dumpfullversion 2>/dev/null || true)
    if [ -z "$ver" ]; then
        ver=$("$1" -dumpversion 2>/dev/null || true)
    fi
    printf '%s' "$ver" | head -n 1 | tr -d '[:space:]'
}

# 示教器 Linux：Qt 5，且不低于 5.12.11。不要使用 Qt 6。
# 交叉编译时 QMAKE_XSPEC 须含 aarch64。在 aarch64 本机上允许 linux-g++。
qt_acceptable() {
    ver=$1
    spec=$2
    major=${ver%%.*}
    if [ "$major" != 5 ]; then
        return 1
    fi
    if ! version_ge "$ver" "5.12.11"; then
        return 1
    fi
    case $spec in
        *aarch64*)
            return 0
            ;;
    esac
    [ "$HOST_ARCH" = aarch64 ]
}

compiler_basename_ok() {
    base=$1
    case $base in
        gcc-ar|gcc-nm|gcc-ranlib|g++-ar|*-gcc-ar|*-gcc-nm|*-gcc-ranlib)
            return 1
            ;;
        gcc|g++|gcc-*|g++-*|*-gcc|*-g++)
            return 0
            ;;
    esac
    return 1
}

executable_file() {
    [ -n "$1" ] && [ -f "$1" ] && [ -x "$1" ]
}

append_qmake() {
    {
        printf '%s\n' "$1"
        printf 'QT_VERSION=%s\n' "$2"
        printf 'QT_INSTALL_PREFIX=%s\n' "$3"
        printf 'QMAKE_XSPEC=%s\n' "$4"
        printf '\n'
    } >> "$qmake_out"
}

append_compiler() {
    dest=$1
    path=$2
    ver=$3
    machine=$4
    {
        printf '%s\n' "$path"
        if [ -n "$ver" ]; then
            printf '版本=%s\n' "$ver"
        else
            printf '%s\n' "版本=未知"
        fi
        if [ -n "$machine" ]; then
            printf '目标架构=%s\n' "$machine"
        fi
        printf '\n'
    } >> "$dest"
}

note_qmake() {
    path=$1
    if ! executable_file "$path"; then
        return 0
    fi
    if grep -F -x "$path" "$seen" >/dev/null 2>&1; then
        return 0
    fi
    printf '%s\n' "$path" >> "$seen"
    ver=$("$path" -query QT_VERSION 2>/dev/null || true)
    prefix=$("$path" -query QT_INSTALL_PREFIX 2>/dev/null || true)
    spec=$("$path" -query QMAKE_XSPEC 2>/dev/null || true)
    ver=$(printf '%s\n' "$ver" | head -n 1 | tr -d '[:space:]')
    prefix=$(printf '%s\n' "$prefix" | head -n 1 | tr -d '[:space:]')
    spec=$(printf '%s\n' "$spec" | head -n 1 | tr -d '[:space:]')
    if ! qt_acceptable "$ver" "$spec"; then
        qmake_skip=$((qmake_skip + 1))
        return 0
    fi
    append_qmake "$path" "$ver" "$prefix" "$spec"
}

note_compiler() {
    path=$1
    base=$(basename "$path")
    case $base in
        gcc-linaro-*)
            return 0
            ;;
    esac
    if ! compiler_basename_ok "$base"; then
        return 0
    fi
    if ! executable_file "$path"; then
        return 0
    fi
    if grep -F -x "$path" "$seen" >/dev/null 2>&1; then
        return 0
    fi
    printf '%s\n' "$path" >> "$seen"
    machine=$("$path" -dumpmachine 2>/dev/null || true)
    machine=$(printf '%s\n' "$machine" | head -n 1 | tr -d '[:space:]')
    ver=$(gcc_full_version "$path")
    # 控制器按版本与目标架构归类，不要求可执行文件名与表格中的名称相同。
    if version_eq_mm "$ver" "5.4"; then
        case $machine in
            *aarch64*)
                compiler_skip=$((compiler_skip + 1))
                return 0
                ;;
        esac
        append_compiler "$x86_out" "$path" "$ver" "$machine"
        return 0
    fi
    if version_eq_mm "$ver" "6.3.1"; then
        case $machine in
            *aarch64*)
                append_compiler "$linaro_out" "$path" "$ver" "$machine"
                return 0
                ;;
        esac
        compiler_skip=$((compiler_skip + 1))
        return 0
    fi
    case $machine in
        *aarch64*)
            ;;
        *)
            compiler_skip=$((compiler_skip + 1))
            return 0
            ;;
    esac
    if [ -z "$ver" ] || ! version_ge "$ver" "7.4.1"; then
        compiler_skip=$((compiler_skip + 1))
        return 0
    fi
    append_compiler "$pendant_out" "$path" "$ver" "$machine"
}

print_section() {
    title=$1
    file=$2
    printf '%s\n' "$title"
    if [ ! -s "$file" ]; then
        printf '%s\n\n' "无符合条件的结果。"
        return 0
    fi
    cat "$file"
}

search_env() {
    raw=$(mktemp) || die "无法创建临时文件。"
    seen=$(mktemp) || die "无法创建临时文件。"
    qmake_out=$(mktemp) || die "无法创建临时文件。"
    pendant_out=$(mktemp) || die "无法创建临时文件。"
    x86_out=$(mktemp) || die "无法创建临时文件。"
    linaro_out=$(mktemp) || die "无法创建临时文件。"
    trap 'rm -f "$raw" "$seen" "$qmake_out" "$pendant_out" "$x86_out" "$linaro_out"' EXIT
    qmake_skip=0
    compiler_skip=0
    # 一次遍历三个目录。无法进入的目录跳过，不从根目录继续向外查找。
    find /usr /usr/local /opt -xdev \( \
        \( \( -type f -o -type l \) \( \
            -name qmake -o -name 'qmake-qt5' -o \
            -name gcc -o -name g++ -o \
            -name 'gcc-*' -o -name 'g++-*' -o \
            -name '*-gcc' -o -name '*-g++' -o \
            -name 'gcc-linaro-*' \
        \) \) \
        -o \
        \( -type d -name 'gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu' \) \
    \) -print > "$raw" 2>/dev/null || true
    while IFS= read -r path; do
        [ -n "$path" ] || continue
        base=$(basename "$path")
        if [ -d "$path" ] && [ ! -L "$path" ] && [ "$base" = gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu ]; then
            for bin in "$path"/bin/*; do
                if [ -e "$bin" ]; then
                    note_compiler "$bin"
                fi
            done
            continue
        fi
        case $base in
            qmake|qmake-qt5)
                note_qmake "$path"
                ;;
            *)
                note_compiler "$path"
                ;;
        esac
    done < "$raw"
    print_section "qmake" "$qmake_out"
    print_section "示教器 Linux" "$pendant_out"
    print_section "控制器 x86_64" "$x86_out"
    print_section "控制器 aarch64" "$linaro_out"
    printf '已跳过 qmake %s 个。\n' "$qmake_skip"
    printf '已跳过编译器 %s 个。\n' "$compiler_skip"
}

require_linux
HOST_ARCH=$(uname -m)

if [ $# -gt 0 ]; then
    die "查找脚本不接受参数。"
fi

search_env
