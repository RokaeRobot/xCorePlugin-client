/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ScrewFastening.h
 * @brief 螺丝锁附插件的客户端入口。
 */

#ifndef SCREW_FASTENING_H
#define SCREW_FASTENING_H

#include <QPointer>
#include <QList>

#include "plugincommon.h"
#include "interface/interfacemanager.h"

class QEvent;
class QWidget;

namespace xplugin {

class PresetPage;

class ScrewFastening : public PluginBase
{
    Q_OBJECT
public:
    explicit ScrewFastening(QObject *parent = nullptr);

    /**
     * @brief 创建主界面、注册 SetProcess 编辑页并绑定连接与程序启停通知。
     */
    void init() override;

    /**
     * @brief 统一主界面和现存指令编辑页的字体、布局与样式，允许重复调用。
     */
    void render() override;

    /**
     * @brief 刷新寄存器选项，并为预设写入提示安装主界面事件过滤器。
     */
    void idle() override;

protected:
    /**
     * @brief 监听主界面绘制事件，满足提示条件时在绘制结束后询问是否写入已保存的预设。
     * @param[in] watched 被监视的对象指针。
     * @param[in] event 事件指针。
     * @return 是否停止事件继续传递。
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    /**
     * @brief 手动连接期间至多提示一次预设写入，确认后重新检查连接与运行锁。
     */
    void maybeAskPresetWrite();

    /** @brief 根据程序运行状态，控制参数编辑及控制指令页的置位、复位操作是否可用。 */
    void refreshInputLock();

    QPointer<QWidget> m_centerPage;    // 用于刷新主界面样式、应用运行限制和监听绘制事件。
    QPointer<PresetPage> m_presetPage; // 用于在连接后检查已保存的预设，并在用户确认后下发。
    QList<QPointer<QWidget>> m_commandPages; // 用于刷新编辑页样式，刷新时清除已销毁页面的记录。
    bool m_presetAsked = false;     // 当前手动连接期间是否已检查预设参数写入提示。
    bool m_runSignalActive = false; // 收到启动信号后保持运行锁，停止或连接状态变化时清除此标志。
};

} // namespace xplugin

#endif // SCREW_FASTENING_H
