# 多架构插件包

用户明确要求多架构时读取本文件。只建立本次确认的架构目录。客户端架构目录为 `aarch64`、`amd64`、`win`。控制器架构目录为 `aarch64` 和 `amd64`。多架构插件包自插件 1.0.2.6 起支持，配套 xCore 控制系统 v3.2.2 及以上。

示例里的 `MyPlugin` 代指第一个插件名，`OtherPlugin` 代指同一架构下的第二个插件名。目录名和文件名使用已经确认的插件名。示例只列出两个插件，同一架构下可以放入一个或多个插件。只确认一个插件时，每个架构目录下只建立一个插件目录。

## 打包工作目录

```text
工作目录/
├── client/
│   ├── aarch64/
│   │   ├── MyPlugin/
│   │   │   ├── MyPlugin.so
│   │   │   ├── xplugin.so
│   │   │   └── MyPlugin.json
│   │   └── OtherPlugin/
│   │       ├── OtherPlugin.so
│   │       ├── xplugin.so
│   │       └── OtherPlugin.json
│   ├── amd64/
│   │   └── MyPlugin/
│   │       ├── MyPlugin.so
│   │       ├── xplugin.so
│   │       └── MyPlugin.json
│   └── win/
│       ├── MyPlugin/
│       │   ├── MyPlugin.dll
│       │   ├── xplugin.dll
│       │   └── MyPlugin.json
│       └── OtherPlugin/
│           ├── OtherPlugin.dll
│           ├── xplugin.dll
│           └── OtherPlugin.json
├── controller/
│   ├── aarch64/
│   │   └── MyPlugin/
│   │       ├── MyPlugin.lic
│   │       ├── MyPlugin.so
│   │       └── MyPlugin.json
│   └── amd64/
│       └── MyPlugin/
│           ├── MyPlugin.lic
│           ├── MyPlugin.so
│           └── MyPlugin.json
└── PluginPackage.bat
```

Linux 上，打包脚本为同位置的 `PluginPackage.sh`。

## 生成的插件包

```text
Package.zip
├── client.zip
│   ├── aarch64/
│   │   ├── MyPlugin/
│   │   │   ├── MyPlugin.so
│   │   │   ├── xplugin.so
│   │   │   └── MyPlugin.json
│   │   └── OtherPlugin/
│   │       ├── OtherPlugin.so
│   │       ├── xplugin.so
│   │       └── OtherPlugin.json
│   ├── amd64/
│   │   └── MyPlugin/
│   │       ├── MyPlugin.so
│   │       ├── xplugin.so
│   │       └── MyPlugin.json
│   └── win/
│       ├── MyPlugin/
│       │   ├── MyPlugin.dll
│       │   ├── xplugin.dll
│       │   └── MyPlugin.json
│       └── OtherPlugin/
│           ├── OtherPlugin.dll
│           ├── xplugin.dll
│           └── OtherPlugin.json
└── controller.zip
    ├── aarch64/
    │   └── MyPlugin/
    │       ├── MyPlugin.lic
    │       ├── MyPlugin.so
    │       └── MyPlugin.json
    └── amd64/
        └── MyPlugin/
            ├── MyPlugin.lic
            ├── MyPlugin.so
            └── MyPlugin.json
```
