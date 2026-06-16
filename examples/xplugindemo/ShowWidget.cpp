#include "ShowWidget.h"
#include "ui_ShowWidget.h"
#include "log/logmanager.h"
#include <QJsonValue>
#include <QJsonObject>
#include "interface/interfacemanager.h"
#include <QTimer>
#include <iostream>

#include <QFileDialog>
#include <memory>
#include <thread>
#include <cmath>
#include <array>

#define M_PI 3.14

#include "event/xplugineventsystem.h"

ShowWidget::ShowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowWidget)
{
    ui->setupUi(this);
}

ShowWidget::~ShowWidget()
{
    ELOG << "ShowWidget::~ShowWidget() begin";
    delete ui;

    ELOG << "ShowWidget::~ShowWidget() end";
}

void ShowWidget::updateSheetStyle()
{
    setStyleSheet("*{background:yellow;} #pushButton_1{color:blue;} ");
}

void ShowWidget::on_pushButton_1_clicked()
{
    bool status = xplugin::xPluginInterface().getRobotConnected();
    ui->label_1->setText(QString(status));
}


void ShowWidget::on_pushButton_2_clicked()
{
    QStringList status = xplugin::xPluginInterface().getPointNameList();
    ui->label_2->setText(status.join(","));
}


void ShowWidget::on_pushButton_3_clicked()
{
    QStringList status = xplugin::xPluginInterface().getVarDataList();
    ui->label_3->setText(status.join(","));
}


void ShowWidget::on_pushButton_4_clicked()
{
    QStringList status = xplugin::xPluginInterface().getToolList();
    ui->label_4->setText(status.join(","));
}

void ShowWidget::on_throwLog_clicked()
{
    xplugin::ThrowLog(PLUGIN_NAME, xplugin::enThrowLogLevel(ui->comboBox->currentIndex()), ui->msg->text());
}

void ShowWidget::on_pushButton_5_clicked()
{
    QStringList status = xplugin::xPluginInterface().getToolList();
    ui->label_4->setText(status.join(","));
}


void ShowWidget::on_pushButton_6_clicked()
{
    QHash<QString, QStringList> hash = xplugin::xPluginInterface().getRegisterHash();
    for(auto itor = hash.begin(); itor != hash.end(); itor++){
        ILOG << itor.key() << " : " << itor.value();
    }
}

void ShowWidget::on_pushButton_7_clicked()
{
    ELOG << xplugin::xPluginInterface().getRegisterValue();
}

void ShowWidget::on_pushButton_8_clicked()
{
    ELOG << xplugin::xPluginInterface().getIOHash();
}


void ShowWidget::on_pushButton_9_clicked()
{
    auto fun = [](const QVariant &data){
        ELOG << "get tasks_pointer callback data!";
    };
    xplugin::xPluginInterface().connectXServiceData(PLUGIN_NAME, "project.tasks_pointer", fun);
}


void ShowWidget::on_pushButton_clicked()
{
    xplugin::xPluginInterface().addCurrentPoint("joint_test_1", true,  "tool0", "wobj0");

    xplugin::xPluginInterface().addCurrentPoint("cart_test_1", false,  "tool0", "wobj0");

}

void ShowWidget::on_pushButton_10_clicked()
{
    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkConnect, this, []{
        SLOG << "net work connect!";
    });

    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect, this, []{
        SLOG << "net work disconnect!";
    });

    xplugin::xPluginInterface().connectNetworkStatus(true, PLUGIN_NAME, this, [](){
        SLOG << "net work connect way 2!";

    });

    xplugin::xPluginInterface().connectNetworkStatus(false, PLUGIN_NAME, this, [](){
        SLOG << "net work disconnect way 2!";
    });
}

void ShowWidget::on_pushButton_11_clicked()
{
    xplugin::xPluginInterface().setXServiceData("project.setting", "max_manual_speed", 100);

    QJsonObject obj;
    obj.insert("tool", "tool0");
    obj.insert("wobj", "wobj0");
    xplugin::xPluginInterface().setXServiceData("move.moveto_select_info", obj);
}


