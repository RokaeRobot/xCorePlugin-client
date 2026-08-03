# xCore 客户端插件开发包（HMI Plugin SDK）

面向 **ROKAE xCore 客户端（HMI）** 的二次开发软件包。基于本 SDK，可在不修改 HMI 宿主核心的前提下，扩展工艺包界面、底部状态栏、辅助编程 / 自定义 RL 指令，并与控制器插件进行数据通信。

仓库地址：https://github.com/RokaeRobot/xCorePlugin-client

## 兼容性

### 机器人控制器 / 客户端

- **xCore >= v3.2.2**（当前开发包版本：**v1.0.2.7**，详见 [CHANGELOG.md](./CHANGELOG.md)）

### 开发环境

| 项目 | 要求 |
|------|------|
| 语言 | C++11 |
| GUI | Qt **5.12+**（Widgets；不使用 QML） |
| 构建 | qmake / Qt Creator |
| Windows | Windows 10/11，**MSVC 2019**，x86_64 |
| Linux | Ubuntu 18.04 等，**GCC 7.4** / aarch64 工具链 |

> 为保证与宿主 **ABI** 一致，请优先使用 ROKAE+ 开发包推荐的编译器与 Qt 版本构建。

## 获取预编译库

本仓库包含 **头文件**、**示例工程** 与变更说明；预编译的 **xplugin** 动态库通过 GitHub **Releases** 分发（请与文档 / Tag 版本对齐）。

### 获取步骤

1. **克隆本仓库**

```bash
git clone https://github.com/RokaeRobot/xCorePlugin-client.git
cd xCorePlugin-client
```

2. **确认 xplugin 版本** — 须与 Release 版本一致（当前为 **v1.0.2.7**，见 [CHANGELOG.md](./CHANGELOG.md)）。

3. **打开对应 Release 页面** — [Release v1.0.2.7](https://github.com/RokaeRobot/xCorePlugin-client/releases/tag/v1.0.2.7)  
   链接格式：`https://github.com/RokaeRobot/xCorePlugin-client/releases/tag/v{VERSION}`

4. **下载** `xCorePlugin_v{version}.zip`（例如 `xCorePlugin_v1.0.2.7.zip`）。

5. **解压到仓库约定目录**，使预编译库落入：

```text
xCorePlugin-client/lib/
  windows/     # xplugin.dll / xplugin.lib 等
  aarch64/     # xplugin.so 等
```

> 具体文件名以 Release 包内目录为准；示例工程 `examples/xplugindemo` 已按上述相对路径链接库。

## 工程说明及使用方法

### 目录说明

| 路径 | 说明 |
|------|------|
| `include/` | 对外头文件（PluginBase、InterfaceManager、RL、事件系统、日志等） |
| `lib/windows` | Windows 预编译 xplugin（来自 Release） |
| `lib/aarch64` | aarch64 Linux 预编译 xplugin（来自 Release） |
| `examples/xplugindemo` | 完整功能示例（主窗体、状态栏、扩展侧栏、自定义 RL 等） |
| `examples/example` | 精简示例 |
| `CHANGELOG.md` | 版本变更记录 |
| `LICENSE` | 开源许可证（LGPL-2.1） |

### 使用方法

1. 使用 **Qt Creator** 打开 `examples/xplugindemo/xplugindemo.pro`（或自建动态库工程）。
2. 配置 `INCLUDEPATH` 指向本仓库 `include/`，`LIBS` 指向对应平台的 `lib/`。
3. 继承 **PluginBase**，使用 **XPLUGIN_REGISTER** 注册插件，并保证 `PLUGIN_NAME`、产物库名与工艺包配置中的名称一致。
4. 编译生成 `.dll` / `.so` 后，按工艺包 **config.json** 格式打包，导入 xCore 客户端使用。

#### Windows（qmake 片段）

```qmake
QT += widgets
CONFIG += c++11 plugin no_plugin_name_prefix
TEMPLATE = lib
DEFINES += PLUGIN_NAME=\"MyPlugin\"
INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/lib/windows -lxplugin
```

#### 说明

- 完整生命周期为：`init()` → `afterInit()` → `render()` → `idle()` → `detach()`。
- 客户端插件出现在 xCore **工艺包**相关界面；安装与导入流程见在线文档「安装与使用」。
- 示例覆盖主界面注册、底部状态栏、右侧扩展窗体、辅助编程指令组 / 自定义 RL、与控制器插件通信等能力；更多接口请查阅在线文档「接口说明」。

### 与控制器插件协同

客户端插件与控制器插件共同构成 **Rokae+ 插件生态**：

- 控制器插件开发包：https://github.com/RokaeRobot/xCorePlugin-controller

两端版本需与目标 **xCore** 版本匹配；升级前请阅读两侧仓库的变更说明。

## 授权与注意

- 插件加载后若提示「未授权」，请联系珞石获取正式授权，勿使用非官方渠道的授权文件。
- 客户端 SDK、控制器 **xcore_api** 与目标控制器 / HMI 版本需对齐。

## 文档

更完整的快速入门、功能模块、接口说明与版本记录，请参阅在线文档：

**Rokae+ 插件 → 客户端插件（HMI）**

推荐阅读顺序：依赖与环境 → 制作一个简单的 HMI 插件 → 功能模块 / 接口说明 → 示例 → 安装与使用。

## License

本仓库遵循 **Apache-2.0 License**，详见 [LICENSE](./LICENSE)。

> Copyright (C) ROKAE (Beijing) Technology Co., LTD.
