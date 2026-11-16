/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file CommandPage.h
 * @brief 控制指令页面。
 *
 * 参数行分别执行置位或复位，写入成功后仅保存当前行。
 */

#ifndef COMMAND_PAGE_H
#define COMMAND_PAGE_H

#include <QWidget>

#include "models/SettingsData.h"

namespace screw {
struct ParamRowWidgets;
}

namespace xplugin {

class CommandPage : public QWidget
{
    Q_OBJECT
public:
    explicit CommandPage(QWidget *parent = nullptr);

    /**
     * @brief 从客户端本地设置文件恢复寄存器选择和待写入数值。
     */
    void loadStored();

private:
    /**
     * @brief 将最近读取的配置填回输入控件，缺失项清空，恢复期间阻断变更信号。
     */
    void applyToWidgets();

    /**
     * @brief 写入参数行，并在设备写入成功后保存当前行配置。
     * @param[in] key 参数在本地配置中的名称标识。
     * @param[in] row 参数行容器及输入控件。
     * @param[in] value 待写入的数值文本；复位传入 0。
     * @note 先写设备后存本地；本地保存失败不撤销已完成的设备写入。
     */
    void writeRow(const QString &key, const screw::ParamRowWidgets &row, const QString &value);

    /**
     * @brief 控制指令页面的本地配置读取对象。
     *
     * 从插件本地 settings.json 读取寄存器选择和设定值。
     * 单行修改由 writeRow() 写回 settings.json。
     */

    PageFields m_saved;                                       // 最近从文件读取或成功保存的控制指令配置。
};

} // namespace xplugin

#endif // COMMAND_PAGE_H
