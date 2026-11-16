/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file WarningConfigPage.h
 * @brief 预警配置页面。
 */

#ifndef WARNING_CONFIG_PAGE_H
#define WARNING_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class WarningConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit WarningConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // WARNING_CONFIG_PAGE_H
