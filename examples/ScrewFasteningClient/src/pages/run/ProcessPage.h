/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ProcessPage.h
 * @brief 工艺参数页面。
 */

#ifndef PROCESS_PAGE_H
#define PROCESS_PAGE_H

#include <QWidget>
#include "core/ui/FormSpec.h"
#include "core/settings/SaveResult.h"
#include "rl/ProcessType.h"

class QComboBox;
class RecipeSync;

namespace xplugin {
class ProcessPage : public QWidget
{
    Q_OBJECT
public:
    /** @brief 在父页面的按钮列中创建编辑、取消和保存按钮。 */
    QWidget *createSaveControls(QWidget *parent);

protected:
    /** @brief 指定工艺类型，并使用父页面统一管理的配方同步对象。 */
    ProcessPage(ProcessType type, RecipeSync *recipeSync, QWidget *parent);
    /** @brief 在参数分组上方创建 1～30 配方号选择器，选择器不作为寄存器参数保存。 */
    QComboBox *buildRecipeForm(const char *pageName, const char *recipeSuffix, const QString &recipeLabel,
                               const QVector<FormRow> &rows);
    /** @brief 派生类完成控件创建后调用；空选择器表示工艺只保存一份参数。 */
    void finishSetup(QComboBox *recipeCombo = nullptr);

private:
    /** @brief 恢复当前配方号对应的页面参数。 */
    void restore();
    /** @brief 保存当前配方并结束本地编辑，再同步完整集合；同步失败保留本地保存结果。 */
    screw::SaveResult save();
    /** @brief 切换配方号前检查未保存修改。 */
    void commitRecipe();

    const QString m_section;
    RecipeSync *m_recipeSync;           // 父页面管理的配方同步对象。
    QComboBox *m_recipeCombo = nullptr; // 配方号下拉框，无编号工艺使用空指针。
    bool m_applying = false;            // 是否正在从配置文件恢复页面。
};
} // namespace xplugin
#endif // PROCESS_PAGE_H
