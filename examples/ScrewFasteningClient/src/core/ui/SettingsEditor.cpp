/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsEditor.cpp
 * @brief 恢复页面配置，保留页面输入，并管理编辑状态。
 */

#include "SettingsEditor.h"

#include <QComboBox>
#include <QEvent>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QVariant>
#include <QTimer>
#include <QWidget>
#include "core/controller/RunLock.h"
#include "pages/common/PageFieldWidgets.h"
#include "WidgetFactory.h"

namespace screw {

namespace {

// 只记录寄存器选择和配置数值，排除实时反馈与配方号。
QMap<QString, QString> configurationFields(QWidget *scope)
{
    QMap<QString, QString> fields;
    for (QComboBox *combo : scope->findChildren<QComboBox *>()) {
        if (combo->objectName().startsWith(QLatin1String("combo_")) &&
            !combo->property(kSettingsReadOnlyProperty).toBool() &&
            !combo->property(kSettingsSelectorProperty).toBool()) {
            fields.insert(combo->objectName(), combo->currentText());
        }
    }
    for (QLineEdit *edit : scope->findChildren<QLineEdit *>()) {
        if (qobject_cast<QComboBox *>(edit->parentWidget()) == nullptr &&
            edit->objectName().startsWith(QLatin1String("edit_")) &&
            !edit->property(kSettingsReadOnlyProperty).toBool()) {
            fields.insert(edit->objectName(), edit->text());
        }
    }
    return fields;
}

class SettingsEditor : public QObject
{
public:
    /**
     * @brief 绑定配置恢复和运行锁，页面销毁时一并销毁管理对象。
     * @param[in] scope 需要管理编辑状态的页面指针，同时作为父对象。
     * @param[in] manageEditing 是否通过编辑保存按钮管理输入。
     */
    explicit SettingsEditor(QWidget *scope, bool manageEditing)
        : QObject(scope), m_scope(scope), m_manageEditing(manageEditing)
    {
        setObjectName(QStringLiteral("settings_editor"));
        m_restore = [scope]() { restorePageSettings(scope); };
        scope->installEventFilter(this);
        RunLock::instance().subscribe(this, [this](bool) { apply(); });
    }

    /**
     * @brief 设置编辑状态。
     * @param[in] editing 是否允许编辑页面配置。
     */
    void setEditing(bool editing)
    {
        if (editing && RunLock::instance().isLocked())
            return;
        m_editing = editing;
        apply();
    }

    /**
     * @brief 查询编辑状态。
     * @return 页面处于编辑状态返回 true，否则返回 false。
     */
    bool editing() const
    {
        return m_editing;
    }

    /** @brief 指定文件恢复方法和输入管理方式。 */
    void setRestore(const std::function<void()> &restore, bool manageEditing)
    {
        m_restore = restore;
        m_manageEditing = manageEditing;
    }

    /** @brief 本地保存成功后结束编辑；单行写入页面同时更新保存记录。 */
    void markSaved()
    {
        if (!m_manageEditing)
            m_savedFields = configurationFields(m_scope);
        m_loaded = true;
        setEditing(false);
    }

    /** @brief 重新读取页面配置，记录已保存的参数并结束编辑状态。 */
    void reload()
    {
        refreshRegisterCombos(m_scope);
        if (m_restore)
            m_restore();
        markSaved();
        setEditing(false);
    }

    /** @brief 更新已经单独保存的参数行，不改变其他参数的保存记录。 */
    void markSavedControls(QWidget *controls)
    {
        const auto fields = configurationFields(controls);
        for (auto it = fields.constBegin(); it != fields.constEnd(); ++it)
            m_savedFields.insert(it.key(), it.value());
        m_loaded = true;
    }

    /** @brief 受管页面进入编辑即视为待确认；单行写入页面按配置快照判断修改。 */
    bool hasChanges() const
    {
        return m_manageEditing ? m_editing
                               : m_loaded && configurationFields(m_scope) != m_savedFields;
    }

