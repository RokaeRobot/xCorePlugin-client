/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file BlowProcessPage.h
 * @brief 吹式工艺页面。
 */

#ifndef BLOW_PROCESS_PAGE_H
#define BLOW_PROCESS_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class BlowProcessPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit BlowProcessPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // BLOW_PROCESS_PAGE_H
