/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PageFieldWidgets.h
 * @brief 关联页面控件与已保存的参数。
 */

#ifndef PAGE_FIELD_WIDGETS_H
#define PAGE_FIELD_WIDGETS_H

#include "core/settings/SaveResult.h"
#include "models/SettingsData.h"
class QWidget;

namespace screw {
/**
 * @brief 按参数名收集寄存器选择和可编辑的数值。
 * @param[in] page 配置页面，不能为空。
 * @return 收集到的参数，交给调用方保存。配方号由页面单独读取，实时反馈只用于显示。
 */
PageFields collectPageFields(QWidget *page);
/** @brief 填回已保存的参数并清空缺失的配置项。恢复期间阻断信号，实时反馈值继续由设备刷新。 */
void applyPageFields(QWidget *page, const PageFields &fields);

/** @brief 将普通页面保存到其配置分区。写入成功后更新页面的保存状态。 */
SaveResult savePageSettings(QWidget *page);

/** @brief 按页面的分区标识恢复已保存的参数。 */
void restorePageSettings(QWidget *page);
} // namespace screw
#endif // PAGE_FIELD_WIDGETS_H
