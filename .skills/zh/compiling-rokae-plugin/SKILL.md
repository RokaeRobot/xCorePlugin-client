---
name: compiling-rokae-plugin
description: >-
  编译 Rokae+ 客户端插件或控制器插件时使用。客户端插件为 Windows x86_64 或 Linux aarch64。控制器插件为 Linux x86_64 或 Linux aarch64。编译 aarch64 版本时，常在 Linux x86_64 上使用交叉编译工具链。编译完成后给出插件库的绝对路径。不制作插件包，不新建客户端插件工程。
compatibility: Requires a C++ toolchain for the selected platform. Client plugins also require Qt 5 qmake. Controller plugins do not require Qt.
metadata:
  version: "1.0"
---

# 编译 Rokae+ 插件

本技能用于编译客户端插件或控制器插件。

## 编译环境

客户端插件有 Windows x86_64 和 Linux aarch64 两个版本。Linux aarch64 为示教器版本。

控制器插件只有 Linux x86_64 和 Linux aarch64。

用户请求已经写明版本，且插件、系统和架构在表中同一行时，使用对应的一行。请求没有写明版本时，先按「确认事项」问清版本，再使用对应的一行。用户指定的插件、系统和架构在表中没有同一行时，停止编译，请用户从表中四行指定一行。确认前禁止编译。禁止将不同行中的插件、系统或架构重新组合。

| 插件 | 系统 | 架构 | 官方推荐工具链 | 交付 |
|---|---|---|---|---|
| 客户端插件 | Windows | x86_64 | MSVC 2019（工具集 v142）及以上。Qt 5.12 及以上的 5.x。禁止使用 Qt 6 | `.dll` |
| 客户端插件 | Linux（示教器） | aarch64 | GCC 7.4.1 及以上。Qt 5.12.11 及以上的 5.x。禁止使用 Qt 6 | `.so` |
| 控制器插件 | Linux | x86_64 | gcc-5.4 / g++-5.4（Ubuntu 20.04） | `.so` |
| 控制器插件 | Linux | aarch64 | gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu（Ubuntu 20.04） | `.so` |

表中工具链为官方推荐。表外工具链在与插件宿主程序兼容时可以考虑使用，采用前由用户确认。禁止使用 Qt 6，因为 Qt 6 与插件宿主程序不兼容。本技能不自行搭建编译环境。

## 确认事项

编译之前，可以先读取用户给出的工程和路径，用来发现缺少的信息。缺少的事项在同一回复中一次问清，然后停止。确认前禁止修改插件工程，禁止编译。编译前须确认下列事项：

1. 插件种类与插件名。确认本次编译的是客户端插件还是控制器插件。客户端插件名读取 `.pro` 中的 `PLUGIN_NAME` 与 `XPLUGIN_REGISTER`。控制器插件名读取工程中用于动态库主文件名的注册名。
   - 工程中读不到插件名：请用户给出。禁止推测插件名。禁止新建工程。新建工程属于 `preparing-client-plugin`。
   - 工程中名称不一致：列出各处原值，请用户指定要保留的名字。确认前禁止修改文件。
   - 用户明确要求改名：列出注册名、动态库主文件名和配置文件名。确认前禁止修改文件。确认后改为新名字，再编译。禁止把客户端插件名写入控制器工程，禁止把控制器插件名写入客户端工程。
2. 编译版本。确认本次编译的版本，并按「编译环境」使用表中对应的一行。请求没有写明版本时，请用户指定。用户指定的插件、系统和架构在表中没有同一行时，停止编译，请用户从表中四行指定一行。
3. 二次开发包路径。
   - 客户端插件：确认客户端二次开发包路径。开发包含 `include/`，以及 `lib/windows` 或 `lib/aarch64`。
   - 控制器插件：确认控制器二次开发包路径。开发包包含 `xcore_api/`。编译控制器插件时不需要 Qt 环境。

上述事项确认完成后，按「取得编译环境」继续。

## 取得编译环境

按确认事项中已经确定的一行，取得编译器路径。客户端插件还须取得 `qmake` 路径。路径尚未取得时，停止编译，请用户给出缺少的路径。

### Windows

AI agent 自行决定如何确定编译器路径和 `qmake` 路径。本技能禁止全盘扫描。取得路径后，AI agent 直接编写本技能目录下的 `compiling-rokae-plugin/.local/windows-build-env.json`。本次对话已经将编译器路径和 `qmake` 路径写入 `compiling-rokae-plugin/.local/windows-build-env.json` 时，优先使用 `compiling-rokae-plugin/.local/windows-build-env.json` 中的编译器路径和 `qmake` 路径。记录不写入插件工程，也不写入 `compiling-rokae-plugin/.local/linux-build-env.json`。

