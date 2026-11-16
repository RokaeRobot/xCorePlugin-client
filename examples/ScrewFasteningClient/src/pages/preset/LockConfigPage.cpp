/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file LockConfigPage.cpp
 * @brief 锁附配置页面。
 */

#include "LockConfigPage.h"

namespace xplugin {
LockConfigPage::LockConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.lock_config"), "pnl_lock_config", "pnl_lock_config_form",
                        {
                            {"lock",
                             QString(),
                             {
                                 {"lock_follow_torque_pct", tr("跟随锁付扭矩%")},
                                 {"lock_cylinder_origin_in", tr("气缸原点输入")},
                                 {"lock_cylinder_ctrl_out", tr("气缸控制输出")},
                                 {"lock_blow_material_out", tr("吹料输出")},
                                 {"lock_vacuum_ctrl_out", tr("真空控制输出")},
                                 {"lock_vacuum_detect_in", tr("真空检测输入")},
                                 {"lock_material_signal_in", tr("有料信号输入")},
                                 {"lock_blow_arrive_in", tr("吹式到料输入")},
                                 {"lock_pick_idle_speed", tr("取料空转速度")},
                                 {"lock_pick_idle_torque_pct", tr("取料空转扭矩%")},
                                 {"lock_vacuum_detect_enable", tr("真空检测开启")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
