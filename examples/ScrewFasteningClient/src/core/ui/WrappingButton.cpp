/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WrappingButton.cpp
 * @brief 按可用宽度换行显示按钮文字。
 */

#include "WrappingButton.h"

#include <QFontMetrics>
#include <QStringList>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QTextLayout>
#include <QTextOption>

WrappingButton::WrappingButton(const QString &text, QWidget *parent) : QPushButton(text, parent) {}

QString WrappingButton::wrappedText(int width) const
{
    // 测量与绘制共用同一换行结果，保证导航行计算的高度足以显示按钮文字。
    QTextLayout layout(text(), font());
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);

    QStringList lines;
    layout.beginLayout();
    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(qMax(1, width));
        lines.append(text().mid(line.textStart(), line.textLength()).trimmed());
    }
    layout.endLayout();
    return lines.join(QLatin1Char('\n'));
}

int WrappingButton::heightForWidth(int width) const
{
    QStyleOptionButton option;
    initStyleOption(&option);
    option.rect.setWidth(width);
    const QRect contents = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
    option.text = wrappedText(contents.width());
    const QSize textSize = fontMetrics().size(Qt::TextShowMnemonic, option.text);
    const QSize buttonSize = style()->sizeFromContents(QStyle::CT_PushButton, &option, textSize, this);
    return qMax(minimumHeight(), buttonSize.height());
}

void WrappingButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOptionButton option;
    initStyleOption(&option);
    const QRect contents = style()->subElementRect(QStyle::SE_PushButtonContents, &option, this);
    option.text = wrappedText(contents.width());

    // 保留按钮原有的背景、文字颜色及选中效果，只调整文字换行。
    QStylePainter painter(this);
    painter.drawControl(QStyle::CE_PushButton, option);
}
