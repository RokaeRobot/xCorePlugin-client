# Single-architecture plugin package

Read this file when multi-architecture is not required. Place plugin directories directly under `client/` or `controller/`. When packaging only client plugins, do not create `controller/`. When packaging only controller plugins, do not create `client/`. When there is only one plugin, do not create a second plugin directory.

In the examples, `MyPlugin` stands for the first plugin name, and `OtherPlugin` stands for the second plugin name. Directory names and file names use the confirmed plugin names. The examples list only two plugins. A plugin package may contain one or more plugins.

## Windows packaging work directory

```text
work-directory/
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

## Linux packaging work directory

```text
work-directory/
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

## Generated plugin package

On Windows, packaging the work directory above produces the following plugin package:

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

On Linux, suffixes inside `client.zip` match the client files in the Linux packaging work directory, and are `.so`. When only `client/` exists, the plugin package contains only `client.zip`. When only `controller/` exists, the plugin package contains only `controller.zip`.
