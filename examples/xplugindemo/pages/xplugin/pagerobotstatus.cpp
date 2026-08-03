#include "pagerobotstatus.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

PageRobotStatus::PageRobotStatus(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QWidget *gGet = layout.addGroup(tr("Read Status"));
    layout.addButton(gGet, QStringLiteral("getRobotConnected()"),
                     tr("Whether HMI is connected to controller."), [result]() {
        const bool v = xplugin::xPluginInterface().getRobotConnected();
        result->append(QStringLiteral("getRobotConnected"), true,
                       v ? QStringLiteral("true") : QStringLiteral("false"));
    });
    layout.addButton(gGet, QStringLiteral("getRobotID()"),
                     tr("Get robot unique ID."), [result]() {
        result->append(QStringLiteral("getRobotID"), true, xplugin::xPluginInterface().getRobotID());
    });
    layout.addButton(gGet, QStringLiteral("getRobotEleStatus()"),
                     tr("Get robot power on/off status."), [result]() {
        const bool v = xplugin::xPluginInterface().getRobotEleStatus();
        result->append(QStringLiteral("getRobotEleStatus"), true,
                       v ? QStringLiteral("true") : QStringLiteral("false"));
    });
    layout.addButton(gGet, QStringLiteral("getRobotOperateIsHand()"),
                     tr("Whether operate mode is manual (hand)."), [result]() {
        const bool v = xplugin::xPluginInterface().getRobotOperateIsHand();
        result->append(QStringLiteral("getRobotOperateIsHand"), true,
                       v ? QStringLiteral("true") : QStringLiteral("false"));
    });
    layout.addButton(gGet, QStringLiteral("getRobotPhyStatus()"),
                     tr("Get robot physical/run status string."), [result]() {
        result->append(QStringLiteral("getRobotPhyStatus"), true,
                       xplugin::xPluginInterface().getRobotPhyStatus());
    });
    layout.addButton(gGet, QStringLiteral("getUserOperateLevel()"),
                     tr("Get current user permission level."), [result]() {
        result->append(QStringLiteral("getUserOperateLevel"), true,
                       QString::number(xplugin::xPluginInterface().getUserOperateLevel()));
    });
    layout.addButton(gGet, QStringLiteral("getRobotIP()"),
                     tr("Get connected controller IP address."), [result]() {
        result->append(QStringLiteral("getRobotIP"), true, xplugin::xPluginInterface().getRobotIP());
    });
    layout.addButton(gGet, QStringLiteral("getRobotType()"),
                     tr("Get robot type/model name."), [result]() {
        result->append(QStringLiteral("getRobotType"), true, xplugin::xPluginInterface().getRobotType());
    });
    layout.addButton(gGet, QStringLiteral("getRobotModelType()"),
                     tr("Get robot device model type code."), [result]() {
        result->append(QStringLiteral("getRobotModelType"), true,
                       QString::number(xplugin::xPluginInterface().getRobotModelType()));
    });

    QWidget *gSet = layout.addGroup(tr("Power / Manual-Auto (confirm before write)"));
    layout.addButton(gSet, QStringLiteral("changeRobotPowerStatue(!ele)"),
                     tr("Toggle robot power status (write)."), [result]() {
        const bool cur = xplugin::xPluginInterface().getRobotEleStatus();
        const bool ok = xplugin::xPluginInterface().changeRobotPowerStatue(!cur);
        result->append(QStringLiteral("changeRobotPowerStatue"), ok,
                       QStringLiteral("from %1 -> %2").arg(cur).arg(!cur));
    });
    layout.addButton(gSet, QStringLiteral("changeRobotModelStatue(!hand)"),
                     tr("Toggle manual/auto operate mode (write)."), [result]() {
        const bool cur = xplugin::xPluginInterface().getRobotOperateIsHand();
        const bool ok = xplugin::xPluginInterface().changeRobotModelStatue(!cur);
        result->append(QStringLiteral("changeRobotModelStatue"), ok,
                       QStringLiteral("from hand=%1 -> %2").arg(cur).arg(!cur));
    });

    QWidget *gRun = layout.addGroup(tr("Program Run / Filter"));
    layout.addButton(gRun, QStringLiteral("startRunningFilter(true)"),
                     tr("Install start-running filter."), [result]() {
        result->append(QStringLiteral("startRunningFilter(true)"),
                       xplugin::xPluginInterface().startRunningFilter(true), QString());
    });
    layout.addButton(gRun, QStringLiteral("stopRunningFilter(true)"),
                     tr("Install stop-running filter."), [result]() {
        result->append(QStringLiteral("stopRunningFilter(true)"),
                       xplugin::xPluginInterface().stopRunningFilter(true), QString());
    });
    layout.addButton(gRun, QStringLiteral("startRunningFilter(false)"),
                     tr("Remove start-running filter."), [result]() {
        result->append(QStringLiteral("startRunningFilter(false)"),
                       xplugin::xPluginInterface().startRunningFilter(false), QString());
    });
    layout.addButton(gRun, QStringLiteral("stopRunningFilter(false)"),
                     tr("Remove stop-running filter."), [result]() {
        result->append(QStringLiteral("stopRunningFilter(false)"),
                       xplugin::xPluginInterface().stopRunningFilter(false), QString());
    });
    layout.addButton(gRun, QStringLiteral("startRunning()"),
                     tr("Start current program running."), [result]() {
        result->append(QStringLiteral("startRunning"),
                       xplugin::xPluginInterface().startRunning(), QString());
    });
    layout.addButton(gRun, QStringLiteral("stopRunning()"),
                     tr("Stop current program running."), [result]() {
        result->append(QStringLiteral("stopRunning"),
                       xplugin::xPluginInterface().stopRunning(), QString());
    });

    layout.finish();
}
