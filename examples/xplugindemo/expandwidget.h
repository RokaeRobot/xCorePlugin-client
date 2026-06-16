#ifndef EXPANDWIDGET_H
#define EXPANDWIDGET_H

#include <QWidget>

namespace Ui {
class ExpandWidget;
}

class ExpandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExpandWidget(QWidget *parent = nullptr);
    ~ExpandWidget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ExpandWidget *ui;
};

#endif // EXPANDWIDGET_H
