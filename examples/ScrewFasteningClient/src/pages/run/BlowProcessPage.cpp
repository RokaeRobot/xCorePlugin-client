/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file BlowProcessPage.cpp
 * @brief 吹式工艺页面。
 */

#include "BlowProcessPage.h"
#include "core/ui/FormPageBuilder.h"

namespace xplugin {
BlowProcessPage::BlowProcessPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(BlowProcess, recipeSync, parent)
{
    setObjectName(QStringLiteral("pnl_blow"));
    const QVector<FormGroup> groups = {
        {"blow",
         QString(),
         {
             {"blow_safe_height", tr("安全高度")},
             {"blow_arrive_delay", tr("到料延时(ms)")},
             {"blow_lift_height", tr("抬起吹料高度")},
         }},
    };
    screw::buildFormContent(this, "pnl_blow_form", groups, false);
    finishSetup();
}

} // namespace xplugin
