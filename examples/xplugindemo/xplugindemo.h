#ifndef XPLUGINDEMO_H
#define XPLUGINDEMO_H

#include "expandwidget.h"
#include "pages/demoshell.h"
#include "showstatusbar.h"
#include "xplugindemo_global.h"
#include "plugincommon.h"
#include "rl/rlmanager.h"

namespace xplugin {

/**
 * @brief xplugindemo 插件主类（二次开发示例入口）
 *
 * 生命周期:
 * - init()      : Center / StatusBar / Expand / Phy + network
 * - afterInit() : RLManager
 * - idle/render : empty in this demo
 *
 * UI: tr("English"); zh in language/*.ts
 */
class XPLUGINDEMO_EXPORT XPluginDemo : public PluginBase
{
    Q_OBJECT
public:
    explicit XPluginDemo(QObject *parent = nullptr);

private:
    void init() override;
    void afterInit() override;
    void render() override;
    void idle() override;

private:
    DemoShell *m_shell;                 ///< 中心导航壳
    ShowStatusBar *m_bar;               ///< 状态栏
    ExpandWidget *m_expandWidget;       ///< 右侧拓展
    xplugin::RLManager *m_rlManager;    ///< RL
};

} // namespace xplugin

#endif // XPLUGINDEMO_H