void ShowWidget::on_pushButton_12_clicked()
{
    QJsonObject obj;
    obj.insert("target_language", "cn");
    xplugin::xPluginInterface().commandXServiceData("user_log.change_language", obj);
    SLOG << "commandXServiceData";
    QTimer::singleShot(3000, this, [](){
        SLOG << xplugin::xPluginInterface().getXServiceData("user_log.language_type", "language");
    });
}

void ShowWidget::on_pushButton_13_clicked()
{
    SLOG << xplugin::xPluginInterface().execCommandXServiceData("factory.get_flex_status", "");
}

void ShowWidget::on_pushButton_15_clicked()
{
    SLOG << xplugin::xPluginInterface().getRobotIP();
}

void ShowWidget::on_pushButton_16_clicked()
{
    SLOG << xplugin::xPluginInterface().getRobotType();
}

void ShowWidget::on_pushButton_17_clicked()
{
    SLOG << xplugin::xPluginInterface().getRobotModelType();
}

void ShowWidget::on_pushButton_14_clicked()
{
    SLOG << xplugin::xPluginInterface().getXServiceData("user_log.language_type", "language");
}

void ShowWidget::on_pushButton_20_clicked()
{
    xplugin::xPluginInterface().setXServiceData("project.setting", "max_manual_speed", 100);

    QJsonObject obj;
    obj.insert("tool", "tool0");
    obj.insert("wobj", "wobj0");
    xplugin::xPluginInterface().setXServiceData("move.moveto_select_info", obj);
}

void ShowWidget::on_pushButton_21_clicked()
{
    QJsonObject obj;
    obj.insert("target_language", "cn");
    xplugin::xPluginInterface().commandXServiceData("user_log.change_language", obj);
    SLOG << "commandXServiceData";
    QTimer::singleShot(3000, this, [](){
        SLOG << xplugin::xPluginInterface().getXServiceData("user_log.language_type", "language");
    });
}

void ShowWidget::on_pushButton_22_clicked()
{
    SLOG << xplugin::xPluginInterface().execCommandXServiceData("factory.get_flex_status", "");
}

void ShowWidget::on_pushButton_18_clicked()
{
    SLOG << xplugin::xPluginInterface().rcRequest("robot", "body.get_DH_params", QJsonValue());
    SLOG << xplugin::xPluginInterface().rcRequestFullCommand("robot.frame.get_current");
}

void ShowWidget::on_pushButton_19_clicked()
{
    SLOG << xplugin::xPluginInterface().rcExecRequest("robot", "body.get_DH_params", QJsonValue());
    SLOG << xplugin::xPluginInterface().rcExecRequestFullCommand("robot.frame.get_current");
}

void ShowWidget::on_pushButton_23_clicked()
{
    xplugin::xPluginInterface().disconnectXServiceData(PLUGIN_NAME, "project.tasks_pointer");
}

void ShowWidget::on_addPoint_clicked()
{
    QString pointName = ui->pointName->text();
    QString tool = ui->toolName->text();
    QString wobj = ui->wobjName->text();
    SLOG << xplugin::xPluginInterface().addCurrentPoint(pointName, true, tool, wobj);
}


void ShowWidget::on_movetostart_clicked()
{
    int code = 0;
    QString pointName = ui->pointName->text();
    xplugin::xPluginInterface().moveToStart(code, pointName, 50, true);
}


void ShowWidget::on_pushButton_24_clicked()
{
   xplugin::xPluginInterface().moveToStop();
}


void ShowWidget::on_pushButton_25_clicked()
{
    QString pointName = ui->pointName->text();
    QString tool = ui->toolName->text();
    QString wobj = ui->wobjName->text();
    SLOG << xplugin::xPluginInterface().addCurrentPoint(pointName, false, tool, wobj);
}

void ShowWidget::on_btnFullScreen_clicked()
{
    static bool isFull = true;
    xplugin::ChangeFullScreen(PLUGIN_NAME, isFull);
    isFull = !isFull;
}


