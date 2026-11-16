/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RivetConfigPage.h
 * @brief 铆钉点配置页面。
 */

#ifndef RIVET_CONFIG_PAGE_H
#define RIVET_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class RivetConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit RivetConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // RIVET_CONFIG_PAGE_H
