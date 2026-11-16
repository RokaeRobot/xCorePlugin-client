---
name: preparing-client-plugin
description: >-
  Use when initializing a Rokae+ client plugin project, or when mounting a page on the teach pendant. When no plugin project exists, create a minimal project and use include/ and lib/ from xCorePlugin. The center page calls CreateCenterWidget, the bottom status bar calls CreateStatusBar, the right expand pane calls CreateRightExpandWidgte, and physical keys call CreatePhyButtonList. Do not compile, do not package, and do not write business APIs.
compatibility: Requires Python 3 only when creating the minimal project, to run scripts/create_minimal_project.py. Do not install Python and do not search for an interpreter. Requires the Rokae+ client plugin SDK (xCorePlugin) with include/ and lib/. Target Qt is 5.12 or any later 5.x release. Do not use Qt 6.
metadata:
  version: "1.0"
---

# Prepare a client plugin

This skill covers two tasks: initialize a minimal project, and mount a page.

This skill does not apply to compiling, packaging, or writing business APIs.

The project uses Qt 5.12 and later 5.x. Do not use Qt 6, because Qt 6 is incompatible with the plugin host program.

## Project directory

The project directory is the directory specified by the user. When the user does not specify a project directory, the project directory is the current working directory. Place the `.pro` file in the project directory. Place header files and source files in `src/` under the project directory.

## Initialize a minimal project

### Creation conditions

When the project directory already contains a plugin project's `.pro` file, or already contains header files and source files that implement a plugin, do not create a new project. Mounting a page may still proceed. `include/` and `lib/` belong to the SDK and are not treated as plugin project files. When the project directory contains only the SDK, a project may still be created.

### Confirmation items

Confirm the following three items before initialization. Ask for every missing item in the same reply. Before the user confirms the three items, do not run `git init`, do not write project files, do not extract archives, and do not copy the SDK.

1. Plugin name. Use the same name as `PLUGIN_NAME` and as the C++ class name, for example `MyPlugin`. When the name does not follow C++ class name rules, ask the user to provide a plugin name that follows those rules.
2. SDK. Check the project directory first, then ask which preparation method to use. Do not search for the SDK outside the project directory. When more than one source exists at the same time, list every source and ask the user to choose one.
   - The project directory already has `include/` and `lib/`: ask whether to use these two directories directly.
   - The project directory contains a folder whose name includes `xCorePlugin`, and that folder contains `include/` and `lib/`: ask whether to copy `include/` and `lib/` from that folder into the project directory.
   - The project directory contains an archive whose file name includes `xCorePlugin`: ask whether to extract the archive and place `include/` and `lib/` from the archive into the project directory.
   - None of the sources above is present: ask for the SDK path.
3. Whether to use git for version control.

### Creation steps

After the user confirms, run the following steps in order.

1. When the user agrees to use git, and the project directory is not yet a Git repository: run `git init` in the project directory first.
2. Prepare the SDK in the way the user confirmed. When the user confirms using the existing `include/` and `lib/` in the project directory directly, do not copy again.
3. In this skill directory, run the following command to generate the `.pro` file and the header and source files in `src/`. Replace `MyPlugin` in the command with the confirmed plugin name, and replace `project-directory` with the actual path.

   ```bash
   python scripts/create_minimal_project.py --name MyPlugin --dir project-directory
   ```

   When the user supplies an argument for `setPluginLabel`, append `--label` at the end of the command, and write the argument after `--label`. When the user does not supply an argument for `setPluginLabel`, do not append `--label`. The script uses the plugin name as the argument of `setPluginLabel`.

   When the `python` command is missing, or the version is not Python 3, do not run the generator script, do not install Python, do not search for an interpreter outside the project directory, do not write the `.pro` file, header files, and source files by hand, do not run step 4 below, and do not proceed to a local commit. Keep any `git init` already run and any SDK already prepared.

4. List the files created this time, and ask the user to confirm the file contents.

## Mount a page

When a page needs to be mounted, the project directory must already contain a plugin class. When the project directory does not yet contain a plugin class, do not mount a page. When a plugin class already exists, read [references/mount-api.md](references/mount-api.md) and choose a function from the table below.

| Location | Function |
|---|---|
| Process package page, center page | `CreateCenterWidget` |
| Bottom status bar | `CreateStatusBar` |
| Right expand pane | `CreateRightExpandWidgte` |
| Physical key, teach-pendant side key | `CreatePhyButtonList` |

Call the function in the plugin class `init()`. Pass `PLUGIN_NAME` as the `pluginName` argument of the function in the table. After the change, list the edits and ask the user to confirm.

## Local commit

After initializing a minimal project or mounting a page, run the following steps only when the project directory is a Git repository.

1. After the user confirms the current edits, ask whether to commit to the local repository.
2. After the user agrees to commit, run `git add` and `git commit`.

When the user has not asked to push, do not run `git push`.
