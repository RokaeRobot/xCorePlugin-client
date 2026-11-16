/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FeedbackPage.h
 * @brief 指令反馈页面。
 */

#ifndef FEEDBACK_PAGE_H
#define FEEDBACK_PAGE_H

#include <QWidget>

namespace xplugin {

class FeedbackPage : public QWidget
{
    Q_OBJECT
public:
    explicit FeedbackPage(QWidget *parent = nullptr);
};

} // namespace xplugin

#endif // FEEDBACK_PAGE_H
