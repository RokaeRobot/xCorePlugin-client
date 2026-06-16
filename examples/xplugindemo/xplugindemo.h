#ifndef XPLUGINDEMO_H
#define XPLUGINDEMO_H

#include "ShowWidget.h"
#include "showstatusbar.h"
#include "xplugindemo_global.h"
#include "plugincommon.h"
#include "rl/rlmanager.h"
#include "expandwidget.h"

namespace  xplugin{
class RLContentBase;
class XPLUGINDEMO_EXPORT XPluginDemo : public PluginBase
{
    Q_OBJECT
public:
    XPluginDemo(QObject *parent = nullptr);

private:
    void init() override;
    void afterInit() override;

    void render() override;

    void idle() override;


private:
    ShowWidget *m_widget;
    ShowStatusBar *m_bar;
    ExpandWidget *m_expandWidget;


    xplugin::RLManager *m_rlManager;
};
}

#endif // XPLUGINDEMO_H
