#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>

namespace Ui {
class ShowWidget;
}

class ShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShowWidget(QWidget *parent = nullptr);
    ~ShowWidget();

    void updateSheetStyle();

private slots:
    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_throwLog_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_addPoint_clicked();

    void on_movetostart_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_25_clicked();

    void on_btnFullScreen_clicked();

    void on_btnModelWindow_clicked();

    void on_btnChangePower_clicked();

    void on_btnChangeModel_clicked();

    void on_btnSubscribe_clicked();

    void on_btnPublish_clicked();

    void on_btnUnSubscribe_clicked();

    void on_btnBeforeInstall_clicked();

    void on_btnUnInstall_clicked();

private:
    Ui::ShowWidget *ui;
};

#endif // SHOWWIDGET_H