    /**
     * @brief 关联操作按钮。
     * @param[in] button 需要随编辑状态更新文字的按钮指针。
     */
    void bindButton(QPushButton *button, bool cancel)
    {
        (cancel ? m_cancelButtons : m_buttons).append(button);
        apply();
    }

protected:
    /**
     * @brief 页面显示时恢复已保存配置；存在待确认编辑或修改时保留当前输入。
     * @param[in] watched 被监视的对象指针。
     * @param[in] event 事件指针。
     * @return 是否停止事件继续传递。
     */
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_scope && event->type() == QEvent::Show) {
            // 编辑中的页面保留输入和配方号，返回页面时只更新控件是否允许操作。
            if (hasChanges())
                apply();
            else
                reload();
        }
        return QObject::eventFilter(watched, event);
    }

private:
    /**
     * @brief 按编辑状态和运行锁更新按钮及输入权限，实时反馈始终保持只读。
     */
    void apply()
    {
        // 页面切换可能重建按钮，先清除已经销毁的按钮记录。
        for (int i = m_buttons.size() - 1; i >= 0; --i) {
            if (m_buttons.at(i).isNull()) {
                m_buttons.removeAt(i);
                continue;
            }
            m_buttons.at(i)->setText(m_editing ? QObject::tr("保存") : QObject::tr("编辑"));
            m_buttons.at(i)->setEnabled(!RunLock::instance().isLocked());
        }

        for (int i = m_cancelButtons.size() - 1; i >= 0; --i) {
            if (m_cancelButtons.at(i).isNull()) {
                m_cancelButtons.removeAt(i);
                continue;
            }
            m_cancelButtons.at(i)->setVisible(m_editing);
        }

        // 控制指令页面保持原有输入方式，只管理配置恢复和修改检查。
        if (!m_manageEditing)
            return;

        // 编辑状态与运行限制共同决定配置是否可输入；反馈数值始终只读。
        const bool enabled = m_editing && !RunLock::instance().isLocked();
        for (QComboBox *combo : m_scope->findChildren<QComboBox *>()) {
            combo->setProperty(kSettingsManagedProperty, true);
            // 配方号用于切换查看配方，程序停止时无需进入编辑状态也能选择配方号。
            const bool selector = combo->property(kSettingsSelectorProperty).toBool();
            combo->setEnabled((selector ? !RunLock::instance().isLocked() : enabled) &&
                              !combo->property(kSettingsReadOnlyProperty).toBool());
        }

        // 单独管理数值框，寄存器下拉框中的文字输入不作为额外配置项处理。
        for (QLineEdit *edit : m_scope->findChildren<QLineEdit *>()) {
            if (qobject_cast<QComboBox *>(edit->parentWidget()) != nullptr)
                continue;
            edit->setProperty(kSettingsManagedProperty, true);
            edit->setEnabled(enabled && !edit->property(kSettingsReadOnlyProperty).toBool());
        }
    }

    QWidget *m_scope;       // 需要管理编辑状态的页面指针。
    bool m_editing = false; // 页面是否处于编辑状态。
    bool m_manageEditing = true; // 是否通过编辑保存按钮管理输入。
    bool m_loaded = false; // 是否已经恢复过文件配置。
    QMap<QString, QString> m_savedFields; // 最近成功保存的配置，不包含实时值。
    std::function<void()> m_restore; // 从文件恢复当前页面配置的函数。
    QList<QPointer<QPushButton>> m_buttons; // 编辑保存按钮指针，按钮销毁后自动置空。
    QList<QPointer<QPushButton>> m_cancelButtons; // 仅在编辑期间显示的取消按钮。
};

// 获取页面的编辑状态管理对象，尚未创建时先创建。
SettingsEditor *editorFor(QWidget *scope, bool manageEditing = true)
{
    if (scope == nullptr)
        return nullptr;
    QObject *editor =
        scope->findChild<QObject *>(QStringLiteral("settings_editor"), Qt::FindDirectChildrenOnly);
    return editor == nullptr ? new SettingsEditor(scope, manageEditing)
                             : static_cast<SettingsEditor *>(editor);
}

} // namespace

void bindSettingsRestore(QWidget *scope, const std::function<void()> &restore, bool manageEditing)
{
    if (auto *editor = editorFor(scope, manageEditing))
        editor->setRestore(restore, manageEditing);
}

void markSettingsSaved(QWidget *scope)
{
    if (auto *editor = editorFor(scope))
        editor->markSaved();
}

void markSettingsSaved(QWidget *scope, QWidget *savedControls)
{
    if (savedControls == nullptr)
        return;
    if (auto *editor = editorFor(scope))
        editor->markSavedControls(savedControls);
}

