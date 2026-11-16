# Rokae+ plugin skills

The three skills are independent. Each skill completes only the work in the skill's own scope. When required input is missing, stop and state the missing items.

| Skill | Purpose | Input | Output |
|---|---|---|---|
| `preparing-client-plugin` | In a directory that does not yet have a plugin project, create a minimal project; mount a page to the center page, the bottom status bar, the right expand pane, or a physical key, according to the location | Plugin name and SDK (xCorePlugin); mounting also requires the page and the target location | Minimal project; mount calls in `init()` |
| `compiling-rokae-plugin` | Compile a client plugin or a controller plugin according to the build environment | Plugin name, target platform, and compiler path; a client plugin also requires `qmake` and the SDK. On Linux, when paths are not given, the search script lists candidates and the user confirms | Absolute path of the plugin library |
| `packaging-rokae-plugin` | Make a plugin package. The package may contain only client plugins, only controller plugins, or both client plugins and controller plugins. Packaging can run on Windows and on Linux | Packaging work directory and plugin libraries. The plugin name is the plugin library file name with `.dll` or `.so` removed. When no plugin library file is present, the user supplies the plugin name | Absolute path of the plugin package zip. The package contains `client.zip`, `controller.zip`, or both |

## Boundaries

- `preparing-client-plugin` does not compile, does not package, and does not write business APIs.
- `compiling-rokae-plugin` does not make a plugin package and does not create a client plugin project.
- `packaging-rokae-plugin` does not compile and does not create a client plugin project.
- When one prompt contains work that belongs to more than one skill, call the skills in order by ownership. After a skill finishes the skill's own part, continue with the next skill. Do not perform another skill's work inside the current skill.
