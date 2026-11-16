/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SaveValidation.h
 * @brief 保存前检查参数行是否填写完整。
 */
#ifndef SAVE_VALIDATION_H
#define SAVE_VALIDATION_H

#include <QString>
class QWidget;

namespace screw {
/** @brief 返回首个参数行、配方号或曲线来源校验错误；通过时返回空字符串。 */
QString settingsValidationError(QWidget *scope);
}
#endif
