/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RunPage.cpp
 * @brief 运行控制页面。
 */

#include "RunPage.h"
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include "core/ui/FitStack.h"
#include "core/ui/NavBar.h"
#include "core/ui/StyleTokens.h"
#include "core/controller/RecipeSync.h"
#include "run/TorqueSettingPage.h"
#include "run/SuctionProcessPage.h"
#include "run/BlowProcessPage.h"
#include "run/ScrewPointPage.h"
#include "run/IoOutputPage.h"
#include "run/IoWaitPage.h"

xplugin::RunPage::RunPage(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("pnl_run_page"));
    auto *recipeSync = new RecipeSync(this);

    auto *runPageLayout = new QVBoxLayout(this);

    // 创建六个工艺分类按钮，分类顺序与工艺页面对应。
    QList<QPushButton *> pnl_run_navButtons;
    auto *pnl_run_nav = screw::createNavBar(
        {
            tr("扭矩设置"),
            tr("吸式工艺"),
            tr("吹式工艺"),
            tr("螺丝点"),
            tr("IO输出"),
            tr("IO等待"),
        },
        style::kSecondaryNavButtonHeight, "pnl_run_nav", this, &pnl_run_navButtons);
    runPageLayout->addWidget(pnl_run_nav);

    // 工艺内容放在左侧，当前页面的编辑保存按钮放在右侧底部。
    auto *pnl_run_body = new QWidget(this);
    pnl_run_body->setObjectName(QStringLiteral("pnl_run_body"));
    pnl_run_body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto *runBodyLayout = new QHBoxLayout(pnl_run_body);
    runPageLayout->addWidget(pnl_run_body);

    runPageLayout->addStretch();

    auto *pnl_run_content = new QWidget(pnl_run_body);
    pnl_run_content->setObjectName(QStringLiteral("pnl_run_content"));

    auto *runContentColumn = new QVBoxLayout(pnl_run_content);
    runBodyLayout->addWidget(pnl_run_content, 1);

    auto *pnl_run_stack = new FitStack(pnl_run_content);
    pnl_run_stack->setObjectName(QStringLiteral("pnl_run_stack"));
    runContentColumn->addWidget(pnl_run_stack);

    auto *pnl_run_save = new QWidget(pnl_run_body);
    pnl_run_save->setObjectName(QStringLiteral("pnl_run_save"));
    auto *runSaveColumn = new QVBoxLayout(pnl_run_save);
    runSaveColumn->addStretch();
    runBodyLayout->addWidget(pnl_run_save, 0, Qt::AlignBottom);

    const QVector<ProcessPage *> pages = {
        new TorqueSettingPage(recipeSync, pnl_run_stack), new SuctionProcessPage(recipeSync, pnl_run_stack),
        new BlowProcessPage(recipeSync, pnl_run_stack),   new ScrewPointPage(recipeSync, pnl_run_stack),
        new IoOutputPage(recipeSync, pnl_run_stack),      new IoWaitPage(recipeSync, pnl_run_stack)};
    for (ProcessPage *page : pages)
        pnl_run_stack->addWidget(page);

    // 复用按钮列容器，重建当前工艺的操作按钮；各工艺共用同一个集合同步对象。
    auto showSaveButton = [pages, runSaveColumn, pnl_run_save](int index) {
        while (QLayoutItem *item = runSaveColumn->takeAt(0)) {
            if (QWidget *widget = item->widget())
                widget->deleteLater();
            delete item;
        }
        runSaveColumn->addStretch();
        runSaveColumn->addWidget(pages.at(index)->createSaveControls(pnl_run_save), 0, Qt::AlignRight);
    };
    screw::bindNav(pnl_run_stack, pnl_run_navButtons);
    connect(pnl_run_stack, &FitStack::currentChanged, this, showSaveButton);
    pnl_run_stack->setCurrentIndex(0);
    showSaveButton(0);
}
