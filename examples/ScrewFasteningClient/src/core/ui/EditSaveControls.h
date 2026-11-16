/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file EditSaveControls.h
 * @brief 各设置页面共用的编辑和保存入口。
 */

#ifndef EDIT_SAVE_CONTROLS_H
#define EDIT_SAVE_CONTROLS_H

#include "core/settings/SaveResult.h"

#include <functional>

class QWidget;

namespace screw {

/**
 * @brief 创建编辑、取消和保存按钮区域，处理填写检查、操作确认和保存反馈。
 * @param[in] parent 按钮区域的父控件。
 * @param[in] settingsScope 按钮对应的设置页面。
 * @param[in] confirmText 保存确认文字；为空时使用默认提示。
 * @param[in] saveAction 页面保存函数；为空时保存本地通用设置。
 * @return 由 parent 管理的按钮容器；通过弱指针跟踪 settingsScope 的生命周期。
 */
QWidget *createEditSaveControls(QWidget *parent, QWidget *settingsScope,
                                const QString &confirmText = QString(),
                                const std::function<SaveResult()> &saveAction = {});

} // namespace screw

#endif // EDIT_SAVE_CONTROLS_H
