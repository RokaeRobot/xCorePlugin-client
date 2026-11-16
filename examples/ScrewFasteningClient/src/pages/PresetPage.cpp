/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PresetPage.cpp
 * @brief 预设参数页面。
 */

#include "PresetPage.h"
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include "core/ui/FitStack.h"
#include "core/ui/NavBar.h"
#include "core/ui/StyleTokens.h"
#include "interface/interfacemanager.h"
#include "preset/BaseConfigPage.h"
#include "preset/RivetConfigPage.h"
#include "preset/LockConfigPage.h"
#include "preset/TorqueMeterConfigPage.h"
#include "preset/WarningConfigPage.h"
#include "preset/MotorConfigPage.h"

xplugin::PresetPage::PresetPage(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("pnl_preset_page"));

    auto *presetPageLayout = new QVBoxLayout(this);

    // 二级菜单与参数页按相同顺序建立，每个分类独立编辑和保存。
    QList<QPushButton *> pnl_preset_navButtons;
    auto *pnl_preset_nav = screw::createNavBar(
        {
            tr("基础配置"),
            tr("铆钉点配置"),
            tr("锁附配置"),
            tr("扭矩测试仪配置"),
            tr("预警配置"),
            tr("电机参数"),
        },
        style::kSecondaryNavButtonHeight, "pnl_preset_nav", this, &pnl_preset_navButtons);
    presetPageLayout->addWidget(pnl_preset_nav);

    // 左侧放置分类参数页，右侧底部显示当前分类的编辑保存按钮。
    auto *pnl_preset_body = new QWidget(this);
    pnl_preset_body->setObjectName(QStringLiteral("pnl_preset_body"));
    pnl_preset_body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto *presetBodyLayout = new QHBoxLayout(pnl_preset_body);
    presetPageLayout->addWidget(pnl_preset_body);

    presetPageLayout->addStretch();

    auto *pnl_preset_content = new QWidget(pnl_preset_body);
    pnl_preset_content->setObjectName(QStringLiteral("pnl_preset_content"));

    auto *presetContentColumn = new QVBoxLayout(pnl_preset_content);
    presetBodyLayout->addWidget(pnl_preset_content, 1);

    auto *pnl_preset_stack = new FitStack(pnl_preset_content);
    pnl_preset_stack->setObjectName(QStringLiteral("pnl_preset_stack"));
    presetContentColumn->addWidget(pnl_preset_stack);

    auto *pnl_preset_save = new QWidget(pnl_preset_body);
    pnl_preset_save->setObjectName(QStringLiteral("pnl_preset_save"));
    auto *presetSaveColumn = new QVBoxLayout(pnl_preset_save);
    presetSaveColumn->addStretch();
    presetBodyLayout->addWidget(pnl_preset_save, 0, Qt::AlignBottom);

    m_pages = {new BaseConfigPage(pnl_preset_stack),    new RivetConfigPage(pnl_preset_stack),
               new LockConfigPage(pnl_preset_stack),    new TorqueMeterConfigPage(pnl_preset_stack),
               new WarningConfigPage(pnl_preset_stack), new MotorConfigPage(pnl_preset_stack)};
    for (PresetSectionPage *page : m_pages)
        pnl_preset_stack->addWidget(page);

    // 复用右侧按钮列容器，销毁旧按钮后绑定当前分类，编辑状态仍由分类页面持有。
    auto showSaveButton = [this, presetSaveColumn, pnl_preset_save](int index) {
        while (QLayoutItem *item = presetSaveColumn->takeAt(0)) {
            if (QWidget *widget = item->widget())
                widget->deleteLater();
            delete item;
        }
        presetSaveColumn->addStretch();
        presetSaveColumn->addWidget(m_pages.at(index)->createSaveControls(pnl_preset_save), 0,
                                    Qt::AlignRight);
    };
    screw::bindNav(pnl_preset_stack, pnl_preset_navButtons);
    connect(pnl_preset_stack, &FitStack::currentChanged, this, showSaveButton);
    pnl_preset_stack->setCurrentIndex(0);
    showSaveButton(0);
}

bool xplugin::PresetPage::hasSavedPresets() const
{
    for (PresetSectionPage *page : m_pages) {
        if (page->hasSavedParameters())
            return true;
    }
    return false;
}

bool xplugin::PresetPage::writeSavedPresets()
{
    // 连接后的写入统一读取已保存参数，不采用页面上尚未保存的输入。
    screw::RegisterWriteResult count;
    for (PresetSectionPage *page : m_pages) {
        const auto result = page->writeSavedParameters();
        count.written += result.written;
        count.failed += result.failed;
    }
    // 全部分类处理后统一报告结果，部分写入失败也不能当作全部成功。
    if (count.written == 0 && count.failed == 0) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Info, tr("没有已保存的预设参数可写入"));
        return true;
    }
    if (count.failed > 0) {
        xplugin::ThrowLog(
            QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
            tr("预设参数写入未完成：成功 %1 项，失败 %2 项").arg(count.written).arg(count.failed));
        return false;
    }
    xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Info,
                      tr("预设参数已写入锁丝机，共 %1 项").arg(count.written));
    return true;
}
