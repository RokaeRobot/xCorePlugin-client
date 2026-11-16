/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueMeterConfigPage.cpp
 * @brief 扭矩测试仪配置页面。
 */

#include "TorqueMeterConfigPage.h"

namespace xplugin {
TorqueMeterConfigPage::TorqueMeterConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.torque_meter"), "pnl_torque_meter_config",
                        "pnl_torque_meter_config_form",
                        {
                            {"torque_meter",
                             QString(),
                             {
                                 {"tm_smart_comm", tr("智能通讯")},
                                 {"tm_x_axis_pos", tr("X轴（推轴）执行坐标")},
                                 {"tm_x_axis_pos_3rd", tr("X轴（推轴）执行坐标(第三位小数)")},
                                 {"tm_station_no", tr("通讯站号")},
                                 {"tm_baud_rate", tr("波特率")},
                                 {"tm_observe_delay", tr("观察延时ms")},
                                 {"tm_reverse_speed", tr("反转速度")},
                                 {"tm_calibration_count", tr("校准次数")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
