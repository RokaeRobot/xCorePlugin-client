/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WidgetFactory.cpp
 * @brief 公共控件的创建和寄存器选项维护。
 *
 * 各页面共用参数行、编辑保存按钮和寄存器下拉框，减少重复代码。
 */

#include "WidgetFactory.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QEvent>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QPointer>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSizePolicy>
#include <QStringList>
#include <QVariant>
#include <QWidget>

#include "StyleTokens.h"
#include "SettingsEditor.h"
#include "interface/interfacemanager.h"

namespace screw {

namespace {
// 列表选择不启用文字输入，只有主动点击文字区域或使用键盘输入时才允许编辑。
class ComboTextInput : public QObject
{
public:
    explicit ComboTextInput(QComboBox *combo)
        : QObject(combo), m_combo(combo), m_edit(combo->lineEdit()), m_popup(combo->view()->window())
    {
        setObjectName(QStringLiteral("combo_text_input"));
        combo->installEventFilter(this);
        m_edit->installEventFilter(this);
        m_popup->installEventFilter(this);
        setTextInputEnabled(false);
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (!m_combo || !m_edit)
            return false;

        // 打开或关闭列表前先关闭文字输入，焦点返回时仍保持只读。
        if (watched == m_popup && (event->type() == QEvent::Show || event->type() == QEvent::Hide)) {
            const QPointer<QWidget> popupGuard(m_popup);
            setTextInputEnabled(false);
            return !popupGuard;
        } else if (watched == m_combo && event->type() == QEvent::MouseButtonPress) {
            const QPointer<QComboBox> comboGuard(m_combo);
            setTextInputEnabled(false);
            if (!comboGuard)
                return true;
            m_edit->clearFocus();
            if (!comboGuard)
                return true;
            m_combo->clearFocus();
            return !comboGuard;
        } else if (watched == m_combo && event->type() == QEvent::FocusIn && !m_textInputEnabled) {
            // 不将列表返回的焦点转交给内部输入框。
            m_combo->update();
            return true;
        } else if (watched == m_combo &&
                   (event->type() == QEvent::Hide ||
                    (event->type() == QEvent::EnabledChange && !m_combo->isEnabled()))) {
            const QPointer<QComboBox> comboGuard(m_combo);
            setTextInputEnabled(false);
            return !comboGuard;
        } else if (watched == m_edit) {
            if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
                const QPointer<QComboBox> comboGuard(m_combo);
                setTextInputEnabled(true);
                if (!comboGuard)
                    return true;
                m_edit->setFocus(Qt::MouseFocusReason);
                return !comboGuard;
            }
        } else if (watched == m_combo && event->type() == QEvent::KeyPress) {
            // 保留实体键盘输入；方向键等列表操作不进入文字输入。
            const auto *key = static_cast<QKeyEvent *>(event);
            if (key->key() == Qt::Key_F2 || (!key->text().isEmpty() && key->text().at(0).isPrint())) {
                const QPointer<QComboBox> comboGuard(m_combo);
                setTextInputEnabled(true);
                if (!comboGuard)
                    return true;
                m_edit->setFocus(Qt::OtherFocusReason);
                return !comboGuard;
            }
        }
        return false;
    }

private:
    void setTextInputEnabled(bool enabled)
    {
        m_textInputEnabled = enabled;
        m_edit->setReadOnly(!enabled);
        m_edit->setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
        m_combo->setAttribute(Qt::WA_InputMethodEnabled, enabled);
        m_combo->setFocusProxy(enabled ? m_edit.data() : nullptr);
    }

