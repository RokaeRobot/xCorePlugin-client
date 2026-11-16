/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueChartWidget.cpp
 * @brief 角度与扭矩曲线的显示。
 *
 * 负责坐标系和测量曲线的绘制，数据读取由页面处理。
 */

#include "TorqueChartWidget.h"

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QRect>
#include <QResizeEvent>

#include <cmath>
#include <limits>

namespace {
// 图表尺寸和间距，像素值与比例值分别注明。
constexpr int kMinHeight = 240; // 图表最小高度，单位为像素。

constexpr double kPlotWidthRatio = 0.80; // 坐标区域占图表宽度的目标比例。

constexpr double kPlotHeightRatio = 0.80; // 坐标区域占图表高度的目标比例。

constexpr int kChartPadding = 8; // 图表内容与容器边缘的最小距离，单位为像素。

constexpr int kTextGap = 8; // 坐标轴、刻度、轴名和状态提示之间的距离，单位为像素。

constexpr int kHorizontalTickSpacing = 90; // 横轴相邻刻度的目标距离，单位为像素。

constexpr int kVerticalTickSpacing = 60; // 纵轴相邻刻度的目标距离，单位为像素。

constexpr int kTickFontSize = 8; // 刻度文字字号，单位为磅。

constexpr int kStatusWidth = 72; // 坐标系右侧为状态提示预留的最小宽度，单位为像素。

const QColor kBackgroundColor(0xFF, 0xFF, 0xFF); // 图表背景颜色。

const QColor kBorderColor(0xE2, 0xE8, 0xF0); // 图表边框颜色。

const QColor kGridColor(0xDC, 0xE3, 0xEC); // 网格颜色。

const QColor kAxisColor(0x60, 0x6A, 0x78); // 坐标轴和刻度文字颜色。

const QColor kTitleColor(0x33, 0x3A, 0x45); // 坐标轴名称颜色。

const QColor kCurveColor(0x1E, 0x90, 0xFF); // 曲线颜色。

// 刻度间隔采用 1、2、2.5、5、10 的倍数，避免刻度出现过多小数。
double tickStep(double minimum, double maximum, int intervals, double minimumStep = 0.0)
{
    const double approximate = maximum / intervals - minimum / intervals;
    const double unit = qMax(minimumStep, std::pow(10.0, std::floor(std::log10(approximate))));
    if (unit == 0.0)
        return approximate;
    const double multiples[] = {1.0, 2.0, 2.5, 5.0, 10.0};
    double closest = unit;
    for (double multiple : multiples) {
        // 扭矩刻度间距必须是 0.01 的整数倍，排除 0.025 的间距。
        if (minimumStep > 0.0 && unit < minimumStep * 10.0 && multiple == 2.5)
            continue;
        const double step = multiple * unit;
        if (std::isfinite(step) && std::abs(step - approximate) < std::abs(closest - approximate))
            closest = step;
    }
    return closest;
}

// 包含坐标范围两端的刻度，允许计算小数时产生的微小偏差。
QVector<double> tickValues(double minimum, double maximum, double step)
{
    QVector<double> values;
    if (!(step > 0.0) || !std::isfinite(step))
        return values;
    const double tolerance = step * 1e-8;
    const double first = std::ceil(minimum / step - 1e-8);
    for (int index = 0; index < 64; ++index) {
        double value = (first + index) * step;
        if (!std::isfinite(value) || (value > maximum && value - maximum > tolerance))
            break;
        if (value >= minimum || minimum - value <= tolerance) {
            if (std::abs(value) < step * 1e-8)
                value = 0.0;
            values.append(value);
        }
    }
    return values;
}

// 曲线、刻度和网格共用归一化；跨度溢出时先缩半，支持跨越正负极值的范围。
double axisFraction(double value, double minimum, double maximum)
{
    const double span = maximum - minimum;
    if (std::isfinite(span))
        return (value - minimum) / span;
    return (value * 0.5 - minimum * 0.5) / (maximum * 0.5 - minimum * 0.5);
}

QString tickText(double value, int decimals = -1)
{
    if (decimals >= 0)
        return QString::number(value, 'f', decimals);
    return QString::number(value, 'g', 12);
}
} // namespace

