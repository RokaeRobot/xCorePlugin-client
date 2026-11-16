# 挂载到示教器的接口

## CreateCenterWidget

```cpp
int CreateCenterWidget(const QString &pluginName, QWidget *widget);
```

将 `widget` 注册为该插件的中心主界面。通常于 `init()` 中调用。`pluginName` 为空时返回 `-1`；未找到对应插件模块时返回 `-2`；调用完成后返回 `0`。`widget` 为空指针时本次注册被忽略，仍返回 `0`。`pluginName` 为插件标识，须与 `PLUGIN_NAME` 一致。`widget` 为中心主界面控件。

## CreateStatusBar

```cpp
int CreateStatusBar(const QString &pluginName, QWidget *widget, int index = 1);
```

将 `widget` 加入底部状态栏的指定槽位。同一插件可按不同 `index` 多次调用，以加入多个控件。`pluginName` 为空时返回 `-1`；未找到对应插件模块时返回 `-2`；`widget` 为空指针或加入失败时返回 `0`；加入成功时返回 `1`。`pluginName` 为插件标识，须与 `PLUGIN_NAME` 一致。`widget` 为状态栏控件。`index` 为槽位索引，默认 `1`。

## CreateRightExpandWidgte

```cpp
int CreateRightExpandWidgte(const QString &pluginName, QWidget *widget, const QString &name = "");
int SwitchRightExpandWidgte(const QString &pluginName = "", const QString &name = "");
```

将 `widget` 注册为右侧扩展窗页。`pluginName` 为空时返回 `-1`；未找到对应插件模块时返回 `-2`；注册失败（含扩展侧栏不可用、`widget` 为空等）时返回 `-1`；成功返回 `0`。`pluginName` 为插件标识，须与 `PLUGIN_NAME` 一致。`widget` 为扩展窗控件。`name` 为该扩展窗页的名称，供后续 `SwitchRightExpandWidgte()` 切换时使用，为空时使用插件名。函数名与头文件原文一致，禁止把 `Widgte` 改成 `Widget`。用户未要求打开时，禁止在 `init()` 中调用 `SwitchRightExpandWidgte`。

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

向 HMI 注册一组物理按键。`pluginName` 为空时返回 `-1`；未找到对应插件模块时返回 `-2`；成功返回 `0`。`pluginName` 为插件标识，须与 `PLUGIN_NAME` 一致。`type` 为按键组类型，供后续 `ChangePhyButtonStatus()` 使用。`left_buttons`、`right_buttons` 分别为左、右侧按键列表。`center_texts` 为中间区域文本列表。省略该参数时，默认值为仅含一个空字符串的列表。下标只使用 0 至 5。`type` 未另行指定时使用 `PLUGIN_NAME`。用户未给出按下后的动作时，`exec_function` 写成空函数，例如 `[](bool) {}`。注册后调用 `ChangePhyButtonStatus` 启用。
