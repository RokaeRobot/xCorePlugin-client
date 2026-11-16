/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SaveValidation.cpp
 * @brief 保存前检查参数行是否填写完整。
 */

#include "SaveValidation.h"
#include "SettingsEditor.h"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QVariant>
#include <QWidget>

namespace screw {

QString settingsValidationError(QWidget *scope)
{
    if (scope == nullptr)
        return QObject::tr("配置页面不可用。");

    // 只检查可配置参数：选择器单独校验编号，只读反馈不要求填写数值。
    bool hasParameter = false;
    for (QComboBox *combo : scope->findChildren<QComboBox *>()) {
        if (!combo->objectName().startsWith(QLatin1String("combo_")) ||
            combo->property(kSettingsReadOnlyProperty).toBool())
            continue;
        const QString key = combo->objectName().mid(6);
        const bool hasRegister = !combo->currentText().trimmed().isEmpty();
        if (combo->property(kSettingsSelectorProperty).toBool()) {
            bool ok = false;
            const int index = combo->currentText().trimmed().toInt(&ok);
            if (!ok || index < 1 || index > 30)
                return QObject::tr("配方号应为 1 到 30。");
            continue;
        }
        QLineEdit *edit = combo->parentWidget()->findChild<QLineEdit *>(QStringLiteral("edit_") + key);
        if (edit == nullptr || edit->property(kSettingsReadOnlyProperty).toBool())
            continue;
        const bool hasValue = !edit->text().trimmed().isEmpty();
        if (hasRegister != hasValue) {
            const auto *label = combo->parentWidget()->findChild<QLabel *>(QStringLiteral("lbl_") + key);
            return QObject::tr("“%1”未填写完整，请%2。")
                .arg(label ? label->text() : key,
                     hasRegister ? QObject::tr("填写设定值") : QObject::tr("选择寄存器"));
        }
        hasParameter = hasParameter || hasRegister;
    }
    if (scope->property("settingsNeedsParameter").toBool() && !hasParameter)
        return QObject::tr("当前配方没有参数，请至少完整填写一行参数。");

    // 曲线需要组数和数据两个来源；两个来源全部清空表示停用曲线配置。
    const auto *count = scope->findChild<QComboBox *>(QStringLiteral("combo_curve_group_count"));
    const auto *data = scope->findChild<QComboBox *>(QStringLiteral("combo_curve_data"));
    if (count && data && count->currentText().trimmed().isEmpty() != data->currentText().trimmed().isEmpty())
        return QObject::tr("扭矩曲线需要同时选择组数寄存器和扭矩角度数据寄存器。");
    return {};
}

} // namespace screw
