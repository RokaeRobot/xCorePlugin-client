#!/usr/bin/env python3
"""根据模板生成最简 Rokae+ 客户端插件工程。

只使用 Python 3 自带的功能。
模板位于本技能目录的 assets 文件夹。
脚本向工程目录写入三份文件：插件名.pro、src 下的头文件、src 下的源文件。
`.pro` 的文件名与插件名相同。头文件与源文件的主文件名写成首字母大写的驼峰形式，不含下划线。
同名文件已存在时不覆盖。
"""

import argparse
import re
import sys
from pathlib import Path

# 插件名须能作为 C++ 类名：以英文字母或下划线开头，其余为英文字母、数字或下划线。
IDENT = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def die(msg):
    """把错误信息写到标准错误，并返回失败状态。"""
    print(msg, file=sys.stderr)
    return 1


def pascal_case(name):
    """去掉下划线，并把每个词的首字母改为大写。"""
    parts = [part for part in name.split("_") if part]
    return "".join(part[:1].upper() + part[1:] for part in parts)


def cpp_string(text):
    """把反斜杠、引号和换行写成 C++ 字符串能够容纳的形式。"""
    return (
        text.replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\r", "\\r")
        .replace("\n", "\\n")
    )


def render(template, name, pascal, upper, label):
    """替换模板中的占位符。

    先替换全大写占位符和文件名占位符，再替换 setPluginLabel 的参数，最后替换插件名。
    若先替换插件名，全大写占位符和文件名占位符会被截断。
    """
    text = template.replace("{{PLUGIN_NAME_UPPER}}", upper)
    text = text.replace("{{PLUGIN_NAME_PASCAL}}", pascal)
    text = text.replace("{{PLUGIN_LABEL}}", label)
    text = text.replace("{{PLUGIN_NAME}}", name)
    return text


def main():
    """读取参数，检查工程目录，再写入三份文件。"""
    parser = argparse.ArgumentParser(description="生成最简客户端插件工程")
    parser.add_argument("--name", required=True, help="插件名，同时用作 C++ 类名")
    parser.add_argument("--dir", required=True, type=Path, help="工程目录")
    parser.add_argument("--label", default=None, help="setPluginLabel 的参数。省略时使用插件名")
    args = parser.parse_args()

    name = args.name
    if not IDENT.fullmatch(name):
        return die("插件名不是合法的 C++ 标识符。")

    # 未给出 setPluginLabel 的参数时，使用插件名。
    label = cpp_string(name if args.label is None else args.label)
    root = args.dir
    if not root.is_dir():
        return die(f"工程目录不存在：{root}")

    # 头文件与源文件使用首字母大写的驼峰形式，不含下划线。
    # 头文件保护宏与主文件名的字母相同，并改为全大写。
    pascal = pascal_case(name)
    if not pascal:
        return die("插件名不是合法的 C++ 标识符。")
    upper = pascal.upper()
    pro_path = root / f"{name}.pro"
    header_path = root / "src" / f"{pascal}.h"
    source_path = root / "src" / f"{pascal}.cpp"
    existing = [path for path in (pro_path, header_path, source_path) if path.exists()]
    if existing:
        listed = "\n".join(str(path) for path in existing)
        return die(f"目标文件已存在，未写入：\n{listed}")

    assets = Path(__file__).resolve().parent.parent / "assets"
    outputs = (
        (assets / "plugin.pro.tmpl", pro_path),
        (assets / "plugin.h.tmpl", header_path),
        (assets / "plugin.cpp.tmpl", source_path),
    )
    # 先读完模板并完成替换。模板缺失时不创建 src，也不写入任何文件。
    rendered = []
    for template_path, dest in outputs:
        if not template_path.is_file():
            return die(f"缺少模板：{template_path}")
        text = template_path.read_text(encoding="utf-8")
        rendered.append((dest, render(text, name, pascal, upper, label)))

    header_path.parent.mkdir(parents=True, exist_ok=True)
    for dest, text in rendered:
        with dest.open("w", encoding="utf-8", newline="\n") as handle:
            handle.write(text)
        print(dest)
    return 0


if __name__ == "__main__":
    sys.exit(main())
