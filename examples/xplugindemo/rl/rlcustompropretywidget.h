#ifndef RLCUSTOMPROPRETYWIDGET_H
#define RLCUSTOMPROPRETYWIDGET_H

#include <QWidget>
#include "rl/rlcontentbase.h"

namespace Ui {
class RLCustomPropretyWidget;
}

namespace  xplugin{
class RLCustomPropretyWidget : public RLContentBase
{
    Q_OBJECT

public:
    explicit RLCustomPropretyWidget(const QString &type = "", QWidget *parent = nullptr);
    ~RLCustomPropretyWidget();

    void FillWith(const QString &str) override;
    QString ToString() override;
    bool CheckReplace() override;

private:
    Ui::RLCustomPropretyWidget *ui;
};
}
#endif // RLCUSTOMPROPRETYWIDGET_H
