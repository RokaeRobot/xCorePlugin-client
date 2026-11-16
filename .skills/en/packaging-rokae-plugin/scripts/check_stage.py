#!/usr/bin/env python3
"""校验打包工作目录。客户端与控制器按各自的插件名分别检查。

只使用 Python 3 自带的功能。
"""

import argparse
import json
import sys
from pathlib import Path

CLIENT_ARCHS = ("aarch64", "amd64", "win")
CONTROLLER_ARCHS = ("aarch64", "amd64")


def error(msg):
    print(f"ERROR: {msg}")


def unique_keep_order(items):
    seen = []
    for item in items:
        if item not in seen:
            seen.append(item)
    return seen


def plugin_dirs(parent):
    if not parent.is_dir():
        return []
    return sorted(path for path in parent.iterdir() if path.is_dir())


def suffix_for_arch(arch):
    return ".dll" if arch == "win" else ".so"


def read_json(path, errors):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as exc:
        errors.append(f"{path.as_posix()} 不是合法 JSON：{exc}")
        return None


def note_unexpected(parent, expected, prefix, flag, errors):
    present = {path.name for path in plugin_dirs(parent)}
    for extra in sorted(present - expected):
        errors.append(
            f"{prefix}{extra}/ 未列入本次打包。运行脚本前移出，或把名字加入 {flag}。"
        )


def check_client_files(plugin_dir, name, extension, errors):
    library = plugin_dir / f"{name}{extension}"
    framework = plugin_dir / f"xplugin{extension}"
    config = plugin_dir / f"{name}.json"
    if not library.is_file():
        errors.append(f"缺少 {library.as_posix()}。")
    if not framework.is_file():
        errors.append(f"缺少 {framework.as_posix()}。")
    if not config.is_file():
        errors.append(f"缺少 {config.as_posix()}。")
        return
    data = read_json(config, errors)
    if not isinstance(data, dict):
        return
    plugin = data.get("client_plugin")
    if not isinstance(plugin, dict):
        errors.append(f"{config.as_posix()} 缺少 client_plugin 对象。")
        return
    if plugin.get("name") != name:
        errors.append(f"{config.as_posix()} 中 name 为 {plugin.get('name')!r}，必须为 {name!r}。")
    for key in ("enable", "version"):
        if key not in plugin:
            errors.append(f"{config.as_posix()} 缺少 client_plugin.{key}。")


def check_controller_files(plugin_dir, name, errors):
    library = plugin_dir / f"{name}.so"
    license_file = plugin_dir / f"{name}.lic"
    config = plugin_dir / f"{name}.json"
    if not library.is_file():
        errors.append(f"缺少 {library.as_posix()}。")
    if not license_file.is_file():
        errors.append(f"缺少 {license_file.as_posix()}。")
    if not config.is_file():
        errors.append(f"缺少 {config.as_posix()}。")
        return
    data = read_json(config, errors)
    if not isinstance(data, dict):
        return
    block = data.get("controller_plugin")
    entry = block.get(name) if isinstance(block, dict) else None
    if not isinstance(entry, dict):
        errors.append(f"{config.as_posix()} 缺少 controller_plugin.{name} 对象。")
        return
    if entry.get("name") != name:
        errors.append(f"{config.as_posix()} 中 name 为 {entry.get('name')!r}，必须为 {name!r}。")


def check_client(work, names, suffix, arches, errors):
    parent = work / "client"
    if names and not parent.is_dir():
        errors.append("缺少 client/ 目录。")
        return
    if not names:
        note_unexpected(parent, set(), "client/", "--client-name", errors)
        return
    if arches:
        note_unexpected(parent, set(arches), "client/", "--client-arch", errors)
        for arch in arches:
            arch_dir = parent / arch
            if not arch_dir.is_dir():
                errors.append(f"缺少 client/{arch}/。")
                continue
            note_unexpected(
                arch_dir, set(names), f"client/{arch}/", "--client-name", errors
            )
            extension = suffix_for_arch(arch)
            for name in names:
                plugin_dir = arch_dir / name
                if not plugin_dir.is_dir():
                    errors.append(f"缺少 client/{arch}/{name}/。")
                    continue
                check_client_files(plugin_dir, name, extension, errors)
        return
    note_unexpected(parent, set(names), "client/", "--client-name", errors)
    extension = "." + suffix
    for name in names:
        plugin_dir = parent / name
        if not plugin_dir.is_dir():
            errors.append(f"缺少 client/{name}/。")
            continue
        check_client_files(plugin_dir, name, extension, errors)