`compiling-rokae-plugin/.local/windows-build-env.json` 的内容模板如下：

```json
{
  "compiler": "<编译器路径>",
  "qmake": "<qmake路径>"
}
```

### Linux

在 Linux 上取得编译器路径。示教器 Linux 还须取得 `qmake` 路径。记录文件为本技能目录下的 `compiling-rokae-plugin/.local/linux-build-env.json`。记录不写入插件工程。

本技能目录下的 `scripts/find_linux_build_env.sh` 用于列出候选路径，只在 Linux 上运行，不写入记录文件。需要列出候选时，在本技能目录执行：

```bash
sh scripts/find_linux_build_env.sh
```

查找范围限于 `/usr`、`/usr/local`、`/opt`，并打印与官方推荐相符的候选。本技能禁止在脚本以外查找编译器，禁止改为查找其他目录。不查找开发包，本技能禁止全盘扫描，不因常见安装位置选用目录。脚本没有符合条件的候选时，请用户给出路径，并停止查找。用户确认其他编译器与插件宿主程序兼容后，也可以把用户给出的路径写入记录。

取得路径后，AI agent 直接编写本技能目录下的 `compiling-rokae-plugin/.local/linux-build-env.json`。`compiling-rokae-plugin/.local/linux-build-env.json` 的内容模板如下。尚未取得的版本写为 `null`。记录文件已存在时，只更新本次版本对应的键，禁止改写另外两个键。示教器 Linux 须一并写入 `qmake` 路径。

```json
{
  "pendant-aarch64": {"qmake": "<qmake路径>", "gcc": "<gcc路径>", "gxx": "<g++路径>"},
  "controller-x86_64": {"gcc": "<gcc路径>", "gxx": "<g++路径>"},
  "controller-aarch64": {"gcc": "<gcc路径>", "gxx": "<g++路径>"}
}
```

按下列顺序判断。前面一条已经满足时，不继续判断后续条目。

1. 记录文件中已有本次版本的路径：使用记录中的路径。
2. 本对话中已经得到编译器路径，记录文件中尚未写入本次版本：AI agent 直接编写记录文件，再使用记录中的路径。
3. 本对话中已列出候选路径，尚未选定一条路径：请用户选定一条路径后，AI agent 直接编写记录文件。
4. 记录文件中没有本次版本，本对话中也没有候选路径：运行查找脚本列出候选。请用户选定一条路径后，AI agent 直接编写记录文件。

编译器路径已经取得后，按「编译」继续。客户端插件还须已经取得 `qmake` 路径。

## 编译

默认编译 Release。用户未明确要求 Debug 时，禁止编译 Debug。只编译本次确认的插件种类。目录中已有插件名加上 `.dll` 或 `.so` 的文件时，由本次编译覆盖。禁止因目录中已有插件名加上 `.dll` 或 `.so` 的插件库而跳过编译。编译成功后，在构建目录或工程目录中给出插件库的绝对路径。

客户端插件与控制器插件分别按下列方式编译：

### 客户端插件

在工程目录使用 `qmake`，带上 `CONFIG+=release`。

- Windows：同一命令行中先准备好 MSVC 环境，再执行 `qmake`，然后执行 `nmake` 或 `jom`。
- 示教器 Linux：示教器为 aarch64。编译所在机器为 Linux x86_64 时使用交叉编译。使用已取得的 aarch64 `qmake` 与 GCC，再执行 `make`。

### 控制器插件

在工程目录使用 CMake，配置时指定 `-DCMAKE_BUILD_TYPE=Release`。

- Linux x86_64：把 `CMAKE_C_COMPILER` 和 `CMAKE_CXX_COMPILER` 指向已取得的 gcc 与 g++。
- Linux aarch64：控制器为 aarch64。编译所在机器为 Linux x86_64 时使用交叉编译，并把 `CMAKE_C_COMPILER` 和 `CMAKE_CXX_COMPILER` 指向已取得的 gcc 与 g++。

## 编译成功

如果用户还要求制作插件包，后续打包步骤由 `packaging-rokae-plugin` 执行。

## 编译失败

编译失败时立即停止。先贴出报错摘录，再写简要分析，并列出可选修复。未获用户进一步指示前，禁止修改工程文件，禁止更换编译环境，禁止重新编译。
