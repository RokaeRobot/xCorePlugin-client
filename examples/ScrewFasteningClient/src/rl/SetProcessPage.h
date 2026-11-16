/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SetProcessPage.h
 * @brief SetProcess 指令编辑。
 */

#ifndef SET_PROCESS_PAGE_H
#define SET_PROCESS_PAGE_H

#include "rl/rlcontentbase.h"

class QComboBox;

class SetProcessPage : public xplugin::RLContentBase
{
    Q_OBJECT
public:
    explicit SetProcessPage(QWidget *parent = nullptr);

    /**
     * @brief 将已有 RL 指令文本填入编辑页面。
     * @param[in] str 待编辑的指令文本。
     */
    void FillWith(const QString &str) override;

    /**
     * @brief 将页面选项转换为 RL 指令文本。
     * @return 生成的指令文本。
     * @note 吸式、吹式工艺仍输出选择器中的编号，控制器执行时忽略该编号。
     */
    QString ToString() override;

    /**
     * @brief 检查当前工艺选项是否允许插入。
     * @return 程序未运行且工艺选项有效时返回 true，否则返回 false。
     */
    bool CheckInsert() override;

    /** @brief 检查是否允许提交已有指令的修改，程序运行时拒绝修改。 */
    bool CheckReplace() override;

private:
    /**
     * @brief 根据程序运行状态更新指令编辑权限。
     *
     * 吸式和吹式工艺的配方号下拉框始终禁用。
     * @param[in] locked 是否禁止修改指令。
     */
    void applyInputLock(bool locked);

    QComboBox *m_type = nullptr;   // 工艺类型下拉框指针。
    QComboBox *m_index = nullptr;  // 配方号下拉框指针。
};

#endif // SET_PROCESS_PAGE_H
