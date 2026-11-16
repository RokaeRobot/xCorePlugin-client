/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file MotorConfigPage.cpp
 * @brief 电机参数页面。
 */

#include "MotorConfigPage.h"

namespace xplugin {
MotorConfigPage::MotorConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.motor"), "pnl_motor_config", "pnl_motor_config_form",
                        {
                            {"motor",
                             QString(),
                             {
                                 {"motor_calib_precision", tr("校准精度(±%)")},
                                 {"motor_hold_time", tr("保持时间ms")},
                                 {"motor_driver_precision", tr("电批精度‰")},
                                 {"motor_driver_power", tr("电批功率Nm")},
                                 {"motor_unit", tr("单位Nm/Kg")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
