/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file StatusPage.h
 * @brief 状态读取页面。
 */

#ifndef STATUS_PAGE_H
#define STATUS_PAGE_H

#include <QWidget>

namespace xplugin {

class StatusPage : public QWidget
{
    Q_OBJECT
public:
    explicit StatusPage(QWidget *parent = nullptr);
};

} // namespace xplugin

#endif // STATUS_PAGE_H
