/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueChartWidget.h
 * @brief 角度与扭矩曲线的绘制。
 *
 * 无测量数据时只显示坐标系。
 */

#ifndef TORQUE_CHART_WIDGET_H
#define TORQUE_CHART_WIDGET_H

#include <QPointF>
#include <QRect>
#include <QString>
#include <QVector>
#include <QWidget>

class QLabel;

class TorqueChartWidget : public QWidget
{
public:
    /**
     * @brief 创建角度与扭矩图表，并为状态提示保留独立标签。
     * @param[in] xTitle 横轴名称。
     * @param[in] yTitle 纵轴名称。
     * @param[in] parent 父控件指针。
     */
    TorqueChartWidget(const QString &xTitle, const QString &yTitle, QWidget *parent = nullptr);

    /**
     * @brief 更新曲线数据并请求重绘。
     * @param[in] points 按绘制顺序排列的数据点，横坐标为角度，纵坐标为扭矩。
     * @note 任一点非有限数时拒绝整批数据；空列表清除曲线但保留最近的坐标范围。
     */
    void setPoints(const QVector<QPointF> &points);

    /**
     * @brief 更新坐标系右侧的提示。
     * @param[in] text 提示文字，空字符串表示取消提示。
     * @param[in] disconnected 是否使用断线提示颜色。
     */
    void setStatusText(const QString &text, bool disconnected = false);

protected:
    /**
     * @brief 重绘背景、坐标轴、网格和曲线；状态提示由子标签独立绘制。
     * @param[in] event 绘制事件指针。
     */
    void paintEvent(QPaintEvent *event) override;

    /** @brief 图表尺寸变化时调整状态标签的位置。 */
    void resizeEvent(QResizeEvent *event) override;

    /** @brief 字体变化时重新计算状态标签的位置。 */
    void changeEvent(QEvent *event) override;

private:
    /** @brief 单个坐标轴的显示范围。 */
    struct AxisRange {
        double minimum;
        double maximum;
    };

    /** @brief 坐标区域、刻度和状态标签共用的布局结果。 */
    struct ChartLayout {
        QRect plotArea;
        QRect statusArea;
        AxisRange xRange{0.0, 0.0};
        AxisRange yRange{0.0, 0.0};
        double xStep = 0.0;
        double yStep = 0.0;
        QVector<double> xTicks;
        QVector<double> yTicks;
        int yLabelWidth = 0;
    };

    /** @brief 在数据最小值和最大值两侧留出空余，数值相同时也保留有效范围。 */
    static AxisRange paddedRange(double minimum, double maximum);

    /** @brief 按刻度间距向外扩大坐标范围，使坐标轴边界与刻度对齐。 */
    static AxisRange alignedRange(AxisRange range, double step);

    /** @brief 根据容器尺寸、字体和坐标范围计算绘图与标签位置。 */
    ChartLayout chartLayout() const;

    /** @brief 将状态标签放入坐标系右侧的预留区域。 */
    void updateStatusGeometry();

    QString m_xTitle;          // 横轴名称。
    QString m_yTitle;          // 纵轴名称。
    QVector<QPointF> m_points; // 按绘制顺序保存的角度和扭矩数据点。
    AxisRange m_xRange{0.0, 360.0}; // 横轴范围，取得数据后按角度范围更新。
    AxisRange m_yRange{0.0, 40.0};  // 纵轴范围，取得数据后按扭矩范围更新。
    QLabel *m_statusLabel = nullptr; // 坐标系右侧的状态提示标签。
};

#endif // TORQUE_CHART_WIDGET_H
