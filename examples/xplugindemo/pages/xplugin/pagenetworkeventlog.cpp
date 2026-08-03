#include "pagenetworkeventlog.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "event/xplugineventsystem.h"
#include "interface/interfacemanager.h"
#include "log/logmanager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

#ifdef HAVE_ROKAE_SDK
#include "../../sdk/demosdkbridge.h"
#endif

PageNetworkEventLog::PageNetworkEventLog(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QLineEdit *edLogMsg = new QLineEdit(tr("hello from xplugindemo"), this);
    QComboBox *cbLevel = new QComboBox(this);
    cbLevel->addItem(QStringLiteral("Info"), static_cast<int>(xplugin::enThrowLogLevel::Info));
    cbLevel->addItem(QStringLiteral("Warning"), static_cast<int>(xplugin::enThrowLogLevel::Warning));
    cbLevel->addItem(QStringLiteral("Error"), static_cast<int>(xplugin::enThrowLogLevel::Error));

    QWidget *gNet = layout.addGroup(tr("Network"));
    layout.addButton(gNet, QStringLiteral("connect sigNetworkConnect/DisConnect"),
                     tr("Subscribe InterfaceManager network signals."), [this, result]() {
        connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkConnect,
                this, [result]() {
            result->appendInfo(QStringLiteral("sigNetworkConnect fired, ip=%1")
                                   .arg(xplugin::xPluginInterface().getRobotIP()));
        });
        connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect,
                this, [result](bool status) {
            result->appendInfo(QStringLiteral("sigNetworkDisConnect status=%1").arg(status));
        });
        result->append(QStringLiteral("connect signals"), true, QStringLiteral("subscribed"));
    });
    layout.addButton(gNet, QStringLiteral("connectNetworkStatus(true/false)"),
                     tr("Register connect/disconnect callbacks by API."), [this, result]() {
        xplugin::xPluginInterface().connectNetworkStatus(true, PLUGIN_NAME, this, [result]() {
            result->appendInfo(QStringLiteral("connectNetworkStatus(connect) callback"));
        });
        xplugin::xPluginInterface().connectNetworkStatus(false, PLUGIN_NAME, this, [result]() {
            result->appendInfo(QStringLiteral("connectNetworkStatus(disconnect) callback"));
        });
        result->append(QStringLiteral("connectNetworkStatus"), true, QStringLiteral("registered"));
    });
    layout.addButton(gNet, QStringLiteral("disconnectNetworkStatus()"),
                     tr("Unregister network status callbacks."), [result]() {
        xplugin::xPluginInterface().disconnectNetworkStatus(true, PLUGIN_NAME);
        xplugin::xPluginInterface().disconnectNetworkStatus(false, PLUGIN_NAME);
        result->append(QStringLiteral("disconnectNetworkStatus"), true, QString());
    });

    QWidget *gEvt = layout.addGroup(tr("Event System"));
    layout.addButton(gEvt, QStringLiteral("initEventSystem()"),
                     tr("Initialize plugin event system."), [result]() {
        xplugin::xPluginInterface().initEventSystem();
        result->append(QStringLiteral("initEventSystem"), true, QString());
    });
    layout.addButton(gEvt, QStringLiteral("subscribe UserEvent"),
                     tr("Subscribe a custom user event."), [this, result]() {
        const QString id = xplugin::xPluginEvent().subscribe(
            xplugin::XPluginEventType(xplugin::UserEventStart + 1),
            [result](const xplugin::XPluginEvent &ev) {
                result->appendInfo(QStringLiteral("UserEvent data=%1 source=%2")
                                       .arg(demoVariantToString(ev.data()), ev.source()));
            },
            PLUGIN_NAME, this);
        result->append(QStringLiteral("subscribe"), !id.isEmpty(), id);
    });
    layout.addButton(gEvt, QStringLiteral("publish UserEvent"),
                     tr("Publish a custom user event."), [result]() {
        xplugin::xPluginEvent().publish(
            xplugin::XPluginEventType(xplugin::UserEventStart + 1),
            QStringLiteral("demo-data"), PLUGIN_NAME);
        result->append(QStringLiteral("publish"), true, QStringLiteral("demo-data"));
    });
    layout.addButton(gEvt, QStringLiteral("unsubscribeByType UserEvent"),
                     tr("Unsubscribe all handlers of user event type."), [result]() {
        const int n = xplugin::xPluginEvent().unsubscribeByType(
            xplugin::XPluginEventType(xplugin::UserEventStart + 1));
        result->append(QStringLiteral("unsubscribeByType"), true, QStringLiteral("removed=%1").arg(n));
    });
    layout.addButton(gEvt, QStringLiteral("subscribe RCPowerStatusChange"),
                     tr("Subscribe robot power status change event."), [this, result]() {
        const QString id = xplugin::xPluginEvent().subscribe(
            xplugin::XPluginEventType::RCPowerStatusChange,
            [result](const xplugin::XPluginEvent &ev) {
                result->appendInfo(QStringLiteral("RCPowerStatusChange=%1")
                                       .arg(demoVariantToString(ev.data())));
            },
            PLUGIN_NAME, this);
        result->append(QStringLiteral("subscribe RCPowerStatusChange"), !id.isEmpty(), id);
    });
    layout.addButton(gEvt, QStringLiteral("subscribe RCRunModelChange"),
                     tr("Subscribe run model change event."), [this, result]() {
        const QString id = xplugin::xPluginEvent().subscribe(
            xplugin::XPluginEventType::RCRunModelChange,
            [result](const xplugin::XPluginEvent &ev) {
                result->appendInfo(QStringLiteral("RCRunModelChange=%1")
                                       .arg(demoVariantToString(ev.data())));
            },
            PLUGIN_NAME, this);
        result->append(QStringLiteral("subscribe RCRunModelChange"), !id.isEmpty(), id);
    });
    layout.addButton(gEvt, QStringLiteral("Install BeforeRun filter sample"),
                     tr("Install BeforeRunStart/Stop filter sample."), [this, result]() {
        xplugin::xPluginEvent().subscribe(
            xplugin::XPluginEventType::RCBeforeRunStart,
            [](const xplugin::XPluginEvent &) {
                QTimer::singleShot(1000, []() {
                    xplugin::xPluginInterface().startRunning();
                });
            },
            PLUGIN_NAME, this);
        xplugin::xPluginEvent().subscribe(
            xplugin::XPluginEventType::RCBeforeRunStop,
            [](const xplugin::XPluginEvent &) {
                QTimer::singleShot(1000, []() {
                    xplugin::xPluginInterface().stopRunning();
                });
            },
            PLUGIN_NAME, this);
        xplugin::xPluginInterface().startRunningFilter(true);
        xplugin::xPluginInterface().stopRunningFilter(true);
        result->append(QStringLiteral("BeforeRun filters"), true, QStringLiteral("installed"));
    });

    QWidget *gLog = layout.addGroup(tr("Log / ThrowLog"));
    {
        QFormLayout *form = new QFormLayout;
        form->addRow(tr("Level"), cbLevel);
        form->addRow(tr("Message"), edLogMsg);
        qobject_cast<QVBoxLayout *>(gLog->layout())->addLayout(form);
    }
    layout.addButton(gLog, QStringLiteral("ThrowLog()"),
                     tr("Show a throw-log dialog message."), [result, cbLevel, edLogMsg]() {
        const int ret = xplugin::ThrowLog(
            PLUGIN_NAME,
            static_cast<xplugin::enThrowLogLevel>(cbLevel->currentData().toInt()),
            edLogMsg->text());
        result->append(QStringLiteral("ThrowLog"), ret == 0, QStringLiteral("ret=%1").arg(ret));
    });

