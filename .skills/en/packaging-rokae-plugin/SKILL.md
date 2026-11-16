---
name: packaging-rokae-plugin
description: >-
  Use when making a Rokae+ plugin package. The package may contain only client plugins, only controller plugins, or both client plugins and controller plugins. Packaging can run on Windows or Linux. After completion, give the absolute path of the plugin package. Do not compile. Do not create a client plugin project.
compatibility: Requires Python 3 to run scripts/check_stage.py before packaging. Do not install Python and do not search for an interpreter. Windows runs assets/PluginPackage.bat. Linux runs assets/PluginPackage.sh. Packaging does not require a compiler.
metadata:
  version: "1.0"
---

# Make a Rokae+ plugin package

This skill packages already compiled client plugins and controller plugins into a plugin package that can be imported.

## Package contents

HMI imports a plugin-package zip. The zip contains the data packages `client.zip` and `controller.zip`. The package may contain both client plugins and controller plugins, or only client plugins, or only controller plugins. Client plugins and controller plugins have no required dependency on each other.

A plugin package may contain one or more plugins. Multiple plugins may each form a separate plugin package and be imported separately; multiple plugin directories may also stand side by side in the same `client.zip` or `controller.zip` and be imported once.

### Names and files

The directory name, the plugin library file name, the configuration file name, the license file name, and `name` in the configuration use the same plugin name. The file name of `xplugin` is fixed as `xplugin`, and the suffix matches the plugin library. The client plugin name and the controller plugin name may differ. Do not change the client plugin name and the controller plugin name to the same name for packaging.

`win` uses `.dll`. `aarch64` and `amd64` use `.so`. The controller library uses `.so`. Do not place the SDK `include/` in the plugin package.

### Configuration files

In configuration examples and directory examples, `MyPlugin`, `OtherPlugin`, `example`, `plugin1`, and `plugin2` stand for plugin names only.

#### Client plugin

A client configuration example is as follows:

```json
{
  "client_plugin": {
    "name": "MyPlugin",
    "depend": [],
    "ctrlDepend": [],
    "enable": false,
    "version": "3.0.1",
    "min_hmi_version": "3.0.1",
    "custom_hmi_version": [],
    "author": "rokae",
    "description": "description"
  }
}
```

| Field | Required | Description |
|---|---|---|
| `name` | Yes | Plugin name |
| `depend` | No | List of other client plugin names that are depended on. Use an empty array when there is no dependency |
| `ctrlDepend` | No | List of controller plugin names that are depended on |
| `enable` | Yes | Whether to enable by default after import |
| `version` | Yes | Plugin version |
| `min_hmi_version` | No | Lowest supported HMI (xCore) version |
| `custom_hmi_version` | No | List of custom compatible versions |
| `author` | No | Author |
| `description` | No | Description |

When one plugin depends on another plugin, write the depended-on plugin name in `depend` or `ctrlDepend` of the depending plugin's configuration file, for example `"depend": ["OtherPlugin"]`. `depend` is a client dependency. `ctrlDepend` is a controller dependency.

#### Controller plugin

Place an object named with the plugin name under `controller_plugin` in the controller configuration. `name` inside the object uses the same plugin name.

A controller configuration example is as follows:

```json
{
  "controller_plugin": {
    "example": {
      "name": "example",
      "depend": ["plugin1", "plugin2"],
      "enable": true,
      "min_controller_version": "3.0.2",
      "must_controller_version": [],
      "version": "1.0.0",
      "author": "rokae",
      "description": "description info"
    }
  }
}
```

| Field | Required | Description |
|---|---|---|
| `name` | Yes | Plugin name. For the naming constraint, see Names and files |
| `depend` | Required when there is a dependency | List of other controller plugin names that are depended on. Use `[]` when there is no dependency |
| `enable` | Yes | Whether to enable by default after import. `true` enables the plugin. `false` disables it and skips loading |
| `min_controller_version` | No | Lowest controller version required for the plugin to run |
| `must_controller_version` | No | List of controller versions on which the plugin is allowed to run. When non-empty, this overrides `min_controller_version`. Use `[]` when not specified |
| `version` | No | Plugin version |
| `author` | No | Author |
| `description` | No | Description |

## Packaging work directory

The packaging work directory is the directory specified by the user. When the user does not specify a directory, the packaging work directory is the current working directory.

Place `client/`, `controller/`, and the packaging script in the packaging work directory. The packaging script compresses `client/` into `client.zip`, compresses `controller/` into `controller.zip`, and then places those archives into a zip named after the packaging work directory.

