/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file IoOutputPage.cpp
 * @brief IO输出页面。
 */

#include "IoOutputPage.h"

namespace xplugin {
IoOutputPage::IoOutputPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(IoOutput, recipeSync, parent)
{
    auto *recipeCombo = buildRecipeForm("pnl_io_out", "io_out_no", tr("配方号"),
                                        {
                                            {"io_out_port", tr("端口")},
                                            {"io_out_state", tr("状态")},
                                        });
    finishSetup(recipeCombo);
}

} // namespace xplugin
