/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RunPage.h
 * @brief 运行控制页面。
 */

#ifndef RUN_PAGE_H
#define RUN_PAGE_H

#include <QWidget>

namespace xplugin {
class RunPage : public QWidget
{
    Q_OBJECT
public:
    /** @brief 创建运行控制页面和二级菜单。 */
    explicit RunPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // RUN_PAGE_H
