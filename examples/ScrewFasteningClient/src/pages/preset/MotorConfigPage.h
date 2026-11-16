/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file MotorConfigPage.h
 * @brief 电机参数页面。
 */

#ifndef MOTOR_CONFIG_PAGE_H
#define MOTOR_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class MotorConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit MotorConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // MOTOR_CONFIG_PAGE_H
