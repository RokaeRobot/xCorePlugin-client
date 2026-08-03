#ifndef DEMOSDKBRIDGE_H
#define DEMOSDKBRIDGE_H

/**
 * @brief Optional Rokae SDK bridge (compiled only when HAVE_ROKAE_SDK is defined)
 *
 * Shows how to connect SDK with HMI IP from xplugin (same pattern as xpluginvision).
 * Not a full motion demo.
 */

#ifdef HAVE_ROKAE_SDK

#include <QObject>
#include <QString>

namespace rokae {
class xMateRobot;
}

class DemoSdkBridge : public QObject
{
    Q_OBJECT
public:
    static DemoSdkBridge *instance();

    /** Connect SDK using xPluginInterface().getRobotIP() */
    bool connectByHmiIp(QString *errMsg = nullptr);
    bool disconnectSdk(QString *errMsg = nullptr);

    /** Subscribe sigNetworkConnect/DisConnect to auto connect/disconnect SDK */
    void bindNetworkSignals(QObject *context);

    rokae::xMateRobot *robot() const;

private:
    explicit DemoSdkBridge(QObject *parent = nullptr);

    rokae::xMateRobot *m_robot;
    bool m_bound;
};

#endif // HAVE_ROKAE_SDK

#endif // DEMOSDKBRIDGE_H