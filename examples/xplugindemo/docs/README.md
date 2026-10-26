# xplugindemo 项目结构与使用说明

> 写给基于 **Rokae+ xplugin** 开发或改造插件的二次开发者。
> 以官方示例 `examples/xplugindemo` 为模板，讲述**目录怎么摆**、**代码怎么写**、**API 怎么调**、**插件怎么部署**。
> 你只需会链 SDK、实现约定生命周期、调 API、部署到指定目录。

---

## 1. 一句话理解

你的插件是 **Qt 动态库**（如 `myplugin.dll`），通过 xplugin 提供的：

- 头文件 `include/`
- 运行时库 `lib/` 下的 `xplugin`
- 注册宏 `XPLUGIN_REGISTER` + 基类 `PluginBase`

挂到示教器 HMI上，并用 `xplugin::...` / `xPluginInterface()` 访问机器人、工程、IO、XService 等能力。

**推荐路径**：先编译运行 `xplugindemo` → 在六个功能页点按钮看结果 → 最后复制工程改 `PLUGIN_NAME`实现你自己的项目。

---

## 2. 开发前准备

| 项 | 说明 |
|----|------|
| SDK | `xplugin_include/`（`include/` + `lib/` + 本示例） |
| IDE | Qt Creator + qmake + MSVC / Linux 套件 |
| Qt | Qt5 Widgets（不用 QML 作主 UI） |
| 宿主 | 已集成 xplugin 的示教器，能在「插件管理」启用 |
| 命名 | `PLUGIN_NAME` 全局唯一，与目录名、注册宏一致 |

---

## 3. 项目目录结构

```
xplugindemo/
|-- xplugindemo.pro           # qmake：插件名、链库、源文件
|-- xplugindemo.h/.cpp        # 入口：注册 + 生命周期
|-- xplugindemo_global.h      # DLL 导出宏
|-- expandwidget.*            # 右侧拓展示例
|-- showstatusbar.*           # 状态栏示例
|-- pages/
|   |-- demoshell.*           # 中心页导航
|   |-- common/               # 布局 / 结果面板 / 样式
|   `-- xplugin/              # **业务 API 用法页**
|-- rl/                       # 辅助编程 UI
|-- sdk/                      # 可选第三方 SDK（默认关）
|-- language/                 # 翻译 ts/qm
|-- render/                   # QSS
|-- xplugindemo.qrc
`-- docs/                    # 说明文档（含本文）
```

### 3.1 各块怎么用

| 路径 | 你要学什么 |
|------|----------------|
| `xplugindemo.cpp` | **最小闭环**：`XPLUGIN_REGISTER` / `init` 挂 HMI / `afterInit` 注 RL |
| `pages/demoshell.*` | 中心页多页导航（可换成单页） |
| `pages/xplugin/*` | **对着抄 API**：每个按钮一次真实调用 |
| `pages/common/` | 操作区 + 结果区统一布局 |
| `expandwidget.*` | `CreateRightExpandWidgte` / `SwitchRightExpandWidgte` |
| `showstatusbar.*` | `CreateStatusBar` |
| `rl/` | 自定义辅助编程指令 UI |
| `sdk/` | 不需要可忽略；默认关闭 `HAVE_ROKAE_SDK` |
| `language/` | `tr()` + `.ts` -> `.qm` |

---

## 4. 复制出自己的插件

1. 复制 `xplugindemo` 目录，改名（如 `myvision`）。
2. 改 `.pro`：
   - `DEFINES += PLUGIN_NAME=\"myvision\"`
   - 导出宏 / `DESTDIR` 按团队约定
   - `LIBS` 只链 **-lxplugin**
   - 保留 `CONFIG += no_plugin_name_prefix`
3. 改类名 / 文件名，保留 `XPLUGIN_REGISTER(PLUGIN_NAME, YourClass)`。
4. 先只留 `CreateCenterWidget` 空白页，确认 HMI 能打开，再逐步加功能。

### 4.1 `.pro` 必备

```qmake
TEMPLATE = lib
CONFIG += c++11
CONFIG += no_plugin_name_prefix
DEFINES += PLUGIN_NAME=\"YourName\"
INCLUDEPATH += $$PWD/../../include
LIBS += -L$$PWD/../../lib/windows -lxplugin
```

---

## 5. 生命周期：代码放哪

| 阶段 | 放什么 |
|------|----------|
| 构造 | `setPluginLabel` 等轻量初始化 |
| **`init()`** | **必须** 创建并挂载中心页 / 状态栏 / 侧栏 / 物理键；订阅网络信号 |
| **`afterInit()`** | 注册 **RLManager** |
| `idle`/`render` | 可选周期刷新；勿堵 UI 线程 |

### 5.1 最小 `init()` 模板

```cpp
void MyPlugin::init()
{
    auto *page = new MyCenterPage;
    xplugin::CreateCenterWidget(PLUGIN_NAME, page);

    connect(&xplugin::xPluginInterface(),
            &xplugin::InterfaceManager::sigNetworkConnect,
            this, []() { /* 连上后刷新 */ });
}
```

**关键**：HMI / 业务 API 的插件名参数一律用 `PLUGIN_NAME`，与 `.pro` / 注册宏完全一致。

---

## 6. HMI 挂载用法

### 6.1 中心页

```cpp
xplugin::CreateCenterWidget(PLUGIN_NAME, yourWidget);
```

- 一个插件通常只有**一个**中心入口；复杂 UI 在内部 `QStackedWidget` 自行组织。

### 6.2 状态栏

