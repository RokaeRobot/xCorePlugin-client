#ifndef SHOWSTATUSBAR_H
#define SHOWSTATUSBAR_H

#include <QWidget>

namespace Ui {
class ShowStatusBar;
}

class ShowStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit ShowStatusBar(QWidget *parent = nullptr);
    ~ShowStatusBar();

    void updateText(const QString &text);

private:
    Ui::ShowStatusBar *ui;
};

#endif // SHOWSTATUSBAR_H
