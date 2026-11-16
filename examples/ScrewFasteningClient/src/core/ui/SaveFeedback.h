/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SaveFeedback.h
 * @brief 各设置页面共用的保存结果提示。
 */

#ifndef SAVE_FEEDBACK_H
#define SAVE_FEEDBACK_H

#include <QString>

class QDialog;
class QWidget;

namespace screw {

/**
 * @brief 显示保存成功提示。
 *
 * 提示阻止所属窗口接收输入，500 毫秒后自动关闭并销毁。
 * @param[in] parent 提示所属的窗口。
 * @return 已显示的提示窗口；关闭后指针失效。
 */
QDialog *showSaveSuccess(QWidget *parent);

/**
 * @brief 显示保存失败或后续操作未完成的提示。
 *
 * 提示阻止所属窗口接收输入，用户关闭提示后销毁。
 * @param[in] parent 提示所属的窗口。
 * @param[in] detail 未完成的步骤和原因。
 * @param[in] title 提示标题；为空时显示默认的保存失败标题。
 * @return 已显示的提示窗口；关闭后指针失效。
 */
QDialog *showSaveFailure(QWidget *parent, const QString &detail, const QString &title = QString());

} // namespace screw

#endif // SAVE_FEEDBACK_H
