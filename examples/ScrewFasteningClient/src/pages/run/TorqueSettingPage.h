/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueSettingPage.h
 * @brief 扭矩设置页面。
 */

#ifndef TORQUE_SETTING_PAGE_H
#define TORQUE_SETTING_PAGE_H

#include "ProcessPage.h"

namespace xplugin {
class TorqueSettingPage : public ProcessPage
{
    Q_OBJECT
public:
    explicit TorqueSettingPage(RecipeSync *recipeSync, QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // TORQUE_SETTING_PAGE_H
