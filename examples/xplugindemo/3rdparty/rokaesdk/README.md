# xCore SDK 下载与配置

xCore SDK 是 `xplugindemo` 的可选依赖，默认未启用。

## 1. 获取 SDK

根据目标控制器和开发环境，获取相应版本的 SDK 头文件、第三方头文件及预编译库。SDK 头文件与库文件的版本应保持一致，库文件需与目标操作系统和处理器架构匹配。

- 官方仓库：[RokaeRobot/xCoreSDK-CPP](https://github.com/RokaeRobot/xCoreSDK-CPP)
- 下载入口：[GitHub Releases](https://github.com/RokaeRobot/xCoreSDK-CPP/releases)

## 2. 配置目录

SDK 文件需放置在 `examples/xplugindemo/3rdparty/rokaesdk/` 下。`include/` 和 `external/` 需完整复制，静态库仅需放置目标平台对应的文件。

以下目录结构仅作为路径示例参考，实际文件及平台目录以所选 SDK 开发包为准。使用不同的平台目录时，需相应调整 `xplugindemo.pro` 中的库路径和库名称。

```text
3rdparty/rokaesdk/
|-- include/                        # SDK 头文件
|   `-- rokae/
|-- external/                       # 第三方头文件
|   `-- Eigen/
`-- lib/                            # 目标平台的库文件
    |-- Windows/Release/64bit/
    |   |-- xCoreSDK_static.lib
    |   `-- xMateModel.lib
    `-- Linux/
        |-- x86_64/
        |   |-- libxCoreSDK.a
        |   `-- libxMateModel.a
        `-- aarch64/
            `-- libxCoreSDK.a
```

## 3. 启用 SDK

示例工程启用 SDK 后使用 C++17 编译。启用前，需选择与目标 HMI 匹配的 Qt 和编译器套件。

按以下步骤启用 SDK：

1. 打开 `xplugindemo.pro`，解开 `# DEFINES += HAVE_ROKAE_SDK` 的注释。
2. 重新运行 qmake 并编译工程。
