---
name: compiling-rokae-plugin
description: >-
  Use when compiling a Rokae+ client plugin or controller plugin. A client plugin is Windows x86_64 or Linux aarch64. A controller plugin is Linux x86_64 or Linux aarch64. When compiling an aarch64 build, a cross toolchain on Linux x86_64 is commonly used. After a successful compile, give the absolute path of the plugin library. Do not make a plugin package. Do not create a client plugin project.
compatibility: Requires a C++ toolchain for the selected platform. Client plugins also require Qt 5 qmake. Controller plugins do not require Qt.
metadata:
  version: "1.0"
---

# Compile a Rokae+ plugin

This skill compiles a client plugin or a controller plugin.

## Build environment

A client plugin has two builds: Windows x86_64 and Linux aarch64. Linux aarch64 is the teach-pendant build.

A controller plugin has only Linux x86_64 and Linux aarch64.

When the user request already states the build, and the plugin, system, and architecture are on the same row in the table, use that row. When the request does not state the build, ask for the build under Confirmation items, then use the matching row. When the plugin, system, and architecture specified by the user are not on the same row in the table, stop compiling and ask the user to choose one of the four rows. Do not compile before confirmation. Do not recombine the plugin, system, or architecture from different rows.

| Plugin | System | Architecture | Official recommended toolchain | Delivery |
|---|---|---|---|---|
| Client plugin | Windows | x86_64 | MSVC 2019 (toolset v142) or later. Qt 5.12 and later 5.x. Do not use Qt 6 | `.dll` |
| Client plugin | Linux (teach pendant) | aarch64 | GCC 7.4.1 or later. Qt 5.12.11 and later 5.x. Do not use Qt 6 | `.so` |
| Controller plugin | Linux | x86_64 | gcc-5.4 / g++-5.4 (Ubuntu 20.04) | `.so` |
| Controller plugin | Linux | aarch64 | gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu (Ubuntu 20.04) | `.so` |

The toolchains in the table are the official recommendation. A toolchain outside the table may be considered when compatible with the plugin host program, and only after the user confirms. Do not use Qt 6, because Qt 6 is incompatible with the plugin host program. This skill does not set up a build environment on its own.

## Confirmation items

Before compiling, the given project and paths may be read first, to find missing information. Ask for every missing item in the same reply, then stop. Before confirmation, do not modify the plugin project and do not compile. Confirm the following items before compiling:

1. Plugin kind and plugin name. Confirm whether this compile is a client plugin or a controller plugin. Read the client plugin name from `PLUGIN_NAME` and `XPLUGIN_REGISTER` in the `.pro` file. Read the controller plugin name from the registered name used as the dynamic-library stem in the project.
   - When the project does not contain a readable plugin name: ask the user to supply the name. Do not guess the plugin name. Do not create a project. Creating a project belongs to `preparing-client-plugin`.
   - When names in the project are not consistent: list the original values from each place, and ask the user to specify the name to keep. Do not modify files before confirmation.
   - When the user explicitly asks to rename: list the registered name, the dynamic-library stem, and the configuration file name. Do not modify files before confirmation. After confirmation, change to the new name, then compile. Do not write a client plugin name into a controller project. Do not write a controller plugin name into a client project.
2. Compile build. Confirm the build for this compile, and use the matching row in Build environment. When the request does not state the build, ask the user to specify. When the plugin, system, and architecture specified by the user are not on the same row in the table, stop compiling and ask the user to choose one of the four rows.
3. Secondary SDK path.
   - Client plugin: confirm the client secondary SDK path. The SDK contains `include/`, and `lib/windows` or `lib/aarch64`.
   - Controller plugin: confirm the controller secondary SDK path. The SDK contains `xcore_api/`. Compiling a controller plugin does not require a Qt environment.

After the items above are confirmed, continue with Obtain the build environment.

## Obtain the build environment

Obtain the compiler path for the row already confirmed in Confirmation items. A client plugin also requires a `qmake` path. When a path has not been obtained, stop compiling and ask the user to supply the missing path.

### Windows

