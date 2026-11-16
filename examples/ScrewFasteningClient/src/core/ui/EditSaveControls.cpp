/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file EditSaveControls.cpp
 * @brief 处理共用的编辑、保存确认和结果提示。
 */

#include "EditSaveControls.h"

#include "pages/common/PageFieldWidgets.h"
#include "RegisterDuplicates.h"
#include "core/controller/RunLock.h"
#include "SaveFeedback.h"
#include "SaveValidation.h"
#include "SettingsEditor.h"
#include "WidgetFactory.h"

#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>

namespace screw {
namespace {

// 保存流程结束时清除进行标记，期间阻止重复保存和页面切换。
class SaveAttempt
{
public:
    explicit SaveAttempt(QWidget *scope) : m_scope(scope)
    {
        scope->setProperty("saveInProgress", true);
    }

    ~SaveAttempt()
    {
        if (m_scope)
            m_scope->setProperty("saveInProgress", false);
    }

private:
    QPointer<QWidget> m_scope;
};

void editOrSave(QPushButton *button, QWidget *scope, const QString &confirmText,
                const std::function<SaveResult()> &saveAction)
{
    if (scope == nullptr || scope->property("saveInProgress").toBool())
        return;

    if (RunLock::instance().isLocked()) {
        QMessageBox::information(
            button->window(), QObject::tr("提示"),
            QObject::tr("程序运行中，暂不能编辑或保存设置。请停止程序后再操作。"));
        return;
    }

    if (!settingsEditing(scope)) {
        setSettingsEditing(scope, true);
        return;
    }

    const QPointer<QPushButton> buttonGuard(button);
    const QPointer<QWidget> scopeGuard(scope);
    SaveAttempt attempt(scope);
    const QString validationError = settingsValidationError(scope);
    if (!validationError.isEmpty()) {
        showSaveFailure(button->window(), validationError);
        return;
    }

    const QStringList duplicates = duplicateSettingsRegisters(scope);
    const bool confirmed =
        duplicates.isEmpty()
            ? QMessageBox::question(button->window(), QObject::tr("确认"),
                                    confirmText.isEmpty() ? QObject::tr("确定要保存吗？") : confirmText,
                                    QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Ok
            : confirmDuplicateSettingsRegisters(button->window(), duplicates, confirmText);
    if (!confirmed || !buttonGuard || !scopeGuard)
        return;

    // 等待用户确认期间运行状态可能更新，保存前重新检查。
    if (RunLock::instance().isLocked()) {
        QMessageBox::information(button->window(), QObject::tr("提示"),
                                 QObject::tr("程序运行中，暂不能保存设置。请停止程序后再操作。"));
        return;
    }

    const SaveResult result = saveAction ? saveAction() : savePageSettings(scope);
    if (!buttonGuard || !scopeGuard)
        return;

    // 本地成功后由保存函数结束编辑；后续同步或设备写入失败只报告未完成步骤。
    switch (result.status) {
    case SaveStatus::Saved:
        setSettingsEditing(scope, false);
        showSaveSuccess(button->window());
        break;
    case SaveStatus::NotSaved:
        showSaveFailure(button->window(), result.detail.isEmpty()
                                              ? QObject::tr("设置未能写入插件目录中的 settings.json。")
                                              : result.detail);
        break;
    case SaveStatus::RecipeSyncIncomplete:
        showSaveFailure(button->window(), result.detail, QObject::tr("配方同步未完成"));
        break;
    case SaveStatus::RegisterWriteIncomplete:
        showSaveFailure(button->window(), result.detail, QObject::tr("设备写入未完成"));
        break;
    }
}

} // namespace

QWidget *createEditSaveControls(QWidget *parent, QWidget *settingsScope, const QString &confirmText,
                                const std::function<SaveResult()> &saveAction)
{
    auto *controls = new QWidget(parent);
    auto *layout = new QVBoxLayout(controls);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto *cancel = new QPushButton(QObject::tr("取消"), controls);
    cancel->setObjectName(QStringLiteral("btn_cancel_edit"));
    cancel->setProperty(kSettingsManagedProperty, true);
    cancel->setStyleSheet(QStringLiteral("QPushButton { background-color: #FFFFFF; color: #254E73;"
                                         " border: 1px solid #254E73; }"
                                         "QPushButton:pressed { background-color: #E9F0F7; }"));
    layout->addWidget(cancel);
    auto *button = createSaveButton(controls);
    button->setObjectName(QStringLiteral("btn_edit_save"));
    layout->addWidget(button);
    const QPointer<QWidget> scope(settingsScope);
    QObject::connect(cancel, &QPushButton::clicked, cancel, [scope]() { cancelSettingsEditing(scope); });
    QObject::connect(button, &QPushButton::clicked, button, [button, scope, confirmText, saveAction]() {
        editOrSave(button, scope, confirmText, saveAction);
    });
    bindSettingsEditSaveButton(settingsScope, button);
    bindSettingsEditSaveButton(settingsScope, cancel, true);
    return controls;
}

} // namespace screw
