/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file IoWaitPage.h
 * @brief IO等待页面。
 */

#ifndef IO_WAIT_PAGE_H
#define IO_WAIT_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class IoWaitPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit IoWaitPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // IO_WAIT_PAGE_H