The AI agent decides how to determine the compiler path and the `qmake` path. This skill does not allow a full-disk scan. After obtaining the paths, the AI agent writes `compiling-rokae-plugin/.local/windows-build-env.json` under this skill directory. When this conversation has already written the compiler path and the `qmake` path into `compiling-rokae-plugin/.local/windows-build-env.json`, prefer the compiler path and the `qmake` path in `compiling-rokae-plugin/.local/windows-build-env.json`. Do not write the record into the plugin project, and do not write the record into `compiling-rokae-plugin/.local/linux-build-env.json`.

The content template of `compiling-rokae-plugin/.local/windows-build-env.json` is as follows:

```json
{
  "compiler": "<compiler-path>",
  "qmake": "<qmake-path>"
}
```

### Linux

Obtain the compiler path on Linux. Teach-pendant Linux also requires a `qmake` path. The record file is `compiling-rokae-plugin/.local/linux-build-env.json` under this skill directory. Do not write the record into the plugin project.

`scripts/find_linux_build_env.sh` under this skill directory lists candidate paths, runs only on Linux, and does not write the record file. When candidates need to be listed, run the following in this skill directory:

```bash
sh scripts/find_linux_build_env.sh
```

Limit the search to `/usr`, `/usr/local`, and `/opt`, and print candidates that match the official recommendation. This skill does not allow searching for compilers outside the script, and does not allow searching other directories instead. Do not search for the SDK. This skill does not allow a full-disk scan, and does not choose a directory because the directory is a common install location. When the script has no matching candidate, ask the user to supply a path, and stop searching. After the user confirms that another compiler is compatible with the plugin host program, the path given by the user may also be written into the record.

After obtaining the paths, the AI agent writes `compiling-rokae-plugin/.local/linux-build-env.json` under this skill directory. The content template of `compiling-rokae-plugin/.local/linux-build-env.json` is as follows. Write versions not yet obtained as `null`. When the record file already exists, update only the key for this build. Do not rewrite the other two keys. Teach-pendant Linux must also write the `qmake` path.

```json
{
  "pendant-aarch64": {"qmake": "<qmake-path>", "gcc": "<gcc-path>", "gxx": "<g++-path>"},
  "controller-x86_64": {"gcc": "<gcc-path>", "gxx": "<g++-path>"},
  "controller-aarch64": {"gcc": "<gcc-path>", "gxx": "<g++-path>"}
}
```

Judge in the following order. When an earlier item is already satisfied, do not continue to later items.

1. The record file already has the path for this build: use the path in the record.
2. This conversation has already obtained a compiler path, and the record file has not yet written this build: the AI agent writes the record file, then uses the path in the record.
3. This conversation has already listed candidate paths, and no path has been selected: after the user selects one path, the AI agent writes the record file.
4. The record file does not have this build, and this conversation has no candidate path: run the search script to list candidates. After the user selects one path, the AI agent writes the record file.

After the compiler path has been obtained, continue with Compile. A client plugin must also have obtained a `qmake` path.

## Compile

Compile Release by default. When the user has not explicitly asked for Debug, do not compile Debug. Compile only the plugin kind confirmed this time. When the directory already contains a file named with the plugin name plus `.dll` or `.so`, this compile overwrites that file. Do not skip the compile because the directory already contains a plugin library named with the plugin name plus `.dll` or `.so`. After a successful compile, give the absolute path of the plugin library in the build directory or the project directory.

Compile a client plugin and a controller plugin in the following ways:

### Client plugin

In the project directory, use `qmake` with `CONFIG+=release`.

- Windows: in the same command line, prepare the MSVC environment first, then run `qmake`, then run `nmake` or `jom`.
- Teach-pendant Linux: the teach pendant is aarch64. When the compile machine is Linux x86_64, use cross compilation. Use the obtained aarch64 `qmake` and GCC, then run `make`.

### Controller plugin

In the project directory, use CMake, and specify `-DCMAKE_BUILD_TYPE=Release` at configure time.

- Linux x86_64: point `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` at the obtained gcc and g++.
- Linux aarch64: the controller is aarch64. When the compile machine is Linux x86_64, use cross compilation, and point `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` at the obtained gcc and g++.

## Compile success

When the user also asks to make a plugin package, later packaging steps are performed by `packaging-rokae-plugin`.

## Compile failure

When the compile fails, stop at once. First paste an excerpt of the error, then write a brief analysis, and list optional fixes. Until the user gives further instruction, do not modify project files, do not change the build environment, and do not compile again.
