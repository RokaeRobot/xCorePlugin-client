/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file LabelColumnAligner.cpp
 * @brief 表单标签列的宽度对齐。
 *
 * 按页面字体和标签文字统一列宽，供不同表单共用。
 */

#include "LabelColumnAligner.h"

#include <QEvent>
#include <QLabel>
#include <QVariant>
#include <QWidget>

#include "StyleTokens.h"

LabelColumnAligner::LabelColumnAligner(QWidget *form) : QObject(form), m_form(form) {}

bool LabelColumnAligner::eventFilter(QObject *watched, QEvent *event)
{
    // 表单尺寸或字体变化后重新测量，保持标签列对齐。
    if (watched == m_form && (event->type() == QEvent::Polish || event->type() == QEvent::Show ||
                             event->type() == QEvent::Resize || event->type() == QEvent::FontChange)) {
        alignColumns();
    }
    return QObject::eventFilter(watched, event);
}

void LabelColumnAligner::alignColumns()
{
    // 只统计参数标签的文字宽度，页面标题和其他说明文字不参与列宽计算。
    const QList<QLabel *> labels = m_form->findChildren<QLabel *>();
    int width = 0;
    for (QLabel *label : labels) {
        if (!label->objectName().startsWith(QLatin1String("lbl_"))) {
            continue;
        }
        width = qMax(width, label->fontMetrics().horizontalAdvance(label->text()));
    }

    if (width <= 0) {
        return;
    }

    // 可用宽度取表单宽度的三分之一与四倍字体高度中的较大值，再应用各标签的样式上限。
    width += style::kLabelExtraWidth;
    const int availableWidth = qMax(m_form->fontMetrics().height() * 4, m_form->contentsRect().width() / 3);
    width = qMin(width, availableWidth);
    for (QLabel *label : labels) {
        if (!label->objectName().startsWith(QLatin1String("lbl_"))) {
            continue;
        }
        const int limit = label->property(kLabelWidthLimitProperty).toInt();
        if (limit > 0)
            width = qMin(width, limit);
    }

    for (QLabel *label : labels) {
        if (label->objectName().startsWith(QLatin1String("lbl_")) &&
            (label->minimumWidth() != width || label->maximumWidth() != width))
            label->setFixedWidth(width);
    }
}
