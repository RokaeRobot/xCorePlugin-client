/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SaveResult.h
 * @brief 页面保存的结果及提示内容。
 */

#ifndef SAVE_RESULT_H
#define SAVE_RESULT_H

#include <QString>

namespace screw {

/** @brief 区分本地保存失败与本地已保存、后续同步或设备写入未完成。 */
enum class SaveStatus {
    NotSaved,                // 本地配置未保存。
    Saved,                   // 本地保存及所需的后续操作均已完成。
    RecipeSyncIncomplete,   // 本地配方已保存，控制器同步未完成。
    RegisterWriteIncomplete  // 本地预设已保存，至少一项寄存器写入未完成。
};

/** @brief 页面保存结果；detail 说明未完成的步骤，后续操作失败不撤销本地保存。 */
struct SaveResult
{
    SaveStatus status;
    QString detail;
};

} // namespace screw

#endif // SAVE_RESULT_H
