/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WrappingButton.h
 * @brief 支持文字自动换行的按钮。
 */

#ifndef WRAPPING_BUTTON_H
#define WRAPPING_BUTTON_H

#include <QPushButton>

class WrappingButton : public QPushButton
{
public:
    explicit WrappingButton(const QString &text, QWidget *parent = nullptr);

    /** @brief 按按钮宽度计算完整显示文字所需的高度。 */
    int heightForWidth(int width) const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    /** @brief 按可用文字宽度生成换行后的按钮文字。 */
    QString wrappedText(int width) const;
};

#endif // WRAPPING_BUTTON_H
