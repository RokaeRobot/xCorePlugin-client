#ifndef PAGENETWORKEVENTLOG_H
#define PAGENETWORKEVENTLOG_H

#include <QWidget>

/**
 * @brief 功能页: 网络/Event/Log
 *
 * network/event/log API
 * Layout: DemoPageLayout. UI strings: English tr() + ts.
 */
class PageNetworkEventLog : public QWidget
{
    Q_OBJECT
public:
    explicit PageNetworkEventLog(QWidget *parent = nullptr);
};

#endif // PAGENETWORKEVENTLOG_H
