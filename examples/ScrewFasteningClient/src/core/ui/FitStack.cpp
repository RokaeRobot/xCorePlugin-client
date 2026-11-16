/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FitStack.cpp
 * @brief 按当前页面计算尺寸的页面容器。
 *
 * 切换页面时更新所需空间，避免隐藏页面造成多余的滚动区域。
 */

#include "FitStack.h"

#include <QStackedLayout>

namespace {

// 页面尺寸和换行高度都只取当前页面，隐藏页面不参与空间计算。
class CurrentPageLayout : public QStackedLayout
{
public:
    explicit CurrentPageLayout(QWidget *parent) : QStackedLayout(parent) {}

    QSize sizeHint() const override
    {
        const QWidget *page = currentWidget();
        return page != nullptr ? page->sizeHint().expandedTo(minimumSize()) : QSize(0, 0);
    }

    QSize minimumSize() const override
    {
        const QWidget *page = currentWidget();
        return page != nullptr ? page->minimumSizeHint().expandedTo(page->minimumSize()) : QSize(0, 0);
    }

    bool hasHeightForWidth() const override
    {
        const QWidget *page = currentWidget();
        return page != nullptr && page->hasHeightForWidth();
    }

    int heightForWidth(int width) const override
    {
        const QWidget *page = currentWidget();
        if (page == nullptr)
            return 0;
        const int height = page->hasHeightForWidth() ? page->heightForWidth(width)
                                                    : page->sizeHint().height();
        return qMax(height, minimumSize().height());
    }
};

} // namespace

FitStack::FitStack(QWidget *parent) : QWidget(parent), m_layout(new CurrentPageLayout(this))
{
    // 切换页面后重新计算布局和最小尺寸，使外层滚动区域及时缩短或增长。
    connect(m_layout, &QStackedLayout::currentChanged, this, [this](int index) {
        m_layout->invalidate();
        m_layout->activate();
        updateGeometry();
        emit currentChanged(index);
    });
}

int FitStack::addWidget(QWidget *page)
{
    return m_layout->addWidget(page);
}

int FitStack::count() const
{
    return m_layout->count();
}

QWidget *FitStack::widget(int index) const
{
    return m_layout->widget(index);
}

QWidget *FitStack::currentWidget() const
{
    return m_layout->currentWidget();
}

int FitStack::currentIndex() const
{
    return m_layout->currentIndex();
}

void FitStack::setCurrentIndex(int index)
{
    m_layout->setCurrentIndex(index);
}

QSize FitStack::sizeHint() const
{
    return m_layout->totalSizeHint();
}

QSize FitStack::minimumSizeHint() const
{
    return m_layout->totalMinimumSize();
}