    QPointer<QComboBox> m_combo;      // 需要区分列表选择和文字输入的下拉框。
    QPointer<QLineEdit> m_edit;       // 下拉框内部的文字输入框。
    QPointer<QWidget> m_popup;        // 下拉列表窗口。
    bool m_textInputEnabled = false; // 是否允许文字输入。
};

// 刷新选项时保留当前输入，避免寄存器列表变化清除未保存的选择。
void fillRegisterCombo(QComboBox *combo)
{
    // 使用通信配置中的寄存器名称作为选项，排序后便于查找。
    QStringList names = xplugin::xPluginInterface().getRegisterHash().keys();
    names.sort(Qt::CaseInsensitive);

    // 寄存器列表未变化时保留现有选项，避免反复更新正在编辑的下拉框。
    bool unchanged = combo->count() == names.size();
    for (int index = 0; unchanged && index < names.size(); ++index) {
        unchanged = combo->itemText(index) == names.at(index);
    }
    if (unchanged) {
        return;
    }

    // 更新选项前保存当前输入；寄存器暂时不存在时仍保留输入文字，便于修改配置。
    const QString current = combo->currentText();
    combo->blockSignals(true);
    combo->clear();
    combo->addItems(names);
    const int index = combo->findText(current);
    if (index >= 0) {
        combo->setCurrentIndex(index);
    } else if (combo->isEditable()) {
        combo->setEditText(current);
    }
    combo->blockSignals(false);
}

// 寄存器下拉框的选项刷新与输入处理。
class RegisterComboLoader : public QObject
{
public:
    /**
     * @brief 绑定下拉框输入处理，并在获得焦点或按下鼠标时刷新寄存器选项。
     * @param[in] combo 需要刷新寄存器选项的下拉框指针，同时作为父对象。
     */
    explicit RegisterComboLoader(QComboBox *combo) : QObject(combo), m_combo(combo)
    {
        // Qt 先调用后安装的过滤器；先绑定输入处理，再安装选项刷新，保证刷新先接收焦点事件。
        bindComboTextInput(combo);
        combo->installEventFilter(this);
        fillRegisterCombo(combo);
    }

protected:
    /**
     * @brief 在下拉框获得焦点或按下鼠标时刷新选项，保留当前输入。
     * @param[in] watched 被监视的对象指针。
     * @param[in] event 事件指针。
     * @return 是否停止事件继续传递。
     */
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_combo &&
            (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::FocusIn)) {
            fillRegisterCombo(m_combo);
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QComboBox *m_combo = nullptr; // 需要刷新寄存器选项的下拉框指针。
};

} // namespace

QHBoxLayout *createNavRow(QWidget *parent)
{
    auto *layout = new QHBoxLayout(parent);
    return layout;
}

int labelColumnWidth(const QWidget *reference, const QStringList &texts)
{
    const QFontMetrics metrics(reference->font());
    int width = 0;
    for (const QString &text : texts) {
        width = qMax(width, metrics.horizontalAdvance(text));
    }
    return width + style::kLabelExtraWidth;
}

ParamRowWidgets createParamRow(QWidget *parent, const char *key, const QString &labelText, int labelWidth,
                               bool readOnly)
{
    // 同一行的控件共用名称标识，供配置保存、恢复和统一样式使用。
    const QString suffix = QString::fromLatin1(key);

    auto *row = new QWidget(parent);
    row->setObjectName(QStringLiteral("row_") + suffix);

    // 参数行保持内容高度，页面增加的高度留在表单下方。
    row->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto *layout = createNavRow(row);

    auto *label = new QLabel(labelText, row);
    label->setObjectName(QStringLiteral("lbl_") + suffix);

    label->setMinimumWidth(labelWidth);
    layout->addWidget(label);

    // 寄存器选择统一标记，供列表刷新、重复检查和参数收集使用。
    auto *combo = new QComboBox(row);
    combo->setObjectName(QStringLiteral("combo_") + suffix);
    combo->setEditable(true);
    combo->setProperty(kRegisterComboProperty, true);
    new RegisterComboLoader(combo);
    layout->addWidget(combo);

    // 反馈页面的数值框始终只读；配置页面的数值框允许输入待写入值。
    auto *edit = new QLineEdit(row);
    edit->setObjectName(QStringLiteral("edit_") + suffix);

    edit->setEnabled(!readOnly);
    edit->setProperty(kSettingsReadOnlyProperty, readOnly);
    if (!readOnly) {
        edit->setValidator(new QRegExpValidator(QRegExp(QStringLiteral("^-?\\d*\\.?\\d*$")), edit));
    }
    layout->addWidget(edit);

    layout->addStretch();
    return {row, combo, edit};
}

QPushButton *createSaveButton(QWidget *parent)
{
    auto *button = new QPushButton(parent);
    button->setProperty(kSaveButtonProperty, true);
    return button;
}

void refreshRegisterCombos(QWidget *root)
{
    if (root == nullptr) {
        return;
    }
    const QList<QComboBox *> combos = root->findChildren<QComboBox *>();
    for (QComboBox *combo : combos) {
        if (!combo->property(kRegisterComboProperty).toBool()) {
            continue;
        }
        fillRegisterCombo(combo);
    }
}

void watchRegisterCombo(QComboBox *combo)
{
    if (combo == nullptr) {
        return;
    }
    combo->setProperty(kRegisterComboProperty, true);
    new RegisterComboLoader(combo);
}

void bindComboTextInput(QComboBox *combo)
{
    if (combo == nullptr || !combo->isEditable() ||
        combo->findChild<QObject *>(QStringLiteral("combo_text_input")) != nullptr)
        return;
    new ComboTextInput(combo);
}

} // namespace screw
