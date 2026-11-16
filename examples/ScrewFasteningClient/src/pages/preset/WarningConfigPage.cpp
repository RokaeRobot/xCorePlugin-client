/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WarningConfigPage.cpp
 * @brief 预警配置页面。
 */

#include "WarningConfigPage.h"

namespace xplugin {
WarningConfigPage::WarningConfigPage(QWidget *parent)
    : PresetSectionPage(QStringLiteral("preset.warning"), "pnl_warning_config", "pnl_warning_config_form",
                        {
                            {"warning",
                             QString(),
                             {
                                 {"warn_bit_life_preset", tr("批头寿命预设")},
                                 {"warn_bit_alert_preset", tr("批头预警预设")},
                                 {"warn_nozzle_clamp_preset", tr("吸嘴夹嘴预设")},
                                 {"warn_nozzle_alert_preset", tr("吸夹嘴预警预设")},
                                 {"warn_filter_life_preset", tr("滤芯寿命预设")},
                                 {"warn_filter_alert_preset", tr("滤芯预警预设")},
                             }},
                        },
                        parent)
{
}
} // namespace xplugin