def check_controller(work, names, arches, errors):
    parent = work / "controller"
    if names and not parent.is_dir():
        errors.append("缺少 controller/ 目录。")
        return
    if not names:
        note_unexpected(parent, set(), "controller/", "--controller-name", errors)
        return
    if arches:
        note_unexpected(parent, set(arches), "controller/", "--controller-arch", errors)
        for arch in arches:
            arch_dir = parent / arch
            if not arch_dir.is_dir():
                errors.append(f"缺少 controller/{arch}/。")
                continue
            note_unexpected(
                arch_dir, set(names), f"controller/{arch}/", "--controller-name", errors
            )
            for name in names:
                plugin_dir = arch_dir / name
                if not plugin_dir.is_dir():
                    errors.append(f"缺少 controller/{arch}/{name}/。")
                    continue
                check_controller_files(plugin_dir, name, errors)
        return
    note_unexpected(parent, set(names), "controller/", "--controller-name", errors)
    for name in names:
        plugin_dir = parent / name
        if not plugin_dir.is_dir():
            errors.append(f"缺少 controller/{name}/。")
            continue
        check_controller_files(plugin_dir, name, errors)


def client_ok_paths(work, names, arches):
    if arches:
        for arch in arches:
            for name in names:
                yield work / "client" / arch / name
        return
    for name in names:
        yield work / "client" / name


def controller_ok_paths(work, names, arches):
    if arches:
        for arch in arches:
            for name in names:
                yield work / "controller" / arch / name
        return
    for name in names:
        yield work / "controller" / name


def main():
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(description="校验 Rokae+ 插件打包目录")
    parser.add_argument("work", type=Path, help="打包工作目录")
    parser.add_argument("--package-os", required=True, choices=("windows", "linux"))
    parser.add_argument("--client-suffix", choices=("dll", "so"))
    parser.add_argument("--client-name", action="append", default=[])
    parser.add_argument("--controller-name", action="append", default=[])
    parser.add_argument("--client-arch", action="append", choices=CLIENT_ARCHS, default=[])
    parser.add_argument(
        "--controller-arch", action="append", choices=CONTROLLER_ARCHS, default=[]
    )
    args = parser.parse_args()

    work = args.work
    client_names = unique_keep_order(args.client_name)
    controller_names = unique_keep_order(args.controller_name)
    client_arches = unique_keep_order(args.client_arch)
    controller_arches = unique_keep_order(args.controller_arch)
    errors = []

    if not work.is_dir():
        error(f"打包工作目录不存在：{work}")
        return 1
    if not client_names and not controller_names:
        error("未指定客户端插件名或控制器插件名。")
        return 1
    if client_arches and not client_names:
        error("已给出 --client-arch，但未给出 --client-name。")
        return 1
    if controller_arches and not controller_names:
        error("已给出 --controller-arch，但未给出 --controller-name。")
        return 1
    if client_names and not client_arches and not args.client_suffix:
        error("已指定客户端插件名，但未给出 --client-suffix。")
        return 1

    script_name = "PluginPackage.bat" if args.package_os == "windows" else "PluginPackage.sh"
    script = work / script_name
    if not script.is_file():
        errors.append(f"缺少 {script.as_posix()}。请从本技能 assets/ 复制，已有同名脚本时不覆盖。")

    check_client(work, client_names, args.client_suffix or "dll", client_arches, errors)
    check_controller(work, controller_names, controller_arches, errors)

    if errors:
        print("无法继续打包，缺少或不符下列项：")
        for index, message in enumerate(errors, 1):
            print(f"{index}. {message}")
        return 1

    print("OK")
    for path in client_ok_paths(work, client_names, client_arches):
        print(f"client: {path.resolve()}")
    for path in controller_ok_paths(work, controller_names, controller_arches):
        print(f"controller: {path.resolve()}")
    print(f"script: {script.resolve()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
