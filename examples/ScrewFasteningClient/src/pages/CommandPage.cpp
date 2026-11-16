/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file CommandPage.cpp
 * @brief 控制指令页面。
 *
 * 配置控制指令使用的寄存器和数值，并提供指令下发操作。
 */

#include "CommandPage.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPointer>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "core/ui/LabelColumnAligner.h"
#include "core/controller/RegisterIo.h"
#include "core/settings/SettingsStore.h"
#include "core/ui/SettingsEditor.h"
#include "core/ui/WidgetFactory.h"
#include "interface/interfacemanager.h"

namespace {
const QLatin1String kCommandSection("command");

struct CommandRowWidgets
{
    screw::ParamRowWidgets inputs;
    QPushButton *setButton;
    QPushButton *resetButton;
};

// 创建参数行时同时返回输入控件和操作按钮，供回调直接使用。
CommandRowWidgets createCommandRow(QWidget *parent, const char *key, const QString &text, int labelWidth,
                                   bool withReset)
{
    auto *row = new QWidget(parent);
    row->setObjectName(QStringLiteral("row_") + QString::fromLatin1(key));
    row->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto *layout = screw::createNavRow(row);

    auto *label = new QLabel(text, row);
    label->setObjectName(QStringLiteral("lbl_") + QString::fromLatin1(key));
    label->setMinimumWidth(labelWidth);
    layout->addWidget(label);

    auto *combo = new QComboBox(row);
    combo->setObjectName(QStringLiteral("combo_") + QString::fromLatin1(key));
    combo->setEditable(true);
    screw::watchRegisterCombo(combo);
    layout->addWidget(combo);

    auto *edit = new QLineEdit(row);
    edit->setObjectName(QStringLiteral("edit_") + QString::fromLatin1(key));
    edit->setValidator(new QRegExpValidator(QRegExp(QStringLiteral("^-?\\d*\\.?\\d*$")), edit));
    layout->addWidget(edit);

    // 每行提供置位按钮；需要写零的指令另提供复位按钮。
    auto *setButton = new QPushButton(QObject::tr("置位"), row);
    layout->addWidget(setButton);
    QPushButton *resetButton = nullptr;
    if (withReset) {
        resetButton = new QPushButton(QObject::tr("复位"), row);
        layout->addWidget(resetButton);
    }
    layout->addStretch();
    return {{row, combo, edit}, setButton, resetButton};
}

} // namespace

xplugin::CommandPage::CommandPage(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("pnl_command_page"));
    setProperty(screw::kSettingsSectionProperty, QString(kCommandSection));
    auto *layout = new QVBoxLayout(this);

    struct GroupSpec
    {
        const char *key;    // 指令分组的控件名称标识。
        QString title;      // 分组标题。
        QStringList rows;   // 分组内各参数行的名称标识。
        QStringList labels; // 与参数行顺序对应的显示文字。
        bool withReset;     // 参数行是否包含复位按钮。
    };

    // 先列出各类指令和对应参数行，再统一建立页面。
    QVector<GroupSpec> groups;
    groups.append({
        "cmd_start",
        tr("启动控制"),
        {"cmd_run", "cmd_pause", "cmd_reset"},
        {tr("运行"), tr("暂停"), tr("复位")},
        false,
    });
    groups.append({
        "cmd_process",
        tr("运行过程命令"),
        {"cmd_rerun", "cmd_skip"},
        {tr("再次执行"), tr("跳过")},
        false,
    });

    // 输出控制按端口编号生成 24 行，其余指令使用各自的名称列表。
    QStringList outputKeys;
    QStringList outputLabels;
    for (int index = 1; index <= 24; ++index) {
        outputKeys.append(QStringLiteral("cmd_output_%1").arg(index));
        outputLabels.append(tr("输出%1").arg(index));
    }
    groups.append({"cmd_output", tr("输出控制"), outputKeys, outputLabels, true});
    groups.append({
        "cmd_clear",
        tr("清零控制"),
        {"cmd_clear_report", "cmd_clear_bit_count", "cmd_clear_nozzle_count", "cmd_clear_filter"},
        {tr("当日报表清0"), tr("披头使用次数清0"), tr("吸嘴使用次数清0"), tr("滤芯清0")},
        true,
    });
    groups.append({
        "cmd_check",
        tr("扭矩校验"),
        {"cmd_torque_check"},
        {tr("扭矩校验")},
        true,
    });
    groups.append({
        "cmd_screwdriver",
        tr("螺丝刀控制"),
        {"cmd_screwdriver_rotate"},
        {tr("螺丝刀旋转")},
        true,
    });
    groups.append({
        "cmd_motion",
        tr("运动指令"),
        {"cmd_motion_start", "cmd_x_axis_pos", "cmd_x_axis_pos_3rd"},
        {tr("运动指令启动"), tr("X轴（推轴）执行坐标"), tr("X轴（推轴）执行坐标(第三位小数)")},
        true,
    });

    // 全部指令行使用同一标签列宽，保持寄存器选择和数值框对齐。
    QStringList allLabels;
    for (const GroupSpec &group : groups) {
        allLabels.append(group.labels);
    }
    const int labelWidth = screw::labelColumnWidth(this, allLabels);
    installEventFilter(new LabelColumnAligner(this));

    // 按分组建立参数行，置位写入当前数值，复位固定写入零。
    for (const GroupSpec &group : groups) {
        auto *box = new QGroupBox(group.title, this);
        box->setObjectName(QStringLiteral("grp_") + QString::fromLatin1(group.key));
        auto *boxLayout = new QVBoxLayout(box);
        for (int index = 0; index < group.rows.size(); ++index) {
            const QString key = group.rows.at(index);
            const auto controls = createCommandRow(box, key.toLatin1().constData(), group.labels.at(index),
                                                   labelWidth, group.withReset);
            const auto row = controls.inputs;
            boxLayout->addWidget(row.widget);
            connect(controls.setButton, &QPushButton::clicked, this,
                    [this, key, row]() { writeRow(key, row, row.valueEdit->text()); });
            if (controls.resetButton != nullptr) {
                connect(controls.resetButton, &QPushButton::clicked, this,
                        [this, key, row]() { writeRow(key, row, QStringLiteral("0")); });
            }
        }
        layout->addWidget(box);
    }
    layout->addStretch();

    // 页面显示时恢复文件配置，离开页面时检查尚未保存的寄存器选择和设定值。
    screw::bindSettingsRestore(this, [this]() { loadStored(); }, false);
}

