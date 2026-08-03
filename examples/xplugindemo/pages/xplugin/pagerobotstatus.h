#ifndef PAGEROBOTSTATUS_H
#define PAGEROBOTSTATUS_H

#include <QWidget>

/**
 * @brief 功能页: 机器人状态
 *
 * status API
 * Layout: DemoPageLayout. UI strings: English tr() + ts.
 */
class PageRobotStatus : public QWidget
{
    Q_OBJECT
public:
    explicit PageRobotStatus(QWidget *parent = nullptr);
};

#endif // PAGEROBOTSTATUS_H
