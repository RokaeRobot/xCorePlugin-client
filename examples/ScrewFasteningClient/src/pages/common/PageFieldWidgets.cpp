/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PageFieldWidgets.cpp
 * @brief 在页面控件和设置存储之间传递参数。
 */

#include "PageFieldWidgets.h"
#include "core/settings/SettingsStore.h"
#include "core/ui/WidgetFactory.h"
#include "core/ui/SettingsEditor.h"
#include <QComboBox>
#include <QLineEdit>
#include <QSignalBlocker>
#include <QVariant>
#include <QWidget>

namespace screw {
PageFields collectPageFields(QWidget *page)
{
    PageFields data;
    for (QComboBox *combo : page->findChildren<QComboBox *>()) {
        if (combo->property(kRegisterComboProperty).toBool())
            data.fields[combo->objectName().mid(6)].registerName = combo->currentText();
    }
    for (QLineEdit *edit : page->findChildren<QLineEdit *>()) {
        if (qobject_cast<QComboBox *>(edit->parentWidget()) != nullptr ||
            edit->property(kSettingsReadOnlyProperty).toBool() ||
            !edit->objectName().startsWith(QLatin1String("edit_")))
            continue;
        data.fields[edit->objectName().mid(5)].value = edit->text();
    }
    return data;
}

void applyPageFields(QWidget *page, const PageFields &fields)
{
    for (QComboBox *combo : page->findChildren<QComboBox *>()) {
        if (!combo->property(kRegisterComboProperty).toBool())
            continue;
        const QString text = fields.fields.value(combo->objectName().mid(6)).registerName;
        const int index = combo->findText(text);
        const QSignalBlocker blocker(combo);
        if (index >= 0)
            combo->setCurrentIndex(index);
        else if (combo->isEditable())
            combo->setEditText(text);
        else
            combo->setCurrentIndex(-1);
    }
    for (QLineEdit *edit : page->findChildren<QLineEdit *>()) {
        if (qobject_cast<QComboBox *>(edit->parentWidget()) != nullptr ||
            edit->property(kSettingsReadOnlyProperty).toBool() ||
            !edit->objectName().startsWith(QLatin1String("edit_")))
            continue;
        const QSignalBlocker blocker(edit);
        edit->setText(fields.fields.value(edit->objectName().mid(5)).value);
    }
}

SaveResult savePageSettings(QWidget *page)
{
    const QString section = page->property(kSettingsSectionProperty).toString();
    if (section.isEmpty() || !SettingsStore::savePage(section, collectPageFields(page)))
        return {SaveStatus::NotSaved, {}};

    markSettingsSaved(page);
    return {SaveStatus::Saved, {}};
}

void restorePageSettings(QWidget *page)
{
    const QString section = page->property(kSettingsSectionProperty).toString();
    if (!section.isEmpty())
        applyPageFields(page, SettingsStore::loadPage(section));
}
} // namespace screw