bool confirmSettingsLeave(QWidget *root)
{
    if (root == nullptr)
        return true;

    QList<QPointer<QWidget>> pending;
    const auto editors = root->findChildren<QObject *>(QStringLiteral("settings_editor"));
    for (QObject *object : editors) {
        auto *editor = static_cast<SettingsEditor *>(object);
        auto *scope = qobject_cast<QWidget *>(editor->parent());
        if (scope == nullptr || !scope->isVisible())
            continue;
        if (scope->property("saveInProgress").toBool())
            return false;
        if (editor->hasChanges())
            pending.append(scope);
    }
    if (pending.isEmpty())
        return true;

    const QPointer<QWidget> rootGuard(root);
    QMessageBox dialog(QMessageBox::Question, QObject::tr("尚未结束编辑"),
                       QObject::tr("当前页面尚未结束编辑，是否放弃未保存的修改并离开？"),
                       QMessageBox::NoButton, root->window());
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    auto *discard = dialog.addButton(QObject::tr("放弃修改"), QMessageBox::DestructiveRole);
    auto *keepEditing = dialog.addButton(QObject::tr("继续编辑"), QMessageBox::RejectRole);
    dialog.setDefaultButton(keepEditing);
    dialog.setEscapeButton(keepEditing);
    dialog.exec();
    if (!rootGuard || dialog.clickedButton() != discard)
        return false;
    // 放弃修改立即恢复原配置，避免页面下次显示时继续保留已经放弃的输入。
    for (const auto &scope : pending) {
        if (scope)
            editorFor(scope)->reload();
    }
    return true;
}

void cancelSettingsEditing(QWidget *scope)
{
    if (scope == nullptr || !settingsEditing(scope) || scope->property("saveInProgress").toBool())
        return;
    const QPointer<QWidget> guard(scope);
    scope->setProperty("saveInProgress", true);
    QMessageBox dialog(QMessageBox::Question, QObject::tr("取消编辑"),
                       QObject::tr("确定放弃当前未保存的修改，并恢复到最近保存的内容吗？"),
                       QMessageBox::NoButton, scope->window());
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    auto *keep = dialog.addButton(QObject::tr("继续编辑"), QMessageBox::RejectRole);
    auto *discard = dialog.addButton(QObject::tr("放弃修改"), QMessageBox::DestructiveRole);
    dialog.setDefaultButton(keep);
    dialog.setEscapeButton(keep);
    dialog.exec();
    if (!guard)
        return;
    scope->setProperty("saveInProgress", false);
    if (dialog.clickedButton() == discard)
        editorFor(scope)->reload();
}

// 只监视插件主界面的隐藏和显示，内部页面切换不触发返回提醒。
class SettingsReturnReminder : public QObject
{
public:
    explicit SettingsReturnReminder(QWidget *root) : QObject(root), m_root(root)
    {
        root->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::Hide)
            m_hidden = true;
        else if (event->type() == QEvent::Show && m_hidden) {
            m_hidden = false;
            // 主界面及子页面显示完成后再检查，避免遗漏尚未显示的编辑页面。
            QTimer::singleShot(0, this, [this]() {
                if (!m_root->isVisible() || m_showing)
                    return;
                const auto editors = m_root->findChildren<QObject *>(QStringLiteral("settings_editor"));
                for (QObject *object : editors) {
                    auto *editor = static_cast<SettingsEditor *>(object);
                    auto *scope = qobject_cast<QWidget *>(editor->parent());
                    if (scope && scope->isVisible() && editor->hasChanges()) {
                        m_showing = true;
                        const QPointer<SettingsReturnReminder> guard(this);
                        QMessageBox::information(m_root->window(), tr("继续编辑"),
                            RunLock::instance().isLocked()
                                ? tr("当前页面尚未结束编辑，输入已保留。程序运行中，暂不能编辑或保存。")
                                : tr("当前页面尚未结束编辑，请继续编辑或保存。"));
                        if (!guard)
                            return;
                        m_showing = false;
                        break;
                    }
                }
            });
        }
        return QObject::eventFilter(watched, event);
    }
private:
    QWidget *m_root; // 插件主界面，负责保留当前一级页面和二级页面。
    bool m_hidden = false; // 主界面已经离开，下一次显示时需要检查编辑状态。
    bool m_showing = false; // 返回提醒已经显示，避免重复打开提醒窗口。
};

void bindSettingsReturnReminder(QWidget *root)
{
    if (root)
        new SettingsReturnReminder(root);
}

void setSettingsEditing(QWidget *scope, bool editing)
{
    if (auto *editor = editorFor(scope))
        editor->setEditing(editing);
}

void reloadSettings(QWidget *root)
{
    if (root == nullptr)
        return;
    const auto editors = root->findChildren<QObject *>(QStringLiteral("settings_editor"));
    for (QObject *object : editors)
        static_cast<SettingsEditor *>(object)->reload();
}

bool settingsEditing(QWidget *scope)
{
    auto *editor = editorFor(scope);
    return editor != nullptr && editor->editing();
}

void bindSettingsEditSaveButton(QWidget *scope, QPushButton *button, bool cancel)
{
    if (button == nullptr)
        return;
    if (auto *editor = editorFor(scope))
        editor->bindButton(button, cancel);
}

} // namespace screw
