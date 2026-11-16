/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file StyleTokens.h
 * @brief 插件界面共用的尺寸和颜色。
 *
 * 布局与控件外观在 render() 中统一设置。
 */

#ifndef STYLE_TOKENS_H
#define STYLE_TOKENS_H

namespace style {

constexpr const char *kNavCheckedBackground = "#c2252b"; // 菜单按钮选中时的背景颜色。
constexpr const char *kNavCheckedTextColor = "#FFFFFF";  // 菜单按钮选中时的文字颜色。

// 一级菜单、二级菜单和编辑保存按钮的尺寸，单位为像素。
constexpr int kPrimaryNavWidth = 144;         // 一级菜单宽度。
constexpr int kPrimaryNavButtonHeight = 56;   // 一级菜单按钮高度。
constexpr int kSecondaryNavButtonHeight = 44; // 二级菜单按钮最小高度。
constexpr int kSaveButtonMinWidth = 88;       // 编辑保存按钮最小宽度。
constexpr int kSaveButtonMinHeight = 44;      // 编辑保存按钮最小高度。

// 标签测量后的额外留白，单位为像素。
constexpr int kLabelExtraWidth = 12; // 标签文字测量宽度之外的预留宽度。

} // namespace style

#endif // STYLE_TOKENS_H
