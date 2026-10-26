#ifndef RLCOMMANDCUSTOMINSERTWIDGET_H
#define RLCOMMANDCUSTOMINSERTWIDGET_H

#include <QWidget>
#include "rl/rlcontentbase.h"

namespace Ui {
class RLCommandCustomInsertWidget;
}
namespace  xplugin{
class RLCommandCustomInsertWidget : public RLContentBase
{
    Q_OBJECT

public:
    explicit RLCommandCustomInsertWidget(const QString &type = "", QWidget *parent = nullptr);
    ~RLCommandCustomInsertWidget();


    void FillWith(const QString &str) override;
    QString ToString() override;
    bool CheckInsert() override;

private:
    Ui::RLCommandCustomInsertWidget *ui;
};
}
#endif // RLCOMMANDCUSTOMINSERTWIDGET_H
