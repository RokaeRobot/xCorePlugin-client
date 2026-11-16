/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FormSpec.h
 * @brief 表单容器使用的参数行和参数分组框定义。
 */

#ifndef FORM_SPEC_H
#define FORM_SPEC_H

#include <QString>
#include <QVector>

/** @brief 表单参数行的名称和显示文字。 */
struct FormRow
{
    const char *key; // 控件名称标识；同一页面内应保持唯一。
    QString text;    // 由调用方提供翻译后的参数名称。
};

/** @brief 参数分组框（QGroupBox）的标题和参数行定义。 */
struct FormGroup
{
    const char *key;       // 分组名称标识；为空指针时使用默认名称。
    QString title;         // 分组标题；空字符串表示无标题分组。
    QVector<FormRow> rows; // 按显示顺序排列的参数行。
};

#endif // FORM_SPEC_H
