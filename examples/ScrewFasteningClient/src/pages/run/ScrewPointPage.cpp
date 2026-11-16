/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ScrewPointPage.cpp
 * @brief 螺丝点页面。
 */

#include "ScrewPointPage.h"

namespace xplugin {
ScrewPointPage::ScrewPointPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(ScrewPoint, recipeSync, parent)
{
    auto *recipeCombo = buildRecipeForm("pnl_screw_point", "screw_no", tr("配方号"),
                                        {
                                            {"screw_x_axis_pos", tr("X轴（推轴）当前位置")},
                                            {"screw_x_axis_pos_3rd", tr("X轴（推轴）当前位置（第三位小数）")},
                                            {"screw_index", tr("螺丝序号")},
                                            {"screw_position_index", tr("位置编号")},
                                            {"screw_relock", tr("复锁功能")},
                                        });
    finishSetup(recipeCombo);
}

} // namespace xplugin
