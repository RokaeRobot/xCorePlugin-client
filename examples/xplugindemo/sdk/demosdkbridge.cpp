#ifdef HAVE_ROKAE_SDK

#include "demosdkbridge.h"

#include "interface/interfacemanager.h"
#include "log/logmanager.h"

#include "3rdparty/rokaesdk/include/rokae/robot.h"

DemoSdkBridge *DemoSdkBridge::instance()
{
    static DemoSdkBridge bridge;
    return &bridge;
}

DemoSdkBridge::DemoSdkBridge(QObject *parent)
    : QObject(parent)
    , m_robot(nullptr)
    , m_bound(false)
{
    try {
        m_robot = new rokae::xMateRobot();
    } catch (const std::exception &e) {
        ELOG << "DemoSdkBridge create robot failed:" << e.what();
        m_robot = nullptr;
    }
}

rokae::xMateRobot *DemoSdkBridge::robot() const
{
    return m_robot;
}

bool DemoSdkBridge::connectByHmiIp(QString *errMsg)
{
    if (!m_robot) {
        if (errMsg) {
            *errMsg = QStringLiteral("robot is null");
        }
        return false;
    }
    if (!xplugin::xPluginInterface().getRobotConnected()) {
        if (errMsg) {
            *errMsg = QStringLiteral("HMI not connected");
        }
        return false;
    }

    const QString ip = xplugin::xPluginInterface().getRobotIP();
    try {
        m_robot->connectToRobot(ip.toStdString());
        if (errMsg) {
            *errMsg = QStringLiteral("connected to %1").arg(ip);
        }
        return true;
    } catch (const std::exception &e) {
        if (errMsg) {
            *errMsg = QString::fromLocal8Bit(e.what());
        }
        return false;
    }
}

bool DemoSdkBridge::disconnectSdk(QString *errMsg)
{
    if (!m_robot) {
        if (errMsg) {
            *errMsg = QStringLiteral("robot is null");
        }
        return false;
    }
    try {
        std::error_code ec;
        m_robot->disconnectFromRobot(ec);
        if (errMsg) {
            *errMsg = QStringLiteral("ec=%1 %2")
                          .arg(ec.value())
                          .arg(QString::fromLocal8Bit(ec.message().c_str()));
        }
        return ec.value() == 0;
    } catch (const std::exception &e) {
        if (errMsg) {
            *errMsg = QString::fromLocal8Bit(e.what());
        }
        return false;
    }
}

void DemoSdkBridge::bindNetworkSignals(QObject *context)
{
    if (m_bound) {
        return;
    }
    m_bound = true;

    QObject *ctx = context ? context : this;
    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkConnect,
            ctx, [this]() {
        QString err;
        const bool ok = connectByHmiIp(&err);
        ILOG << "DemoSdkBridge auto-connect:" << ok << err;
    });
    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect,
            ctx, [this](bool) {
        QString err;
        disconnectSdk(&err);
        ILOG << "DemoSdkBridge auto-disconnect:" << err;
    });
}

#endif // HAVE_ROKAE_SDK
