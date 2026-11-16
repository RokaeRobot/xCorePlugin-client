/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file IoWaitPage.cpp
 * @brief IO等待页面。
 */

#include "IoWaitPage.h"

namespace xplugin {
IoWaitPage::IoWaitPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(IoWait, recipeSync, parent)
{
    auto *recipeCombo = buildRecipeForm("pnl_io_wait", "io_wait_no", tr("配方号"),
                                        {
                                            {"io_wait_input", tr("输入")},
                                            {"io_wait_state", tr("状态")},
                                            {"io_wait_timeout", tr("超时时间ms")},
                                        });
    finishSetup(recipeCombo);
}

} // namespace xplugin
