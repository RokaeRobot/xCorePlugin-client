/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file BaseConfigPage.h
 * @brief 基础配置页面。
 */

#ifndef BASE_CONFIG_PAGE_H
#define BASE_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class BaseConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit BaseConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // BASE_CONFIG_PAGE_H
