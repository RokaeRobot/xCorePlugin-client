/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RivetConfigPage.cpp
 * @brief 铆钉点配置页面。
 */

#include "RivetConfigPage.h"

namespace xplugin {
RivetConfigPage::RivetConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.rivet_config"), "pnl_rivet_config", "pnl_rivet_config_form",
                        {
                            {"rivet",
                             QString(),
                             {
                                 {"rivet_x_axis_pos", tr("X轴（推轴）执行坐标")},
                                 {"rivet_x_axis_pos_3rd", tr("X轴（推轴）执行坐标(第三位小数)")},
                                 {"rivet_screw_detect_in", tr("螺丝检测输入")},
                                 {"rivet_screw_blow_out", tr("螺丝吹气输出")},
                                 {"rivet_throw_enable", tr("抛钉点启用")},
                                 {"rivet_blow_duration", tr("螺丝吹气时长")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
