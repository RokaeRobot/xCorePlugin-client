#ifndef RLCOMMANDCUSTOMPROPRETYWIDGET_H
#define RLCOMMANDCUSTOMPROPRETYWIDGET_H
#include <QWidget>
#include "rl/rlcontentbase.h"

namespace Ui {
class RLCommandCustomPropretyWidget;
}

namespace  xplugin{
class RLCommandCustomPropretyWidget : public RLContentBase
{
    Q_OBJECT

public:
    explicit RLCommandCustomPropretyWidget(const QString &type = "", QWidget *parent = nullptr);
    ~RLCommandCustomPropretyWidget();


    void FillWith(const QString &str) override;
    QString ToString() override;
    bool CheckInsert() override;

private:
    Ui::RLCommandCustomPropretyWidget *ui;
};
}

#endif // RLCOMMANDCUSTOMPROPRETYWIDGET_H
