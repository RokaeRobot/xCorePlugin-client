/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file NavBar.h
 * @brief 一级菜单和二级菜单的创建及页面切换。
 */

#ifndef NAV_BAR_H
#define NAV_BAR_H

#include <QList>
#include <QString>
#include <QStringList>

class QPushButton;
class FitStack;
class QWidget;

namespace screw {

// 菜单标记和目标尺寸供 render() 统一设置按钮样式。
constexpr const char kNavButtonProperty[] = "navButton";       // 菜单按钮的识别属性名。
constexpr const char kNavMinHeightProperty[] = "navMinHeight"; // 菜单按钮最小高度的属性名。

/**
 * @brief 按给定顺序创建菜单栏。
 *
 * 按钮互斥选中，默认选中首个按钮；页面切换由 bindNav 单独关联。
 * @param[in] items 按菜单顺序排列的按钮文字，由调用方提供翻译后的文字。
 * @param[in] buttonMinHeight 按钮最小高度，单位为像素。
 * @param[in] barObjectName 菜单栏控件名称，不能为空。
 * @param[in] parent 菜单栏所属的容器。
 * @param[in,out] buttons 按顺序追加生成的按钮；为空指针时不输出按钮列表。
 * @param[in] vertical 为 true 时纵向排列，为 false 时横向排列。
 * @return 菜单栏容器，由 parent 管理。
 */
QWidget *createNavBar(const QStringList &items, int buttonMinHeight, const char *barObjectName,
                      QWidget *parent, QList<QPushButton *> *buttons = Q_NULLPTR, bool vertical = false);

/**
 * @brief 按相同下标关联菜单按钮和页面。
 *
 * 切换前确认未保存的修改，选择继续编辑时保持原页面和原导航选中状态。
 * @param[in] stack 待切换的页面容器，不能为空。
 * @param[in] buttons 按页面顺序排列的按钮列表，数量应与页面数量一致。
 */
void bindNav(FitStack *stack, const QList<QPushButton *> &buttons);

} // namespace screw

#endif // NAV_BAR_H
