/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ScrewPointPage.h
 * @brief 螺丝点页面。
 */

#ifndef SCREW_POINT_PAGE_H
#define SCREW_POINT_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class ScrewPointPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit ScrewPointPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // SCREW_POINT_PAGE_H
