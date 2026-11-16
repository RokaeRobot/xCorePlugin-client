/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file IoOutputPage.h
 * @brief IO输出页面。
 */

#ifndef IO_OUTPUT_PAGE_H
#define IO_OUTPUT_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class IoOutputPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit IoOutputPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // IO_OUTPUT_PAGE_H
