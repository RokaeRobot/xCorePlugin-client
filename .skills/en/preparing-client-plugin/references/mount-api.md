# Interfaces for mounting on the teach pendant

## CreateCenterWidget

```cpp
int CreateCenterWidget(const QString &pluginName, QWidget *widget);
```

Register `widget` as the center main view of the plugin. The usual call site is `init()`. Return `-1` when `pluginName` is empty. Return `-2` when the matching plugin module is not found. Return `0` after the call completes. When `widget` is a null pointer, ignore the registration and still return `0`. `pluginName` is the plugin identifier and must match `PLUGIN_NAME`. `widget` is the center main-view control.

## CreateStatusBar

```cpp
int CreateStatusBar(const QString &pluginName, QWidget *widget, int index = 1);
```

Add `widget` to the specified slot of the bottom status bar. The same plugin may call the function more than once with different `index` values to add more than one control. Return `-1` when `pluginName` is empty. Return `-2` when the matching plugin module is not found. Return `0` when `widget` is a null pointer or the add fails. Return `1` when the add succeeds. `pluginName` is the plugin identifier and must match `PLUGIN_NAME`. `widget` is the status-bar control. `index` is the slot index, default `1`.

## CreateRightExpandWidgte

```cpp
int CreateRightExpandWidgte(const QString &pluginName, QWidget *widget, const QString &name = "");
int SwitchRightExpandWidgte(const QString &pluginName = "", const QString &name = "");
```

Register `widget` as a right expand-pane page. Return `-1` when `pluginName` is empty. Return `-2` when the matching plugin module is not found. Return `-1` when registration fails (including an unavailable expand sidebar, a null `widget`, and similar cases). Return `0` on success. `pluginName` is the plugin identifier and must match `PLUGIN_NAME`. `widget` is the expand-pane control. `name` is the name of that expand-pane page, used later by `SwitchRightExpandWidgte()` when switching. When `name` is empty, use the plugin name. Keep the function name as in the header. Do not change `Widgte` to `Widget`. When the user has not asked to open the pane, do not call `SwitchRightExpandWidgte` in `init()`.

## CreatePhyButtonList

```cpp
struct PhysicalButton {
    QString text;
    QString touched_icon;
    QString untouched_icon;
    std::function<void(bool is_pressed)> exec_function;
};

int CreatePhyButtonList(const QString &pluginName, const QString &type,
                        const QList<PhysicalButton> &left_buttons,
                        const QList<PhysicalButton> &right_buttons,
                        QStringList center_texts = QStringList(""));
int ChangePhyButtonStatus(const QString &pluginName, const QString &type, bool enable);
```

Register a group of physical keys with HMI. Return `-1` when `pluginName` is empty. Return `-2` when the matching plugin module is not found. Return `0` on success. `pluginName` is the plugin identifier and must match `PLUGIN_NAME`. `type` is the key-group type, used later by `ChangePhyButtonStatus()`. `left_buttons` and `right_buttons` are the left and right key lists. `center_texts` is the list of texts in the center area. When that argument is omitted, the default is a list that contains one empty string. Use only indexes 0 through 5. When `type` is not specified separately, use `PLUGIN_NAME`. When the user does not supply an action for a key press, write `exec_function` as an empty function, for example `[](bool) {}`. After registration, call `ChangePhyButtonStatus` to enable the keys.
