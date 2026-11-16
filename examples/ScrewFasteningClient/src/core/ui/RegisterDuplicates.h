/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RegisterDuplicates.h
 * @brief 保存前的重复寄存器检查和确认提示。
 */

#ifndef REGISTER_DUPLICATES_H
#define REGISTER_DUPLICATES_H

#include <QStringList>

class QWidget;

namespace screw {

/**
 * @brief 查找保存范围内重复选择的寄存器。
 *
 * 仅检查标记为寄存器来源的下拉框；名称去除首尾空白后区分大小写。
 * @param[in] scope 待检查的页面容器，允许为空。
 * @return 排序且去重后的重复名称列表；无重复或容器为空时返回空列表。
 */
QStringList duplicateSettingsRegisters(QWidget *scope);

/**
 * @brief 显示重复寄存器的保存确认。
 * @param[in] parent 弹窗所属的窗口。
 * @param[in] duplicates 需要提醒的重复寄存器名称。
 * @param[in] context 追加显示的保存说明，允许为空。
 * @return 点击确定返回 true；取消或关闭弹窗返回 false。
 */
bool confirmDuplicateSettingsRegisters(QWidget *parent, const QStringList &duplicates,
                                       const QString &context = QString());

} // namespace screw

#endif // REGISTER_DUPLICATES_H
