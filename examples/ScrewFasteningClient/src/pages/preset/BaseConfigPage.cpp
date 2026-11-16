/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file BaseConfigPage.cpp
 * @brief 基础配置页面。
 */

#include "BaseConfigPage.h"

namespace xplugin {
BaseConfigPage::BaseConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.base_config"), "pnl_base_config", "pnl_base_config_form",
                        {
                            {"axis_reset",
                             tr("X（推轴）复位"),
                             {
                                 {"axis_reset_origin", tr("X轴（推轴）复位回原")},
                                 {"axis_reset_speed", tr("X轴（推轴）回原速度")},
                             }},
                            {"io_out",
                             tr("IO输出端口号设置"),
                             {
                                 {"io_out_reset_safe", tr("复位安全输出")},
                                 {"io_out_work_running", tr("工作运行输出")},
                                 {"io_out_paused", tr("设备暂停输出")},
                                 {"io_out_error", tr("设备异常输出")},
                             }},
                            {"io_in",
                             tr("IO输入端口号设置"),
                             {
                                 {"io_in_done", tr("完成信号输出")},
                                 {"io_in_skip", tr("本次跳过输入")},
                                 {"io_in_continue", tr("本次继续输入")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
