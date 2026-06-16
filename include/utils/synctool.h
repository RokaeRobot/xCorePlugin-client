#ifndef SYNCTOOL_H
#define SYNCTOOL_H

#include <QObject>

class QProcess;
class QProgressDialog;
class QEventLoop;
#include "xplugin_global.h"

namespace xplugin {
class XPLUGIN_EXPORT SyncTool : public QObject
{
    Q_OBJECT
public:
    bool pushFile(const QString& from_path, const QString& to_module = "hmi", bool has_err = true);
    bool pushOneFile(const QString& from_path, const QString& to_module = "hmi", bool has_err = true);

    bool pullFile(const QString& from_module = "hmi", const QString& to_path = "", bool has_dialog = true, bool has_err = true);
    static SyncTool* instance();

private:
    SyncTool(QObject *parent = nullptr);
    ~SyncTool();

private:
    QProcess *m_process;
    QProgressDialog *m_progress_dlg;
    QEventLoop *m_loop;
    bool m_success;
    static SyncTool *m_instance;
};
}
#endif // SYNCTOOL_H
