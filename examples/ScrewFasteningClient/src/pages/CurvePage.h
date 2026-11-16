/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file CurvePage.h
 * @brief 扭矩曲线页面。
 */

#ifndef CURVE_PAGE_H
#define CURVE_PAGE_H

#include <QElapsedTimer>
#include <QWidget>

#include "core/controller/RegisterIo.h"

class QComboBox;
class QLineEdit;
class TorqueChartWidget;

namespace xplugin {

class CurvePage : public QWidget
{
    Q_OBJECT
public:
    explicit CurvePage(QWidget *parent = nullptr);

private:
    /** @brief 根据读取结果更新组数、曲线和右侧提示。 */
    void refreshCurve(RegisterIo::ReadResult result);

    /** @brief 数据来源变化或断线时清除原有组数和曲线。 */
    void clearCurve();

    /** @brief 保留上次有效显示，并提示读取失败或数据不完整。 */
    void showReadFailure(const QString &reason);

    QComboBox *m_groupCombo = nullptr; // 组数寄存器选择框。
    QLineEdit *m_groupEdit = nullptr;  // 最近一次读取到的组数寄存器数值。
    QComboBox *m_dataCombo = nullptr;  // 扭矩角度数据寄存器选择框。
    TorqueChartWidget *m_chart = nullptr; // 曲线显示控件。
    QElapsedTimer m_failureTimer;     // 连续未取得有效曲线数据的计时。
    bool m_hasValidData = false;      // 是否已经显示过当前来源的有效数据。
};

} // namespace xplugin

#endif // CURVE_PAGE_H
