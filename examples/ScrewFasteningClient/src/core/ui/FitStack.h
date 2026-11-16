/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FitStack.h
 * @brief 按当前页面计算尺寸的页面容器。
 *
 * 页面切换时更新建议尺寸，避免隐藏页面造成多余的滚动区域。
 */

#ifndef FIT_STACK_H
#define FIT_STACK_H

#include <QWidget>

class QStackedLayout;

class FitStack : public QWidget
{
    Q_OBJECT
public:
    explicit FitStack(QWidget *parent = nullptr);

    /** @brief 添加页面，页面的显示和隐藏由容器管理。 */
    int addWidget(QWidget *page);

    /** @brief 获取页面数量。 */
    int count() const;

    /** @brief 按下标获取页面。 */
    QWidget *widget(int index) const;

    /** @brief 获取当前页面。 */
    QWidget *currentWidget() const;

    /** @brief 获取当前页面下标。 */
    int currentIndex() const;

    /** @brief 切换当前页面。 */
    void setCurrentIndex(int index);

    /**
     * @brief 获取页面正常显示时的建议尺寸。
     * @return 当前页面及容器边距所需的宽高。
     */
    QSize sizeHint() const override;

    /**
     * @brief 获取页面缩小时的建议最小尺寸。
     * @return 当前页面及容器边距所需的最小宽高。
     */
    QSize minimumSizeHint() const override;

signals:
    /** @brief 当前页面切换完成后通知导航和操作按钮。 */
    void currentChanged(int index);

private:
    QStackedLayout *m_layout; // 只按当前页面计算尺寸的页面布局。
};

#endif // FIT_STACK_H
