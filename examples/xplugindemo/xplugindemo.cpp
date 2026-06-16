#include "xplugindemo.h"
#include "plugincommon.h"
#include "interface/interfacemanager.h"
#include "rl/rlmanager.h"
#include "rl/rlcustominsertwidget.h"
#include "rl/rlcustompropretywidget.h"
#include "log/logmanager.h"
#include <QPushButton>

XPLUGIN_REGISTER(PLUGIN_NAME, XPluginDemo)

xplugin::XPluginDemo::XPluginDemo(QObject *parent)
    : xplugin::PluginBase(parent)
{
    ILOG << "XPluginDemo()";

    setDefaultGlobalSheet(false);

    setPluginLabel(tr("test demo"));
}

void xplugin::XPluginDemo::init()
{
    ILOG << "XPluginDemo::init()";

//    QString path = ":/xplugindemo/language/xplugindemo1.qm";
//    xplugin::RegistLanguage(PLUGIN_NAME, xplugin::enLanguage::Chinese, path);

    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkConnect, this, [=](){
       qDebug() << "xplugin::XPluginDemo : Connect";
    });

    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect, this, [=](bool status){
       qDebug() << "xplugin::XPluginDemo : DisConnect";
    });

    ILOG << "XPluginDemo::init()";
    m_widget = new ShowWidget;
    xplugin::CreateCenterWidget(PLUGIN_NAME, m_widget);

    m_bar = new ShowStatusBar;
    xplugin::CreateStatusBar(PLUGIN_NAME, m_bar, 1);

    m_expandWidget = new ExpandWidget;
    xplugin::CreateRightExpandWidgte(PLUGIN_NAME, m_expandWidget);

    QPushButton *btn = new QPushButton("ExpandRightWidget");
    connect(btn, &QPushButton::clicked, this, [](){
        ELOG << "xplugin::SwitchRightExpandWidgte(PLUGIN_NAME)";
        xplugin::SwitchRightExpandWidgte(PLUGIN_NAME);
    });

    xplugin::CreateStatusBar(PLUGIN_NAME, btn, 2);

    m_bar = new ShowStatusBar;
    xplugin::CreateStatusBar(PLUGIN_NAME, m_bar, 1);

    m_bar->updateText(tr("current proress"));

    QList<xplugin::PhysicalButton> left_buttons;
    QList<xplugin::PhysicalButton> right_buttons;

    for(int i = 0; i < 6 ;i++){
        xplugin::PhysicalButton phyBtnLeft;
        phyBtnLeft.text = tr("PhyBtnLeft%1").arg(i);
        phyBtnLeft.untouched_icon = QString("");
        phyBtnLeft.touched_icon = QString("");;
        phyBtnLeft.exec_function = [i](bool is_pressed){
            qDebug() << QString("btn left %1 Click!").arg(i);
        };
        left_buttons.append(phyBtnLeft);
    }

    for(int i = 0; i < 6 ;i++){
        xplugin::PhysicalButton phyBtnRight;
        phyBtnRight.text = tr("phyBtnRight%1").arg(i);
        phyBtnRight.untouched_icon = QString("");
        phyBtnRight.touched_icon = QString("");;
        phyBtnRight.exec_function = [i](bool is_pressed){
            qDebug() << QString("btn right %1 Click!").arg(i);
        };
        right_buttons.append(phyBtnRight);
    }

    xplugin::CreatePhyButtonList(PLUGIN_NAME, PLUGIN_NAME, left_buttons, right_buttons);
    xplugin::ChangePhyButtonStatus(PLUGIN_NAME, PLUGIN_NAME, true);

}

void xplugin::XPluginDemo::afterInit()
{
    // 初始化 RL 辅助编程指令
    m_rlManager = new xplugin::RLManager(this);

    m_rlManager->createTypeKey("Custom");
    m_rlManager->createSkeleton("Custom", "Custom");
    m_rlManager->createInsDescribe("Custom", tr("Describe"));
    m_rlManager->createPattern("Custom", QString("^\\s*%1\\s*(?://.*)?$").arg("Custom"));
    m_rlManager->createInsDefault("Custom", "Custom 100,101");
    m_rlManager->createPropertyWidget("Custom", [](){return new xplugin::RLCustomPropretyWidget("Custom");});
    m_rlManager->createInsertWidget("Custom", [](){return new xplugin::RLCustomInsertWidget("Custom");});
    // 增加Group
    m_rlManager->createTypeToGroup("demo", "Custom");

    m_rlManager->createGroups(tr("demo group"), "demo");
}

void xplugin::XPluginDemo::idle()
{

}

void xplugin::XPluginDemo::render()
{

}