void xplugin::CommandPage::loadStored()
{
    m_saved = screw::SettingsStore::loadPage(kCommandSection);
    applyToWidgets();
}

void xplugin::CommandPage::applyToWidgets()
{
    // 文件中没有保存的寄存器选择和设定值清空，避免保留上次未保存的输入。
    const QList<QComboBox *> combos = findChildren<QComboBox *>();
    for (QComboBox *combo : combos) {
        if (!combo->objectName().startsWith(QLatin1String("combo_"))) {
            continue;
        }
        const QString key = combo->objectName().mid(6);
        const QString text = m_saved.fields.value(key).registerName;
        const QSignalBlocker blocker(combo);
        const int index = combo->findText(text);
        if (index >= 0) {
            combo->setCurrentIndex(index);
        } else if (combo->isEditable()) {
            combo->setEditText(text);
        } else {
            combo->setCurrentIndex(-1);
        }
    }
    const QList<QLineEdit *> edits = findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        if (qobject_cast<QComboBox *>(edit->parentWidget()) != nullptr ||
            !edit->objectName().startsWith(QLatin1String("edit_"))) {
            continue;
        }
        const QString key = edit->objectName().mid(5);
        const QSignalBlocker blocker(edit);
        edit->setText(m_saved.fields.value(key).value);
    }
}

void xplugin::CommandPage::writeRow(const QString &key, const screw::ParamRowWidgets &widgets,
                                    const QString &value)
{
    QWidget *row = widgets.widget;
    QComboBox *combo = widgets.registerCombo;
    QLineEdit *edit = widgets.valueEdit;
    if (row == nullptr || combo == nullptr || edit == nullptr)
        return;

    // 写入前记录当前行的目标寄存器和数值，不收集其他参数行的输入。
    FieldCell written;
    written.registerName = combo->currentText().trimmed();
    written.value = value.trimmed();
    if (written.registerName.isEmpty()) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error, tr("请选择寄存器"));
        return;
    }
    if (written.value.isEmpty()) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error, tr("请输入数值"));
        return;
    }

    // 设备写入失败时保留原输入和文件配置，复位成功后显示零值。
    const QPointer<QWidget> rowGuard(row);
    if (!RegisterIo::writeValue(written.registerName, written.value) || !rowGuard)
        return;
    {
        const QSignalBlocker comboBlocker(combo);
        const QSignalBlocker editBlocker(edit);
        combo->setCurrentText(written.registerName);
        edit->setText(written.value);
    }

    if (!screw::SettingsStore::saveField(kCommandSection, key, written, m_saved)) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          tr("寄存器已写入，当前行配置未能保存到 settings.json。"));
        return;
    }

    screw::markSettingsSaved(this, row);
}
