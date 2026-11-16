/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FormPageBuilder.h
 * @brief 创建各设置页面共用的表单容器。
 */

#ifndef FORM_PAGE_BUILDER_H
#define FORM_PAGE_BUILDER_H

#include <QString>
#include <QHash>
#include <QVector>

#include "FormSpec.h"
#include "WidgetFactory.h"

class FitStack;
class QWidget;

namespace screw {

/**
 * @brief 在已有页面中创建表单容器及可选的编辑/保存按钮。
 *
 * 表单容器内部各参数分组框（QGroupBox）按定义顺序纵向排列。
 * 表单容器靠上排列，按钮与其底部对齐；不读取页面数据。
 * @param[in] page 待构建表单的页面，指针不能为空，且页面未设置布局。
 * @param[in] formName 表单容器的控件名称，指针不能为空。
 * @param[in] groups 按显示顺序排列的参数分组框定义。
 * @param[in] withSaveButton 是否创建编辑/保存按钮。
 * @param[in] editReadOnly 是否将数值框设为只读反馈。
 * @return 按参数名称标识索引的参数行控件，由页面管理。
 */
QHash<QString, ParamRowWidgets> buildFormContent(QWidget *page, const char *formName,
                                                 const QVector<FormGroup> &groups, bool withSaveButton,
                                                 bool editReadOnly = false);

/**
 * @brief 创建不带编辑/保存按钮的表单页面并加入页面容器。
 * @param[in] stack 接收页面的容器，不能为空。
 * @param[in] pageName 页面名称，不能为空。
 * @param[in] formName 表单容器的控件名称，指针不能为空。
 * @param[in] groups 按显示顺序排列的参数分组框定义。
 * @return 新建的页面，由 stack 管理。
 */
QWidget *createFormPage(FitStack *stack, const char *pageName, const char *formName,
                        const QVector<FormGroup> &groups);

/**
 * @brief 创建仅包含提示标签的页面。
 *
 * 提示文字由调用方在页面创建后设置。
 * @param[in] stack 接收页面的容器，不能为空。
 * @param[in] pageName 页面名称，不能为空。
 * @param[in] hintName 提示标签名称，不能为空。
 * @return 新建的页面，由 stack 管理。
 */
QWidget *createPlaceholderPage(FitStack *stack, const char *pageName, const char *hintName);

} // namespace screw

#endif // FORM_PAGE_BUILDER_H
