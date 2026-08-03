#ifndef PAGEIOREGISTER_H
#define PAGEIOREGISTER_H

#include <QWidget>

/**
 * @brief 功能页: IO/寄存器
 *
 * IO/register API
 * Layout: DemoPageLayout. UI strings: English tr() + ts.
 */
class PageIoRegister : public QWidget
{
    Q_OBJECT
public:
    explicit PageIoRegister(QWidget *parent = nullptr);
};

#endif // PAGEIOREGISTER_H
