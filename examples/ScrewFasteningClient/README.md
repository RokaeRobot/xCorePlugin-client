# ScrewFasteningClient

这是示教器上的螺丝锁附客户端插件。操作人员用它填写锁丝机的预设参数，以及运行时使用的工艺配方，包括扭矩设置、吸式工艺、吹式工艺、螺丝点、IO 输出和 IO 等待。

填写结果保存在示教器本地。插件清单要求把运行控制配方交给同名的控制器插件。界面还可以下发控制指令，查看指令反馈和设备状态，并显示扭矩曲线。程序运行期间，参数编辑和相关设备操作会被限制。

在机器人程序中，插件提供一条“设置工艺配方参数”的 SetProcess 指令，用来选择要执行的工艺。

## 目录结构

```text
ScrewFasteningClient/
├── ScrewFastening.pro      qmake 工程：C++11 插件库，链接 xplugin
├── ScrewFastening.json     插件名称、版本、最低 HMI 版本和控制器依赖
├── ScrewFastening.qrc      打包 resources/icons 里的两个箭头图标
├── resources/icons/        界面用的下拉箭头图
└── src/
    ├── ScrewFastening.cpp  创建六个主页面，并注册 SetProcess 指令
    ├── core/
    │   ├── controller/     配方同步、寄存器读写、运行期间的操作限制
    │   ├── settings/       读写示教器本地 settings.json
    │   └── ui/             菜单、表单、编辑保存、配置导入导出
    ├── models/             页面参数和配方集合的数据结构
    ├── pages/              预设参数、运行控制、控制指令、指令反馈、状态读取、扭矩曲线
    │   ├── preset/         基础、铆钉点、锁附、扭矩测试仪、预警、电机参数
    │   ├── run/            六类运行控制工艺页
    │   └── common/         在页面输入和已保存参数之间传递数据
    └── rl/                 SetProcess 指令编辑页和工艺类型
```
