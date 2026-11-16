/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SuctionProcessPage.h
 * @brief 吸式工艺页面。
 */

#ifndef SUCTION_PROCESS_PAGE_H
#define SUCTION_PROCESS_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class SuctionProcessPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit SuctionProcessPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // SUCTION_PROCESS_PAGE_H
