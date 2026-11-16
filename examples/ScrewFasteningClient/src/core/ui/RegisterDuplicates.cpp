/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RegisterDuplicates.cpp
 * @brief 重复寄存器检查和保存确认。
 *
 * 保存前统一检查寄存器名称，存在重复时提示用户确认。
 */

#include "RegisterDuplicates.h"

#include "WidgetFactory.h"

#include <QComboBox>
#include <QHash>
#include <QVariant>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace screw {

QStringList duplicateSettingsRegisters(QWidget *scope)
{
    if (scope == nullptr)
        return QStringList();
    QList<QComboBox *> combos = scope->findChildren<QComboBox *>();
    if (auto *combo = qobject_cast<QComboBox *>(scope))
        combos.prepend(combo);

    // 按寄存器名称统计使用次数，只检查配置寄存器的下拉框。
    QHash<QString, int> counts;
    for (QComboBox *combo : combos) {
        if (!combo->property(kRegisterComboProperty).toBool())
            continue;
        const QString name = combo->currentText().trimmed();
        if (!name.isEmpty())
            ++counts[name];
    }

    // 同一个名称只列出一次，排序后用于保存确认提示。
    QStringList duplicates;
    for (auto entry = counts.constBegin(); entry != counts.constEnd(); ++entry) {
        if (entry.value() > 1)
            duplicates.append(entry.key());
    }
    duplicates.sort(Qt::CaseSensitive);
    return duplicates;
}

bool confirmDuplicateSettingsRegisters(QWidget *parent, const QStringList &duplicates, const QString &context)
{
    QDialog dialog(parent);
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setObjectName(QStringLiteral("register_duplicates_dialog"));
    dialog.setWindowTitle(QObject::tr("确认保存"));
    dialog.setWindowModality(Qt::WindowModal);
    auto *layout = new QVBoxLayout(&dialog);

    // 列出重复使用的风险和寄存器名称，供用户判断是否继续保存。
    auto *warning = new QLabel(
        QObject::tr("检测到重复使用的寄存器，保存后可能导致数据覆盖或异常。是否继续保存？"), &dialog);
    warning->setObjectName(QStringLiteral("lbl_duplicate_warning"));
    warning->setTextFormat(Qt::PlainText);
    warning->setWordWrap(true);
    layout->addWidget(warning);
    auto *names =
        new QLabel(QObject::tr("重复寄存器：%1").arg(duplicates.join(QStringLiteral("、"))), &dialog);
    names->setObjectName(QStringLiteral("lbl_duplicate_names"));
    names->setTextFormat(Qt::PlainText);
    names->setWordWrap(true);
    layout->addWidget(names);
    if (!context.isEmpty()) {
        auto *description = new QLabel(context, &dialog);
        description->setTextFormat(Qt::PlainText);
        description->setWordWrap(true);
        layout->addWidget(description);
    }

    // 默认选中取消，用户明确点击确定后才允许继续保存。
    auto *buttons = new QDialogButtonBox(&dialog);
    auto *ok = buttons->addButton(QObject::tr("确定"), QDialogButtonBox::AcceptRole);
    auto *cancel = buttons->addButton(QObject::tr("取消"), QDialogButtonBox::RejectRole);
    ok->setObjectName(QStringLiteral("btn_duplicate_ok"));
    cancel->setObjectName(QStringLiteral("btn_duplicate_cancel"));
    cancel->setDefault(true);
    layout->addWidget(buttons);
    QObject::connect(ok, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.setMinimumWidth(360);
    dialog.setMaximumWidth(640);
    return dialog.exec() == QDialog::Accepted;
}

} // namespace screw