TorqueChartWidget::TorqueChartWidget(const QString &xTitle, const QString &yTitle, QWidget *parent)
    : QWidget(parent), m_xTitle(xTitle), m_yTitle(yTitle)
{
    setMinimumHeight(kMinHeight);

    // 绘制函数覆盖整个控件区域，避免更新曲线时残留旧图。
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(false);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("curve_status"));
    m_statusLabel->setTextFormat(Qt::PlainText);
    m_statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_statusLabel->setWordWrap(true);
    // 状态标签由 Qt 绘制白色背景，空文字时也覆盖原页面内容。
    m_statusLabel->setAttribute(Qt::WA_StyledBackground, true);
    setStatusText({});
    updateStatusGeometry();
}

void TorqueChartWidget::setPoints(const QVector<QPointF> &points)
{
    if (m_points == points)
        return;
    if (!points.isEmpty()) {
        double minX = points.first().x(), maxX = minX;
        double minY = points.first().y(), maxY = minY;
        for (const QPointF &point : points) {
            if (!std::isfinite(point.x()) || !std::isfinite(point.y()))
                return;
            minX = qMin(minX, point.x());
            maxX = qMax(maxX, point.x());
            minY = qMin(minY, point.y());
            maxY = qMax(maxY, point.y());
        }
        m_xRange = paddedRange(minX, maxX);
        m_yRange = paddedRange(minY, maxY);
    }
    // 曲线被清空时保留最近一次坐标范围，避免坐标刻度跳回默认范围。
    m_points = points;
    updateStatusGeometry();
    update();
}

TorqueChartWidget::AxisRange TorqueChartWidget::paddedRange(double minimum, double maximum)
{
    // 正常数据两端各留跨度的 5%；数值相同时按数值大小留白，全零时使用正负 0.5。
    double padding = (maximum * 0.5 - minimum * 0.5) * 0.1;
    if (minimum == maximum)
        padding = minimum == 0.0 ? 0.5 : std::abs(minimum) * 0.05;
    const double limit = std::numeric_limits<double>::max();
    double lower = minimum - padding;
    double upper = maximum + padding;
    if (!std::isfinite(lower))
        lower = -limit;
    if (!std::isfinite(upper))
        upper = limit;
    // 极小的数值可能使留白被舍去，使用相邻数值保证范围不为零。
    if (lower == upper) {
        lower = std::nextafter(minimum, -limit);
        upper = std::nextafter(maximum, limit);
    }
    return {lower, upper};
}

TorqueChartWidget::AxisRange TorqueChartWidget::alignedRange(AxisRange range, double step)
{
    if (!(step > 0.0) || !std::isfinite(step))
        return range;
    const double lower = std::floor(range.minimum / step) * step;
    const double upper = std::ceil(range.maximum / step) * step;
    if (std::isfinite(lower))
        range.minimum = qMin(range.minimum, lower);
    if (std::isfinite(upper))
        range.maximum = qMax(range.maximum, upper);
    return range;
}

void TorqueChartWidget::setStatusText(const QString &text, bool disconnected)
{
    const QString style = QStringLiteral("QLabel#curve_status { color: %1; background-color: white; font-size: 12px; }")
                              .arg(disconnected ? QStringLiteral("#B91C1C") : QStringLiteral("#A16207"));
    if (m_statusLabel->styleSheet() != style)
        m_statusLabel->setStyleSheet(style);
    m_statusLabel->setText(text);
}

void TorqueChartWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateStatusGeometry();
}

void TorqueChartWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange)
        updateStatusGeometry();
}

void TorqueChartWidget::updateStatusGeometry()
{
    if (m_statusLabel != nullptr)
        m_statusLabel->setGeometry(chartLayout().statusArea);
}

