#include "xplugindemo.h"

#include "interface/interfacemanager.h"
#include "log/logmanager.h"
#include "plugincommon.h"
#include "rl/rlcommandcustominsertwidget.h"
#include "rl/rlcommandcustompropretywidget.h"
#include "rl/rlcustominsertwidget.h"
#include "rl/rlcustompropretywidget.h"
#include "rl/rlmanager.h"

#include <QPushButton>

XPLUGIN_REGISTER(PLUGIN_NAME, XPluginDemo)

xplugin::XPluginDemo::XPluginDemo(QObject *parent)
    : xplugin::PluginBase(parent)
    , m_shell(nullptr)
    , m_bar(nullptr)
    , m_expandWidget(nullptr)
    , m_rlManager(nullptr)
{
    ILOG << "XPluginDemo()";
    setDefaultGlobalSheet(false);
    setPluginLabel(tr("test demo"));
}

void xplugin::XPluginDemo::init()
{
    ILOG << "XPluginDemo::init()";

    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkConnect, this, [=]() {
        qDebug() << "xplugin::XPluginDemo : Connect";
    });
    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect, this, [=](bool) {
        qDebug() << "xplugin::XPluginDemo : DisConnect";
    });

    // 中心页
    m_shell = new DemoShell;
    xplugin::CreateCenterWidget(PLUGIN_NAME, m_shell);

    // 状态栏
    m_bar = new ShowStatusBar;
    xplugin::CreateStatusBar(PLUGIN_NAME, m_bar, 1);
    m_bar->updateText(tr("current progress"));

    // 右侧拓展
    m_expandWidget = new ExpandWidget;
    xplugin::CreateRightExpandWidgte(PLUGIN_NAME, m_expandWidget);

    QPushButton *btn = new QPushButton(tr("Expand Panel"));
    btn->setObjectName(QStringLiteral("statusExpandBtn"));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(28);
    btn->setStyleSheet(QStringLiteral(
        "QPushButton#statusExpandBtn {"
        "  border: none; border-radius: 4px;"
        "  color: #ffffff; background-color: #224D75;"
        "  min-height: 28px; padding: 2px 12px;"
        "  font: 9pt \"Microsoft YaHei UI\";"
        "}"
        "QPushButton#statusExpandBtn:hover { background-color: #2a5f8f; }"
        "QPushButton#statusExpandBtn:pressed {"
        "  background-color: #D9E6F2; color: #091C36;"
        "}"
    ));
    connect(btn, &QPushButton::clicked, this, []() {
        xplugin::SwitchRightExpandWidgte(PLUGIN_NAME);
    });
    xplugin::CreateStatusBar(PLUGIN_NAME, btn, 2);

    QList<xplugin::PhysicalButton> left_buttons;
    QList<xplugin::PhysicalButton> right_buttons;
    for (int i = 0; i < 6; ++i) {
        xplugin::PhysicalButton phyBtnLeft;
        phyBtnLeft.text = tr("PhyBtnLeft%1").arg(i);
        phyBtnLeft.untouched_icon = QString();
        phyBtnLeft.touched_icon = QString();
        phyBtnLeft.exec_function = [i](bool) {
            qDebug() << QStringLiteral("btn left %1 Click!").arg(i);
        };
        left_buttons.append(phyBtnLeft);
    }
    for (int i = 0; i < 6; ++i) {
        xplugin::PhysicalButton phyBtnRight;
        phyBtnRight.text = tr("phyBtnRight%1").arg(i);
        phyBtnRight.untouched_icon = QString();
        phyBtnRight.touched_icon = QString();
        phyBtnRight.exec_function = [i](bool) {
            qDebug() << QStringLiteral("btn right %1 Click!").arg(i);
        };
        right_buttons.append(phyBtnRight);
    }
    xplugin::CreatePhyButtonList(PLUGIN_NAME, PLUGIN_NAME, left_buttons, right_buttons);
    xplugin::ChangePhyButtonStatus(PLUGIN_NAME, PLUGIN_NAME, true);
}

