#ifndef RLCUSTOMINSERTWIDGET_H
#define RLCUSTOMINSERTWIDGET_H

#include <QWidget>
#include "rl/rlcontentbase.h"

namespace Ui {
class RLCustomInsertWidget;
}

namespace  xplugin{
class RLCustomInsertWidget : public RLContentBase
{
    Q_OBJECT

public:
    explicit RLCustomInsertWidget(const QString &type = "", QWidget *parent = nullptr);
    ~RLCustomInsertWidget();

    void FillWith(const QString &str) override;
    QString ToString() override;
    bool CheckInsert() override;

private:
    Ui::RLCustomInsertWidget *ui;
};
}
#endif // RLCUSTOMINSERTWIDGET_H
