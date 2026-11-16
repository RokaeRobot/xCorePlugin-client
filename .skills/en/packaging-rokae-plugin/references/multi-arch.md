# Multi-architecture plugin package

Read this file when the user explicitly requires multi-architecture. Create only the architecture directories confirmed this time. Client architecture directories are `aarch64`, `amd64`, and `win`. Controller architecture directories are `aarch64` and `amd64`. Multi-architecture plugin packages are supported from plugin 1.0.2.6, with xCore control system v3.2.2 and later.

In the examples, `MyPlugin` stands for the first plugin name, and `OtherPlugin` stands for the second plugin name under the same architecture. Directory names and file names use the confirmed plugin names. The examples list only two plugins. One architecture directory may contain one or more plugins. When only one plugin is confirmed, create only one plugin directory under each architecture directory.

## Packaging work directory

```text
work-directory/
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

On Linux, the packaging script at the same location is `PluginPackage.sh`.

## Generated plugin package

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