TorqueChartWidget::ChartLayout TorqueChartWidget::chartLayout() const
{
    ChartLayout layout;
    const QRect full = rect();
    QFont tickFont = font();
    tickFont.setPointSize(kTickFontSize);
    // Qt 5.12 的 QFontMetrics 要求非 const QPaintDevice*，此转换仅用于字体测量。
    const QFontMetrics tickMetrics(tickFont, const_cast<TorqueChartWidget *>(this));

    // 坐标区域按容器比例居中，刻度和轴名需要的空间优先保留。
    const int tickHeight = tickMetrics.height();
    const int topSpace = kChartPadding + tickHeight / 2;
    const int bottomSpace = kChartPadding + 2 * tickHeight + 2 * kTextGap;
    const int plotHeight = qMin(qRound(full.height() * kPlotHeightRatio),
                               full.height() - topSpace - bottomSpace - 1);
    if (plotHeight <= 0)
        return layout;
    const int top = qBound(topSpace, (full.height() - plotHeight) / 2,
                           full.height() - bottomSpace - plotHeight - 1);
    const int bottom = top + plotHeight;

    const double yStep = tickStep(m_yRange.minimum, m_yRange.maximum,
                                 qBound(2, plotHeight / kVerticalTickSpacing, 6), 0.01);
    const AxisRange yRange = alignedRange(m_yRange, yStep);
    const QVector<double> yTicks = tickValues(yRange.minimum, yRange.maximum, yStep);
    int yLabelWidth = 0;
    for (double value : yTicks)
        yLabelWidth = qMax(yLabelWidth, tickMetrics.horizontalAdvance(tickText(value, 2)));

    const int xLabelWidth = qMax(56, qMax(tickMetrics.horizontalAdvance(tickText(m_xRange.minimum)),
                                        tickMetrics.horizontalAdvance(tickText(m_xRange.maximum))) + kTextGap);
    const int leftSpace = qMax(yLabelWidth + tickHeight + 2 * kTextGap + kChartPadding,
                              xLabelWidth / 2 + kChartPadding);
    const int rightSpace = xLabelWidth / 2 + kTextGap + kStatusWidth + kChartPadding;
    const int plotWidth = qMin(qRound(full.width() * kPlotWidthRatio),
                              full.width() - leftSpace - rightSpace - 1);
    if (plotWidth <= 0)
        return layout;
    const int left = qBound(leftSpace, (full.width() - plotWidth) / 2,
                            full.width() - rightSpace - plotWidth - 1);
    const int right = left + plotWidth;

    const double xStep = tickStep(m_xRange.minimum, m_xRange.maximum,
                                 qBound(2, plotWidth / qMax(kHorizontalTickSpacing, xLabelWidth + 2 * kTextGap), 5));
    const AxisRange xRange = alignedRange(m_xRange, xStep);
    const QVector<double> xTicks = tickValues(xRange.minimum, xRange.maximum, xStep);

    int rightLabelWidth = 0;
    for (double value : xTicks)
        rightLabelWidth = qMax(rightLabelWidth, qMax(56, tickMetrics.horizontalAdvance(tickText(value)) + kTextGap) / 2);
    const int statusLeft = right + rightLabelWidth + kTextGap;

    layout.plotArea = QRect(QPoint(left, top), QPoint(right, bottom));
    layout.statusArea = QRect(statusLeft, top, qMax(0, full.width() - statusLeft - kChartPadding), bottom - top);
    layout.xRange = xRange;
    layout.yRange = yRange;
    layout.xStep = xStep;
    layout.yStep = yStep;
    layout.xTicks = xTicks;
    layout.yTicks = yTicks;
    layout.yLabelWidth = yLabelWidth;
    return layout;
}

void TorqueChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 先绘制背景和边框，再绘制坐标及曲线，更新时不保留上一幅曲线的残留内容。
    const QRect full = rect();
    painter.fillRect(full, kBackgroundColor);
    painter.setPen(QPen(kBorderColor, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(full.adjusted(0, 0, -1, -1));

    const ChartLayout layout = chartLayout();
    if (layout.plotArea.isEmpty())
        return;
    const int left = layout.plotArea.left();
    const int right = layout.plotArea.right();
    const int top = layout.plotArea.top();
    const int bottom = layout.plotArea.bottom();
    const AxisRange &xRange = layout.xRange;
    const AxisRange &yRange = layout.yRange;
    QFont tickFont = font();
    tickFont.setPointSize(kTickFontSize);
    // Qt 5.12 的 QFontMetrics 要求非 const QPaintDevice*，此转换仅用于字体测量。
    const QFontMetrics tickMetrics(tickFont, const_cast<TorqueChartWidget *>(this));
    const int tickHeight = tickMetrics.height();

    // 网格与刻度使用同一数据范围，细网格的间隔为刻度间隔的一半。
    painter.setPen(QPen(kGridColor, 1));
    painter.drawRect(QRect(QPoint(left, top), QPoint(right, bottom)));
    for (double value : tickValues(xRange.minimum, xRange.maximum, layout.xStep * 0.5)) {
        const int x = left + qRound((right - left) * axisFraction(value, xRange.minimum, xRange.maximum));
        painter.drawLine(x, top, x, bottom);
    }
    for (double value : tickValues(yRange.minimum, yRange.maximum, layout.yStep * 0.5)) {
        const int y = bottom - qRound((bottom - top) * axisFraction(value, yRange.minimum, yRange.maximum));
        painter.drawLine(left, y, right, y);
    }

    // 坐标轴使用比网格更明显的线条，区分测量方向和背景参考线。
    painter.setPen(QPen(kAxisColor, 2));
    painter.drawLine(left, top, left, bottom);
    painter.drawLine(left, bottom, right, bottom);

    // 角度使用 12 位有效数字，扭矩固定两位小数；两者与各自刻度间距保持一致。
    painter.setFont(tickFont);
    painter.setPen(kAxisColor);
    for (double value : layout.xTicks) {
        const int x = left + qRound((right - left) * axisFraction(value, xRange.minimum, xRange.maximum));
        const QString text = tickText(value);
        const int textWidth = qMax(56, tickMetrics.horizontalAdvance(text) + kTextGap);
        painter.drawText(QRect(x - textWidth / 2, bottom + kTextGap, textWidth, tickHeight),
                         Qt::AlignHCenter | Qt::AlignTop, text);
    }
    for (double value : layout.yTicks) {
        const int y = bottom - qRound((bottom - top) * axisFraction(value, yRange.minimum, yRange.maximum));
        painter.drawText(QRect(left - layout.yLabelWidth - kTextGap, y - tickHeight / 2, layout.yLabelWidth, tickHeight),
                         Qt::AlignRight | Qt::AlignVCenter, tickText(value, 2));
    }

    // 绘制横轴和纵轴名称，名称与刻度分别占用预留区域。
    painter.setPen(kTitleColor);
    painter.drawText(QRect(left, bottom + tickHeight + 2 * kTextGap, right - left, tickHeight),
                     Qt::AlignHCenter, m_xTitle);

    painter.save();

    painter.translate(left - layout.yLabelWidth - 2 * kTextGap - tickHeight / 2, (top + bottom) / 2);
    painter.rotate(-90);
    painter.drawText(QRect(-70, -tickHeight / 2, 140, tickHeight), Qt::AlignHCenter, m_yTitle);
    painter.restore();

    // 没有测量数据时只保留坐标系，不绘制曲线。
    if (m_points.isEmpty()) {
        return;
    }

    // 曲线与坐标刻度共用范围，保持数据点的原有顺序。
    QPainterPath path;
    for (int index = 0; index < m_points.size(); ++index) {
        const double px = left + (right - left) * axisFraction(m_points.at(index).x(), xRange.minimum, xRange.maximum);
        const double py = bottom - (bottom - top) * axisFraction(m_points.at(index).y(), yRange.minimum, yRange.maximum);
        if (index == 0) {
            path.moveTo(px, py);
        } else {
            path.lineTo(px, py);
        }
    }
    painter.setPen(QPen(kCurveColor, 2));
    painter.drawPath(path);
}
