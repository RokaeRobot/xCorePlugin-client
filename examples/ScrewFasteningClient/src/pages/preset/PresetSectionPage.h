/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PresetSectionPage.h
 * @brief 预设分类页面。
 */

#ifndef PRESET_SECTION_PAGE_H
#define PRESET_SECTION_PAGE_H

#include <QWidget>
#include "core/ui/FormSpec.h"
#include "core/settings/SaveResult.h"

namespace screw {
/** @brief 已填写参数的逐项写入统计；失败不回滚，也不阻止后续参数写入。 */
struct RegisterWriteResult
{
    int written = 0;
    int failed = 0;
};
} // namespace screw

namespace xplugin {
class PresetSectionPage : public QWidget
{
public:
    /** @brief 在父页面的按钮列中创建编辑/保存按钮。 */
    QWidget *createSaveControls(QWidget *parent);
    /** @brief 检查配置文件中是否有已填写完整的预设参数。 */
    bool hasSavedParameters() const;
    /** @brief 将配置文件中的已保存参数写入锁丝机。 */
    screw::RegisterWriteResult writeSavedParameters() const;

protected:
    /** @brief 按参数定义构建表单，绑定独立的配置段和编辑状态。 */
    PresetSectionPage(const QString &section, const char *pageName, const char *formName,
                      const QVector<FormGroup> &groups, QWidget *parent);

private:
    /** @brief 从配置文件恢复页面参数。 */
    void restore();
    /** @brief 先保存本地并结束编辑，再逐项写设备；部分失败返回写入未完成状态。 */
    screw::SaveResult save();

    const QString m_section;
};
} // namespace xplugin
#endif // PRESET_SECTION_PAGE_H
