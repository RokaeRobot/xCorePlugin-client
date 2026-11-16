---
name: preparing-client-plugin
description: >-
  初始化 Rokae+ 客户端插件工程，或将页面挂载到示教器时使用。尚无插件工程时创建最简工程，并使用 xCorePlugin 的 include/ 与 lib/。中心页调用 CreateCenterWidget，底部状态栏调用 CreateStatusBar，右侧扩展窗调用 CreateRightExpandWidgte，物理按键调用 CreatePhyButtonList。不编译，不打包，不编写业务接口。
compatibility: Requires Python 3 only when creating the minimal project, to run scripts/create_minimal_project.py. Do not install Python and do not search for an interpreter. Requires the Rokae+ client plugin SDK (xCorePlugin) with include/ and lib/. Target Qt is 5.12 or any later 5.x release. Do not use Qt 6.
metadata:
  version: "1.0"
---

# 准备客户端插件

本技能用于两项工作：初始化最简工程和挂载页面。

本技能不适用于编译、打包和业务接口的编写。

工程适用 Qt 5.12 及以上的 5.x。禁止使用 Qt 6，因为 Qt 6 与插件宿主程序不兼容。

## 工程目录

工程目录为用户指定的目录。用户未指定工程目录时，工程目录为当前工作目录。`.pro` 放在工程目录中，头文件与源文件放在工程目录下的 `src/` 中。

## 初始化最简工程

### 创建条件

工程目录中已有插件工程的 `.pro`，或已有实现插件的头文件、源文件时，禁止创建新的工程，挂载页面仍可进行。`include/` 与 `lib/` 属于开发包，不视为插件工程文件。工程目录中仅有开发包时，仍可创建工程。

### 确认事项

初始化前须确认下列三项。缺项在同一回复中一次问清。下列三项未经用户确认前，禁止执行 `git init`，禁止写入工程文件，禁止解压，也禁止复制开发包。

1. 插件名。同时用作 `PLUGIN_NAME` 与 C++ 类名，例如 `MyPlugin`。不符合 C++ 类名规则时，请用户提供一个符合该规则的插件名。
2. 开发包。先检查工程目录，再询问准备采用的方式。禁止在工程目录以外搜索开发包。同时存在多处来源时，列出全部来源，请用户选定一处。
   - 工程目录已有 `include/` 与 `lib/`：询问是否直接使用这两项目录。
   - 工程目录中有文件夹，文件夹名称包含 `xCorePlugin`，且文件夹内含有 `include/` 与 `lib/`：询问是否将文件夹内的 `include/` 与 `lib/` 复制到工程目录。
   - 工程目录中有压缩包，压缩包文件名包含 `xCorePlugin`：询问是否解压，并将压缩包中的 `include/` 与 `lib/` 放置到工程目录。
   - 以上来源均不具备时：询问开发包路径。
3. 是否使用 git 进行版本管理。

### 创建步骤

用户确认后，按下列顺序执行。

1. 用户同意使用 git，且工程目录尚不是 Git 仓库：先在工程目录执行 `git init`。
2. 按用户确认的方式准备开发包。用户确认直接使用工程目录已有的 `include/` 与 `lib/` 时，禁止再次复制。
3. 在本技能目录中执行下列命令，生成 `.pro` 与 `src/` 中的头文件、源文件。命令中的 `MyPlugin` 换成已确认的插件名，`工程目录` 换成实际路径。

   ```bash
   python scripts/create_minimal_project.py --name MyPlugin --dir 工程目录
   ```

   用户给出 `setPluginLabel` 的参数时，在命令末尾追加 `--label`，参数写在 `--label` 后面。用户未给出 `setPluginLabel` 的参数时，不追加 `--label`，脚本使用插件名作为 `setPluginLabel` 的参数。

   `python` 命令不存在，或版本不是 Python 3 时，禁止运行生成脚本，禁止安装 Python，禁止在工程目录以外查找解释器，禁止改为手工写入 `.pro`、头文件与源文件，禁止执行下面的第 4 步，禁止进入本地提交。此前已执行的 `git init` 与已准备的开发包保持不动。

4. 列出本次新建的文件，请用户确认文件内容。

## 挂载页面

需要挂载页面时，工程目录中须已有插件类。工程目录中尚无插件类时，禁止挂载页面。已有插件类时，读取 [references/mount-api.md](references/mount-api.md)，并按下表选择函数。

| 位置 | 函数 |
|---|---|
| 工艺包页面、中心页 | `CreateCenterWidget` |
| 底部状态栏 | `CreateStatusBar` |
| 右侧扩展窗 | `CreateRightExpandWidgte` |
| 物理按键、示教器侧键 | `CreatePhyButtonList` |

在插件类的 `init()` 中调用。表中函数的参数 `pluginName` 传入 `PLUGIN_NAME`。完成后列出改动，请用户确认。

## 本地提交

初始化最简工程或挂载页面完成后，仅在工程目录是 Git 仓库时执行下列步骤。

1. 用户确认本次改动后，询问是否提交到本地仓库。
2. 用户同意提交后，执行 `git add` 与 `git commit`。

用户未要求推送时，禁止执行 `git push`。
