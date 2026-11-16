/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WidgetFactory.h
 * @brief 控件创建和寄存器选项更新接口，供各设置页面调用。
 */

#ifndef WIDGET_FACTORY_H
#define WIDGET_FACTORY_H

#include <QString>
#include <QStringList>

class QComboBox;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QWidget;

namespace screw {

// 按钮标记供 render() 统一设置编辑、保存按钮的尺寸。
constexpr const char kSaveButtonProperty[] = "saveButton"; // 保存按钮的识别属性名。
constexpr const char kEditButtonProperty[] = "editButton"; // 编辑按钮的识别属性名。
// 寄存器来源标记供选项刷新、保存校验和数据读取使用。
constexpr const char kRegisterComboProperty[] = "registerCombo"; // 寄存器下拉框的识别属性名。

/** @brief 参数行容器及其输入控件，供页面直接访问。 */
struct ParamRowWidgets
{
    QWidget *widget;
    QComboBox *registerCombo;
    QLineEdit *valueEdit;
};

/**
 * @brief 创建横向按钮或参数行布局。
 * @param[in] parent 布局所属的容器。
 * @return 新建的布局，由所属容器管理。
 */
QHBoxLayout *createNavRow(QWidget *parent);

/**
 * @brief 计算表单标签列的统一宽度。
 * @param[in] reference 提供测量字体的控件，不能为空。
 * @param[in] texts 同一表单内需要对齐的标签文字。
 * @return 最长文字的显示宽度加预留宽度，单位为像素。
 */
int labelColumnWidth(const QWidget *reference, const QStringList &texts);

/**
 * @brief 创建包含标签、寄存器下拉框和数值框的参数行。
 *
 * key 用于生成 row_、lbl_、combo_、edit_ 前缀的控件名称。
 * 控件名称用于配置保存和恢复，同一页面内应保持唯一。
 * @param[in] parent 参数行所属的容器。
 * @param[in] key 参数行的名称标识，不能为空。
 * @param[in] labelText 参数名称，由调用方提供翻译后的文字。
 * @param[in] labelWidth 标签列的最小宽度，单位为像素。
 * @param[in] readOnly 是否将数值框设为只读反馈。
 * @return 参数行容器及输入控件，均由父控件管理。
 */
ParamRowWidgets createParamRow(QWidget *parent, const char *key, const QString &labelText, int labelWidth,
                               bool readOnly = false);

/** @brief 创建带有编辑保存样式标记的按钮。 */
QPushButton *createSaveButton(QWidget *parent);

/**
 * @brief 刷新页面内的寄存器选项并保留当前输入。
 * @param[in] root 包含寄存器下拉框的根容器，允许为空。
 */
void refreshRegisterCombos(QWidget *root);

/**
 * @brief 设置寄存器下拉框在展开前刷新选项。
 * @param[in] combo 待设置的下拉框，允许为空；同一下拉框只需设置一次。
 */
void watchRegisterCombo(QComboBox *combo);

/** @brief 区分列表选择和文字输入，仅在主动输入时启用内部编辑框。 */
void bindComboTextInput(QComboBox *combo);

} // namespace screw

#endif // WIDGET_FACTORY_H
