# 单架构插件包

未要求多架构时读取本文件。插件目录直接放在 `client/` 或 `controller/` 下。只打包客户端插件时，不建立 `controller/`。只打包控制器插件时，不建立 `client/`。只有一个插件时，不建立第二个插件目录。

示例里的 `MyPlugin` 代指第一个插件名，`OtherPlugin` 代指第二个插件名。目录名和文件名使用已经确认的插件名。示例只列出两个插件，插件包可以放入一个或多个插件。

## Windows 打包工作目录

```text
工作目录/
├── client/
│   ├── MyPlugin/
│   │   ├── MyPlugin.dll
│   │   ├── xplugin.dll
│   │   └── MyPlugin.json
│   └── OtherPlugin/
│       ├── OtherPlugin.dll
│       ├── xplugin.dll
│       └── OtherPlugin.json
├── controller/
│   └── MyPlugin/
│       ├── MyPlugin.lic
│       ├── MyPlugin.so
│       └── MyPlugin.json
└── PluginPackage.bat
```

## Linux 打包工作目录

```text
工作目录/
├── client/
│   ├── MyPlugin/
│   │   ├── MyPlugin.so
│   │   ├── xplugin.so
│   │   └── MyPlugin.json
│   └── OtherPlugin/
│       ├── OtherPlugin.so
│       ├── xplugin.so
│       └── OtherPlugin.json
├── controller/
│   └── MyPlugin/
│       ├── MyPlugin.lic
│       ├── MyPlugin.so
│       └── MyPlugin.json
└── PluginPackage.sh
```

## 生成的插件包

Windows 上，上述工作目录打包生成的插件包如下：

```text
Package.zip
├── client.zip
│   ├── MyPlugin/
│   │   ├── MyPlugin.dll
│   │   ├── xplugin.dll
│   │   └── MyPlugin.json
│   └── OtherPlugin/
│       ├── OtherPlugin.dll
│       ├── xplugin.dll
│       └── OtherPlugin.json
└── controller.zip
    └── MyPlugin/
        ├── MyPlugin.lic
        ├── MyPlugin.so
        └── MyPlugin.json
```

Linux 上，`client.zip` 内的后缀与 Linux 打包工作目录中的客户端文件相同，为 `.so`。只存在 `client/` 时，插件包只含 `client.zip`。只存在 `controller/` 时，插件包只含 `controller.zip`。
