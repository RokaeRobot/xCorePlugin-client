/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file LockConfigPage.h
 * @brief 锁附配置页面。
 */

#ifndef LOCK_CONFIG_PAGE_H
#define LOCK_CONFIG_PAGE_H

#include "PresetSectionPage.h"

namespace xplugin {
class LockConfigPage : public PresetSectionPage
{
    Q_OBJECT
public:
    explicit LockConfigPage(QWidget *parent = nullptr);
};
} // namespace xplugin
#endif // LOCK_CONFIG_PAGE_H