#ifdef HAVE_ROKAE_SDK
    QWidget *gSdk = layout.addGroup(tr("Optional: Rokae SDK load sample (requires HAVE_ROKAE_SDK)"));
    QLabel *sdkHint = new QLabel(
        tr("Same as xpluginvision: after network connect, call SDK connectToRobot with getRobotIP().\n"
           "Enable it via HAVE_ROKAE_SDK in xplugindemo.pro."),
        gSdk);
    sdkHint->setWordWrap(true);
    sdkHint->setProperty("func", QVariant(QStringLiteral("hint")));
    qobject_cast<QVBoxLayout *>(gSdk->layout())->addWidget(sdkHint);
    layout.addButton(gSdk, QStringLiteral("DemoSdkBridge::connectByHmiIp()"),
                     tr("Connect SDK using HMI getRobotIP()."), [result]() {
        QString err;
        const bool ok = DemoSdkBridge::instance()->connectByHmiIp(&err);
        result->append(QStringLiteral("DemoSdkBridge::connectByHmiIp"), ok, err);
    });
    layout.addButton(gSdk, QStringLiteral("DemoSdkBridge::disconnectSdk()"),
                     tr("Disconnect Rokae SDK."), [result]() {
        QString err;
        const bool ok = DemoSdkBridge::instance()->disconnectSdk(&err);
        result->append(QStringLiteral("DemoSdkBridge::disconnectSdk"), ok, err);
    });
    layout.addButton(gSdk, QStringLiteral("Bind network signals to auto connect/disconnect SDK"),
                     tr("Auto connect/disconnect SDK on network signals."), [this, result]() {
        DemoSdkBridge::instance()->bindNetworkSignals(this);
        result->append(QStringLiteral("bindNetworkSignals"), true,
                       QStringLiteral("sigNetworkConnect/DisConnect -> SDK"));
    });
#else
    QWidget *gSdk = layout.addGroup(tr("Optional: Rokae SDK (disabled by default)"));
    QLabel *sdkHint = new QLabel(
        tr("HAVE_ROKAE_SDK is not defined; Rokae library is not linked.\n"
           "To verify HMI IP -> SDK connect, uncomment DEFINES += HAVE_ROKAE_SDK in xplugindemo.pro and rebuild."),
        gSdk);
    sdkHint->setWordWrap(true);
    sdkHint->setProperty("func", QVariant(QStringLiteral("hint")));
    qobject_cast<QVBoxLayout *>(gSdk->layout())->addWidget(sdkHint);
#endif

    layout.finish();
}