void xplugin::XPluginDemo::afterInit()
{
    // RL
    m_rlManager = new xplugin::RLManager(this);
    m_rlManager->createTypeKey(QStringLiteral("Custom"));
    m_rlManager->createSkeleton(QStringLiteral("Custom"), QStringLiteral("Custom"));
    m_rlManager->createInsDescribe(QStringLiteral("Custom"), tr("Describe"));
    m_rlManager->createPattern(QStringLiteral("Custom"),
                               QStringLiteral("^\\s*%1\\s*(?://.*)?$").arg(QStringLiteral("Custom")));
    m_rlManager->createInsDefault(QStringLiteral("Custom"), QStringLiteral("Custom 100,101"));
    m_rlManager->createPropertyWidget(QStringLiteral("Custom"), []() {
        return new xplugin::RLCustomPropretyWidget(QStringLiteral("Custom"));
    });
    m_rlManager->createInsertWidget(QStringLiteral("Custom"), []() {
        return new xplugin::RLCustomInsertWidget(QStringLiteral("Custom"));
    });
    m_rlManager->createTypeToGroup(QStringLiteral("demo"), QStringLiteral("Custom"));
    m_rlManager->createGroups(tr("demo group"), QStringLiteral("demo"));

    // RL 缺省参数与批量修改示例（dev_rl 9cb5a97）
    m_rlManager->createTypeKey(QStringLiteral("VPosToRobPos"));
    m_rlManager->createSkeleton(QStringLiteral("VPosToRobPos"), QStringLiteral("VPosToRobPos(VisionCodNum, VPos[, RobPos_JP]);"));
    m_rlManager->createInsDescribe(QStringLiteral("VPosToRobPos"),
                                   tr("Convert position information from the camera coordinate system to the workpiece coordinate system.")
                                   + QStringLiteral("\n\n")
                                   + QStringLiteral("robtarget = VPosToRobPos(VisionCodNum, VPos[, RobPos_JP]);"));
    m_rlManager->createPattern(QStringLiteral("VPosToRobPos"),
                               QStringLiteral("^\\s*%1\\s*(?://.*)?$").arg(QStringLiteral("Vision")));
    m_rlManager->createInsDefault(QStringLiteral("VPosToRobPos"), QStringLiteral("VPosToRobPos(1, p1"));
    m_rlManager->createPropertyWidget(QStringLiteral("VPosToRobPos"), []() {
        return new xplugin::RLCommandCustomInsertWidget(QStringLiteral("VPosToRobPos"));
    });
    m_rlManager->createInsertWidget(QStringLiteral("VPosToRobPos"), []() {
        return new xplugin::RLCommandCustomPropretyWidget(QStringLiteral("VPosToRobPos"));
    });
    m_rlManager->createTypeToGroup(QStringLiteral("groupVision"), QStringLiteral("VPosToRobPos"));
    m_rlManager->createGroups(tr("Vision Server"), QStringLiteral("groupVision"));

    xplugin::RlCustomCommandSpec spec;
    spec.type = QStringLiteral("VCommandPosToRobPos");
    spec.describe = tr("Describe");
    spec.defaultLine = QStringLiteral("VCommandPosToRobPos(1, p1);");
    spec.allowAssignPrefix = false;
    spec.fields << xplugin::RlCustomField(QStringLiteral("vSwitch"), xplugin::RlFieldSwitch, xplugin::ARG_DEFAULT)
                << xplugin::RlCustomField(QStringLiteral("vPointIndex"), xplugin::RlFieldInteger, xplugin::ARG_DEFAULT)
                << xplugin::RlCustomField(QStringLiteral("vPos"), xplugin::RlFieldCartPoint)
                << xplugin::RlCustomField(QStringLiteral("vSpeed"), xplugin::RlFieldSpeed)
                << xplugin::RlCustomField(QStringLiteral("vZone"), xplugin::RlFieldZone)
                << xplugin::RlCustomField(QStringLiteral("vTool"), xplugin::RlFieldTool)
                << xplugin::RlCustomField(QStringLiteral("vWobj"), xplugin::RlFieldWobj)
                << xplugin::RlCustomField(QStringLiteral("vPosJoint1"), xplugin::RlFieldJointPoint, xplugin::ARG_DEFAULT)
                << xplugin::RlCustomField(QStringLiteral("vPosJoint2"), xplugin::RlFieldJointPoint, xplugin::ARG_DEFAULT)
                << xplugin::RlCustomField(QStringLiteral("vPosJoint3"), xplugin::RlFieldJointPoint, xplugin::ARG_DEFAULT);
    m_rlManager->createData(QStringLiteral("groupVision"), spec,
        [type = spec.type]() { return new xplugin::RLCommandCustomInsertWidget(type); },
        [type = spec.type]() { return new xplugin::RLCommandCustomPropretyWidget(type); });
}

void xplugin::XPluginDemo::idle()
{
}

void xplugin::XPluginDemo::render()
{
}