void ShowWidget::on_btnModelWindow_clicked()
{
    static bool isModel = true;
    xplugin::ChangeModelWindowStatus(PLUGIN_NAME, isModel);
    isModel = !isModel;
}


void ShowWidget::on_btnChangePower_clicked()
{
    static int callBack = 0;
    bool status = xplugin::xPluginInterface().getRobotEleStatus();
    xplugin::xPluginInterface().changeRobotPowerStatue(!status);

    QString id = xplugin::xPluginEvent().subscribe(xplugin::XPluginEventType::RCPowerStatusChange,
                                      [this](const xplugin::XPluginEvent& eventData){

        callBack++;
        ELOG << "callBack :" << callBack;

        ui->labChangePower->setText(eventData.data().toString());

    }, PLUGIN_NAME, this);

    ELOG << "subscribe :" << id;
}


void ShowWidget::on_btnChangeModel_clicked()
{
    static int callBack = 0;
    bool val = xplugin::xPluginInterface().getRobotOperateIsHand();
    ELOG << "val :" << val;
    xplugin::xPluginInterface().changeRobotModelStatue(!val);
    QString id = xplugin::xPluginEvent().subscribe(xplugin::XPluginEventType::RCRunModelChange,
                                      [this](const xplugin::XPluginEvent& eventData){

        callBack++;
        ELOG << "callBack :" << callBack;

        ui->labChanegModel->setText(eventData.data().toString());
    }, PLUGIN_NAME, this);
    ELOG << "subscribe :" << id;
}

void ShowWidget::on_btnSubscribe_clicked()
{
    auto fun = [](const xplugin::XPluginEvent& evetn){
        qDebug() << "Custom event success!";
        qDebug() << "data : " << evetn.data();
        qDebug() << "source :" << evetn.source();
    };
    QString id = xplugin::xPluginEvent().subscribe(xplugin::XPluginEventType(xplugin::UserEventStart + 1), fun, PLUGIN_NAME, this);

}


void ShowWidget::on_btnPublish_clicked()
{
    xplugin::xPluginEvent().publish(xplugin::XPluginEventType(xplugin::UserEventStart + 1), "data", PLUGIN_NAME);
}


void ShowWidget::on_btnUnSubscribe_clicked()
{
    // 三种取消订阅
    // xplugin::xPluginEvent().unsubscribeByPlugin(PLUGIN_NAME);
    // xplugin::xPluginEvent().unsubscribeByTypeAndPlugin(PLUGIN_NAME, xplugin::XPluginEventType(xplugin::UserEventStart + 1));
    xplugin::xPluginEvent().unsubscribeByType(xplugin::XPluginEventType(xplugin::UserEventStart + 1));
}


void ShowWidget::on_btnBeforeInstall_clicked()
{
    auto beforeRunStart = [](const xplugin::XPluginEvent& evetn){
        ILOG << "xplugin::XPluginEventType::RCBeforeRunStart";
        // do your self

        // 满足条件在启动，否则直接返回就不启动
        if(1){

            QTimer::singleShot(1000, [](){
                xplugin::xPluginInterface().startRunning();
            });
        }
    };
    xplugin::xPluginEvent().subscribe(xplugin::XPluginEventType::RCBeforeRunStart, beforeRunStart, PLUGIN_NAME, this);

    auto beforeRunStop = [](const xplugin::XPluginEvent& evetn){
        ILOG << "xplugin::XPluginEventType::RCBeforeRunStop";
        // do your self

        // 满足条件在关闭，否则直接返回就不关闭
        if(1){

            QTimer::singleShot(1000, [](){
                xplugin::xPluginInterface().stopRunning();
            });
        }
    };
    xplugin::xPluginEvent().subscribe(xplugin::XPluginEventType::RCBeforeRunStop, beforeRunStop, PLUGIN_NAME, this);

    xplugin::xPluginInterface().startRunningFilter(true);
    xplugin::xPluginInterface().stopRunningFilter(true);
}

void ShowWidget::on_btnUnInstall_clicked()
{
    xplugin::xPluginInterface().startRunningFilter(false);
    xplugin::xPluginInterface().stopRunningFilter(false);
}