```cpp
xplugin::CreateStatusBar(PLUGIN_NAME, widget, index);
```

- `index` 为槽位，可多次挂载。

### 6.3 右侧拓展

```cpp
xplugin::CreateRightExpandWidgte(PLUGIN_NAME, expandWidget);
xplugin::SwitchRightExpandWidgte(PLUGIN_NAME); // 打开本插件拓展
xplugin::SwitchRightExpandWidgte();            // 关闭
```

- API 历史拼写 `Widgte`。
- 若拓展为空，优先检查宿主是否启用了右侧拓展区域配置。

### 6.4 物理侧键

```cpp
xplugin::CreatePhyButtonList(PLUGIN_NAME, PLUGIN_NAME, leftList, rightList);
xplugin::ChangePhyButtonStatus(PLUGIN_NAME, PLUGIN_NAME, true);
```

- `PhysicalButton` ：文案 / 图标 / `exec_function` 回调。

### 6.5 全屏 / 模态（按需）

```cpp
xplugin::ChangeFullScreen(PLUGIN_NAME, true);
xplugin::ChangeModelWindowStatus(PLUGIN_NAME, true);
```

对照 `pages/xplugin/pagehmishell.cpp`。

---

## 7. 业务 API 怎么用（根着六页学）

```cpp
#include "interface/interfacemanager.h"

xplugin::xPluginInterface().getRobotIP();
xplugin::GetPluginVersion();
```

**学习方法**：打开对应页 -> 点左侧按钮 -> 看右侧 Result -> 对照源码那一行。

| demo 导航 | 源码 | 你要的能力 |
|----------|------|----------------|
| UI Layer | pagehmishell | 版本/路径/拓展/全屏/模态/物理键 |
| Robot Status | pagerobotstatus | 连接/上下电/手自动/运行启停 |
| Project/Point/Var | pageprojectpointvar | 工具工件 / 变量/PERS / 点位 / 工程 |
| IO/Register | pageioregister | IO / 寄存器 |
| XService/RC | pagexservicerc | XService / RC |
| Network/Event/Log | pagenetworkeventlog | 网络/事件/日志 |

### 7.1 调用习惯

1. **先看网络**：`sigNetworkConnect` / `getRobotConnected()`。
2. **别堵 UI**：耗时 RC/命令放工作线程，结果回 UI。
3. **检查返回值**：demo Result 面板就是范例。
4. **写操作要谨慎**：上下电/切模式/写 IO/删工程等，现场必须确认。

### 7.2 常用入口（完整以头文件为准）

```text
机器人: getRobotConnected / getRobotIP / changeRobotPowerStatue / changeRobotModelStatue / startRunning ...
工程点位: getProjectList / createProject / getPointNameList / addCurrentPoint / moveToStart ...
变量: getVarDataList / addVarData / updateVarData / registPERSVars ...
IO: getIOHash / getIOValue / setIOValue / getRegisterHash ...
服务: getXServiceData / setXServiceData / commandXServiceData / connectRcNotify / disconnectRcNotify / rcRequest / rcExecRequest ...
网络事件: connectNetworkStatus / initEventSystem / subscribe / publish / ThrowLog ...
```

头文件: `xplugin_include/include/interface/interfacemanager.h`

---

## 8. 辅助编程 RL 怎么用

在 **`afterInit()`** 中：

1. `createTypeKey` / `createSkeleton`
2. `createInsDescribe` / `createPattern` / `createInsDefault`
3. `createPropertyWidget` / `createInsertWidget` （见 `rl/`）
4. `createTypeToGroup` / `createGroups`

**效果**：HMI 程序编辑器中出现你的自定义指令。对照 `xplugindemo.cpp` `afterInit()`。

---

## 9. 文案 / 样式

- 源码 UI：`tr("English")` + `language/*.ts` -> `.qm`
- 样式: `render/demo_xmate.qss` 可换。
- `setDefaultGlobalSheet(false)` ：是否跟随宿主全局样式。

---

## 10. 编译 / 部署 / 启用

1. 打开 `.pro` -> **Run qmake** -> 编译
2. DLL 放到 `xplugins/client/<PLUGIN_NAME>/` （或团队约定目录）
3. 示教器 **插件管理** 中安装/启用
4. 进入插件界面验证中心页

### 自检

- [ ] `PLUGIN_NAME` 与注册宏、部署目录名一致
- [ ] 只链 xplugin，平台库匹配
- [ ] `init()` 已 `CreateCenterWidget`等等
---

## 11. 可选：Rokae+ SDK

`sdk/demosdkbridge.*` 演示 `getRobotIP()` 再连Rokae+ SDK。

- `HAVE_ROKAE_SDK` **默认关闭**
- 先保证插件能挂上 HMI，再叠加Rokae+ SDK

---

## 12. 建议学习顺序

1. 跑通 demo，点遍六页。
2. 精读 `xplugindemo.cpp` `init`/`afterInit`。
3. 按业务打开 `pages/xplugin/page*.cpp` 复制 API。
4. 复制工程改名，做自己中心页。
5. 需要时再加 RL / 再叠加Rokae+ SDK。

---

## 13. 相关文件

| 文件 | 用途 |
|------|------|
| 本文 | 结构 + 使用总览 |
| `include/interface/interfacemanager.h` | API 声明（以此为准） |
| `include/plugincommon.h` | 基类 / 注册宏 |
| `include/rl/rlmanager.h` | RL 注册 API |

产品级部署差异以现场交付文档为准；API 用法以本示例与 SDK 头文件为准。