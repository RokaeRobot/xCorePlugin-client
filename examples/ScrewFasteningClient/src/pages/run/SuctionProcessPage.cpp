/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SuctionProcessPage.cpp
 * @brief 吸式工艺页面。
 */

#include "SuctionProcessPage.h"
#include "core/ui/FormPageBuilder.h"

namespace xplugin {
SuctionProcessPage::SuctionProcessPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(SuctionProcess, recipeSync, parent)
{
    setObjectName(QStringLiteral("pnl_suction"));
    const QVector<FormGroup> groups = {
        {"suction",
         QString(),
         {
             {"x_axis_pos", tr("X轴（推轴）当前位置")},
             {"x_axis_pos_3rd", tr("X轴（推轴）当前位置（第三位小数）")},
             {"safe_height", tr("安全高度")},
             {"pick_time", tr("取料时间(ms)")},
             {"follow_height", tr("跟随高度(mm)")},
             {"follow_speed", tr("跟随速度(mm)")},
             {"material_signal_in", tr("有料信号输入")},
             {"align_signal_out", tr("扶正输出信号")},
         }},
    };
    screw::buildFormContent(this, "pnl_suction_form", groups, false);
    finishSetup();
}

} // namespace xplugin
