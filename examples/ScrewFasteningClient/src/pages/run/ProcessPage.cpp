/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ProcessPage.cpp
 * @brief 工艺参数页面。
 */

#include "ProcessPage.h"
#include "core/ui/EditSaveControls.h"
#include "core/ui/LabelColumnAligner.h"
#include "core/controller/RecipeSync.h"
#include "core/controller/RunLock.h"
#include "core/settings/SettingsStore.h"
#include "core/ui/SettingsEditor.h"
#include "core/ui/WidgetFactory.h"
#include "pages/common/PageFieldWidgets.h"
#include "interface/interfacemanager.h"
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QSignalBlocker>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace xplugin {
ProcessPage::ProcessPage(ProcessType type, RecipeSync *recipeSync, QWidget *parent)
    : QWidget(parent), m_section(screw::processSection(type)), m_recipeSync(recipeSync)
{
    setProperty("settingsNeedsParameter", true);
    setProperty(screw::kSettingsSectionProperty, m_section);
}

QComboBox *ProcessPage::buildRecipeForm(const char *pageName, const char *recipeSuffix,
                                        const QString &recipeLabel, const QVector<FormRow> &rows)
{
    setObjectName(QString::fromLatin1(pageName));
    auto *layout = new QVBoxLayout(this);
    QStringList texts;
    texts << recipeLabel;
    for (const FormRow &row : rows)
        texts << row.text;
    const int labelWidth = screw::labelColumnWidth(this, texts);
    installEventFilter(new LabelColumnAligner(this));

    // 配方号保持在参数分组框上方，编号范围为 1 到 30。
    auto *rowNo = new QWidget(this);
    rowNo->setObjectName(QStringLiteral("row_") + QString::fromLatin1(recipeSuffix));
    auto *rowLayout = screw::createNavRow(rowNo);
    auto *label = new QLabel(recipeLabel, rowNo);
    label->setObjectName(QStringLiteral("lbl_") + QString::fromLatin1(recipeSuffix));
    label->setMinimumWidth(labelWidth);
    rowLayout->addWidget(label);
    auto *combo = new QComboBox(rowNo);
    combo->setObjectName(QStringLiteral("combo_") + QString::fromLatin1(recipeSuffix));
    combo->setProperty(screw::kSettingsSelectorProperty, true);
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::NoInsert);
    for (int number = 1; number <= 30; ++number)
        combo->addItem(QString::number(number));
    rowLayout->addWidget(combo, 1);
    layout->addWidget(rowNo);

    auto *group = new QGroupBox(QString(), this);
    group->setObjectName(QStringLiteral("grp_") + QString::fromLatin1(recipeSuffix));
    group->setProperty("func", "no_title");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *groupLayout = new QVBoxLayout(group);
    for (const FormRow &row : rows)
        groupLayout->addWidget(screw::createParamRow(group, row.key, row.text, labelWidth).widget);
    layout->addWidget(group);
    layout->addStretch();
    return combo;
}

void ProcessPage::finishSetup(QComboBox *recipeCombo)
{
    m_recipeCombo = recipeCombo;
    screw::bindSettingsRestore(this, [this]() { restore(); });
    if (m_recipeCombo == nullptr)
        return;
    screw::bindComboTextInput(m_recipeCombo);
    m_recipeCombo->setProperty("activeRecipe", m_recipeCombo->currentText().trimmed());
    connect(m_recipeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this,
            [this](int) { commitRecipe(); });
    connect(m_recipeCombo->lineEdit(), &QLineEdit::editingFinished, this, [this]() { commitRecipe(); });
}

QWidget *ProcessPage::createSaveControls(QWidget *parent)
{
    return screw::createEditSaveControls(
        parent, this,
        tr("保存后会将配方同步到控制器，执行 SetProcess 时才会写入锁丝机寄存器。确定要保存吗？"),
        [this]() { return save(); });
}

void ProcessPage::restore()
{
    int recipe = 0;
    if (m_recipeCombo != nullptr) {
        bool ok = false;
        const int number = m_recipeCombo->property("activeRecipe").toString().toInt(&ok);
        recipe = ok && number >= 1 && number <= 30 ? number : 1;
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(QString::number(recipe));
        m_recipeCombo->setProperty("activeRecipe", QString::number(recipe));
    }
    // 恢复期间不提交选择器事件，防止加载参数时再次触发配方切换。
    m_applying = true;
    const PageFields fields = recipe == 0 ? screw::SettingsStore::loadPage(m_section)
                                          : screw::SettingsStore::loadRecipe(m_section, recipe);
    screw::applyPageFields(this, fields);
    m_applying = false;
}

screw::SaveResult ProcessPage::save()
{
    using screw::SaveStatus;
    const PageFields fields = screw::collectPageFields(this);
    int recipe = 0;
    if (m_recipeCombo != nullptr) {
        bool ok = false;
        recipe = m_recipeCombo->currentText().trimmed().toInt(&ok);
        if (!ok || recipe < 1 || recipe > 30)
            return {SaveStatus::NotSaved, tr("配方号应为 1 到 30。")};
    }
    const bool saved = recipe == 0 ? screw::SettingsStore::savePage(m_section, fields)
                                    : screw::SettingsStore::saveRecipe(m_section, recipe, fields);
    if (!saved)
        return {SaveStatus::NotSaved, {}};

    // 本地保存成功后先更新保存记录，控制器同步失败时仍保留本地配方。
    screw::markSettingsSaved(this);
    QString error;
    const bool synchronized = m_recipeSync->synchronize(error);
    if (!synchronized)
        return {SaveStatus::RecipeSyncIncomplete,
                tr("配方已保存在示教器本地，控制器同步尚未完成。\n%1").arg(error)};
    return {SaveStatus::Saved, {}};
}

void ProcessPage::commitRecipe()
{
    if (m_applying || m_recipeCombo->property("recipeSwitchInProgress").toBool())
        return;
    const QString previous = m_recipeCombo->property("activeRecipe").toString();
    bool ok = false;
    const int number = m_recipeCombo->currentText().trimmed().toInt(&ok);
    if (!ok || number < 1 || number > 30) {
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(previous);
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          tr("配方号应为 1 到 30，已恢复原配方号。"));
        return;
    }
    const QString recipe = QString::number(number);
    if (recipe == previous || screw::RunLock::instance().isLocked()) {
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(previous);
        return;
    }

    // 确认窗口关闭前不重复处理配方切换，页面关闭后不再访问输入控件。
    m_recipeCombo->setProperty("recipeSwitchInProgress", true);
    const QPointer<ProcessPage> pageGuard(this);
    const QPointer<QComboBox> comboGuard(m_recipeCombo);
    // 离开确认需要恢复原配方，确认前先恢复原配方号，避免读取目标配方覆盖当前输入。
    {
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(previous);
    }
    const bool leave = screw::confirmSettingsLeave(this);
    if (!pageGuard || !comboGuard)
        return;
    if (!leave || screw::RunLock::instance().isLocked()) {
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(previous);
    } else {
        const QSignalBlocker blocker(m_recipeCombo);
        m_recipeCombo->setCurrentText(recipe);
        m_recipeCombo->setProperty("activeRecipe", recipe);
        restore();
        screw::markSettingsSaved(this);
    }
    m_recipeCombo->setProperty("recipeSwitchInProgress", false);
}
} // namespace xplugin