## Confirmation items

Before packaging, the project, build directory, and packaging work directory given by the user may be read first, to find missing information. Ask for every missing item at once, then stop. Before confirmation, do not copy files and do not run the packaging script. Confirm the following items before packaging:

1. Packaging work directory.
2. Whether this package contains client plugins, controller plugins, or both client plugins and controller plugins. The AI agent takes the plugin library file name as the plugin name, with `.dll` or `.so` removed. `xplugin.dll` and `xplugin.so` are not plugin names. When more than one plugin library is present, the AI agent writes each plugin name from each library file name. When no plugin library file is present, ask the user to supply the plugin name. Do not guess the plugin name. Do not read a `.pro` file when confirming the plugin name.
3. When packaging client plugins, confirm the architecture, and confirm whether to store files in multi-architecture directories. When packaging only controller plugins, do not confirm the client architecture.

After the items above are confirmed, continue with Arrange files.

## Arrange files

Read the matching example for the confirmed layout, then place files. When multi-architecture is not required, read [references/single-arch.md](references/single-arch.md). When multi-architecture is required, read [references/multi-arch.md](references/multi-arch.md).

Place files for client plugins and controller plugins in the following ways:

### Client plugin

Place the plugin library, `xplugin` with the same suffix, and the plugin name plus `.json`. Copy `xplugin` from `lib/windows` or `lib/aarch64` of the client secondary SDK. When the SDK path is not given, and `xplugin` is missing from the build directory, the plugin project directory, and the packaging work directory, record a missing item. Do not search for the SDK. When the configuration file does not exist, ask first whether to generate the file. `name` uses the client plugin name.

### Controller plugin

Place `.so`, `.json`, and `.lic`. Use the `.lic` file provided by Rokae. Do not generate a `.lic` file. When the configuration file does not exist, ask first whether to generate the file. `name` uses the controller plugin name.

Limit the search to the build directory, plugin project directory, and packaging work directory given by the user, and to the library path already given by compiling. Do not search outside those directories. When one copy is found, copy that file. When more than one copy is found, list the absolute paths and ask the user to choose. Do not copy before confirmation. When none is found, list the target absolute path and ask the user to place the file. Files already complete for the other side may be placed first. When the library file has not been placed, stop in this section.

For `version` and other fields not present in the request, confirm with the user before writing. Move plugin directories not included in this package out of `client/` or `controller/` before running the check.

## Run the packaging script

When the matching script is missing from the packaging work directory, copy the script from this skill's `assets/`. Windows uses [assets/PluginPackage.bat](assets/PluginPackage.bat). Linux uses [assets/PluginPackage.sh](assets/PluginPackage.sh). When a script with the same name already exists, do not overwrite.

First run `scripts/check_stage.py` in this skill directory. Replace the path and plugin names in the command with actual values. When more than one plugin is present, repeat the matching `--client-name` or `--controller-name`. When packaging only controller plugins, do not write `--client-name`, `--client-suffix`, or `--client-arch`. When the user requires multi-architecture, repeat the matching `--client-arch` or `--controller-arch`. When architecture arguments are given, check `client/<arch>/<plugin-name>/` and `controller/<arch>/<plugin-name>/`. `--client-suffix` is then not required.

```bash
python scripts/check_stage.py packaging-work-directory --package-os windows --client-suffix dll --client-name ClientPlugin --controller-name ControllerPlugin
python scripts/check_stage.py packaging-work-directory --package-os linux --client-suffix so --client-name ClientPlugin --controller-name ControllerPlugin
python scripts/check_stage.py packaging-work-directory --package-os linux --client-name MyPlugin --client-arch aarch64 --client-arch amd64 --client-arch win --controller-name MyPlugin --controller-arch aarch64 --controller-arch amd64
```

After the output is `OK`, run the script in the packaging work directory. On Windows, run `PluginPackage.bat`. On Linux, run `bash PluginPackage.sh`. After the script prints `[OK] Done` and the plugin package path, the script waits for a key. Send Enter once to the script. When the output is not `OK`, do not run the packaging script.

When the `python` command is missing, or the version is not Python 3, do not run the check script, do not install Python, do not search for an interpreter outside the packaging work directory, and do not run `PluginPackage.bat` or `PluginPackage.sh`.

## Packaging success

Give the absolute path of the plugin-package zip.

## Packaging failure

When packaging cannot continue, stop at once. First list the absolute paths of missing or mismatched items, then write a brief reason, and list optional actions. Until the user gives further instruction, do not run the packaging script.
