/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueMeterConfigPage.h
 * @brief 扭矩测试仪配置页面。
 */

#ifndef TORQUE_METER_CONFIG_PAGE_H
#define TORQUE_METER_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class TorqueMeterConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit TorqueMeterConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // TORQUE_METER_CONFIG_PAGE_H
