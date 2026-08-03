#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @ file interfacemanager.h
 * @ brief Plugin interface management！
 * @ version 1.0.0
 * @ author ROKAE Development Team
 *****************************************************************************/

#include <QObject>
#include "plugincommon.h"
#include "xplugin_global.h"
#include <functional>
#include <QJsonValue>

namespace xplugin {
struct PhysicalButton{
    QString text;
    QString touched_icon;
    QString untouched_icon;
    std::function<void (bool is_pressed)> exec_function;
};

class XPLUGIN_EXPORT InterfaceManager : public QObject
{
    Q_OBJECT

protected:
    class InterfaceManagerPrivate;
    InterfaceManagerPrivate *p;

signals:
    // Connect连接相应
    void sigNetworkConnect();
    // DisConnect连接相应
    void sigNetworkDisConnect(bool status);

public:
    explicit InterfaceManager(QObject *parnet = nullptr);
    ~InterfaceManager() {}

    // 初始化事件系统
    void initEventSystem();

    // 通用数据接口 自定义插件和控制器插件通信协议
    // QJsonArray[PluginName:{key:value}...]
    QJsonArray commandCustomData(QObject *obj, const QString &pluginName, const QString &key, const QString &val);
    QJsonArray commandCustomData(QObject *obj, const QString &pluginName, const QJsonObject& json);
    QJsonArray commandCustomData(QObject *obj, const QJsonObject& json);
    QJsonArray commandCustomData(QObject *obj, const QJsonArray& arr);

    // 通用数据接口 获取数据
    QJsonValue getNodeData(const QString &key);

    // 通用数据接口 get方法获取xservice数据
    QJsonValue getXServiceData(const QString &key);
    QJsonValue getXServiceData(const QString &key, const QString &subKey);

    // 通用数据接口 set方法设置xservice数据
    bool setXServiceData(const QString &key, const QVariant &var);
    bool setXServiceData(const QString &key, const QString &subKey, const QVariant &var);

    // 通用数据接口 异步command方法设置xservice数据
    bool commandXServiceData(const QString &key, const QVariant &var);

    // 通用数据接口 同步command方法设置xservice数据
    QJsonValue execCommandXServiceData(const QString &key, const QVariant &var);

    bool rcRequest(const QString &module, const QString &command, const QJsonValue &data = QJsonValue(), int timeout = 4800);
    bool rcRequestFullCommand(const QString &full_command, const QJsonValue &request_val = QJsonValue(), int timeout = 4800);

    QJsonValue rcExecRequest(const QString &module, const QString &command, const QJsonValue &data = QJsonValue(), bool has_log = true, int timeout = 1600);
    QJsonValue rcExecRequestFullCommand(const QString &full_command, const QJsonValue &request_val = QJsonValue(), bool has_log = true, int timeout = 1600);

    // 通用数据接口 注册/取消xservice回调数据  // 跨dll边界。指定编译器
    // 由 插件名_Key 做唯一标识！
    bool connectXServiceData(const QString &pluginName, const QString &serviceKey, std::function<void(const QVariant &data)> callBack, const QString& key = "");
    bool disconnectXServiceData(const QString &pluginName, const QString &serviceKey, const QString &key = "");

    // 网络连接接口 注册网络连接成功/取消后的动作  // 跨dll边界。指定编译器
    QString connectNetworkStatus(bool isConnect, const QString &pluginName, QObject *obj, std::function<void()> callBack, const QString &id = "");
    void disconnectNetworkStatus(bool isConnect, const QString &pluginName, const QString &id = "");

    /********************************HMI与控制器配置相关*******************************************/
    // 获取网络状态
    bool getRobotConnected();

    // 获取机器人的ID
    QString getRobotID();

    // 获取机器人上电状态
    bool getRobotEleStatus();

    // 获取机器人操作状态  手动-自动
    bool getRobotOperateIsHand();

    // 获取机器人运行状态
    QString getRobotPhyStatus();

    // 获取当前用户权限级别
    int getUserOperateLevel();

    // 获取正在连接的控制器IP
    QString getRobotIP();

    // 获取机器人型号
    QString getRobotType();

    // 获取机器人构型，机器人设备枚举类型
    int getRobotModelType();

    // 设置机器人上电状态
    bool changeRobotPowerStatue(bool status, bool isRsync = true);

    // 设置机器人手/自动状态
    bool changeRobotModelStatue(bool isHand, bool isRsync = true);
    /*********************************运动指令********************************************/
    // 安装运动启动过滤器 设置后需要手动启动
    bool startRunningFilter(bool doOpen);
    // 安装运动停止过滤器 设置后需要手动结束
    bool stopRunningFilter(bool doOpen);

    // 当前程序运行启动
    bool startRunning();
    // 当前程序运行停止
    bool stopRunning();


    /*********************************HMI数据********************************************/
    // 获取当前工具
    QString getCurTool();

    // 获取当前工件
    QString getCurWobj();

    // 获取工具列表
    QStringList getToolList();

    // 获取工件列表
    QStringList getWobjList();

    // 获取速度列表
    QStringList getSpeedList();
    // 获取转弯区列表
    QStringList getZoneList();

    // 由变量类型获取变量名称列表
    QStringList getVarDataList(const QString &type = "");

    // 由变量类型和值创建基本变量
    bool addVarData(const QString &name, QVariant value, bool isPers = false);

    // 由变量名删除基本变量(不支持 socketserver socketconn serials)
    bool delVarData(const QString &name);
    bool delVarData(const QList<QString> &names);

    // 由变量类型名更新基本变量
    bool updateVarData(const QString &name, QVariant value);

    // 获取变量的结构
    QJsonObject getVarObjDataByName(const QString &name);

    // 读取pers变量
    QVector<QString> getValuesOfPERSVars(const QVector<QString>& names);

    // 设置pers变量的值
    int setValuesOfPERSVars(const QVector<QString> &names, const QVector<QVariant> &values);

    // 注册需要持续获取的变量值
    int registPERSVars(const QVector<QString> &names);
    // 获取注册到的变量值
    QVector<QVariant> getRegistedPERSVars();

    // 获取点位名称列表
    QStringList getPointNameList();
    // 获取点位结构数据
    QJsonObject getPointObjDataByName(const QString &pointName);
    // 获取点位数据
    QVector<double> getPointDataByName(const QString &pointName);
    // 获取当前点位数据
    QJsonValue getCurrentPointData(const QString& toolName = "", const QString& wobjName = "");
    // 由当前点新建点位变量 (关节-笛卡尔)
    bool addCurrentPoint(const QString& pointName, bool isJoint,  const QString& toolName, const QString& wobjName);

    // 获取当前工程名
    QString getCurProject();
    // 获取工程列表  workspace/<robot_id>/names
    QStringList getProjectList();
    // 创建一个默认工程
    int createProject(const QString& projectName, const QString &desc = "", bool isUsed = false);
    // 切换工程
    int changeProject(const QString& projectName);
    // 删除工程
    int removeProject(const QString& projectName);

    // 点位可达性校验
    int checkPointMoveTo(const QString &pointFrom, const QString &pointTo);
    // 点位一致性校验
    int checkPointSame(const QString &pointFrom, const QString &pointTo);

    // 运动至
    void moveToStart(int &errCode, const QString &pointNameTo, int speed, bool isMoveJ);
    void moveToStart(int &errCode, const QString &pointNameTo, int speed, bool confCheck, bool isMoveJ);
    void moveToStop();

    // 修改笛卡尔下的点位值
    void changeCartPoint(const QString& pointName, const QVector<double>& xyz_abc);

    // 获取所有的IO名称列表以及值
    QHash<QString, QVariant> getIOHash();

    // 获取信号类型对应的IO信号名  "DI","DO","GI","GO","AI","AO"
    QStringList getIOByType(const QString &sigType);
    // 获取信号类型对应的IO信号名  "DI","DO","GI","GO","AI","AO"
    QStringList getIOByType(int sigType);

    // 设置IO值，由IO类型和IO名称
    bool setIOValue(const QString &name, int sigType, bool value);
    bool setIOValue(const QString &name, const QString &sigType, bool value);
    bool getIOValue(const QString &name);

    // 获取所有的寄存器名称列表以及值
    QHash<QString, QStringList> getRegisterHash();

    // 获取所有的寄存器值 返回值类型 ：Arr
    QJsonValue getRegisterValue();
};

// 数据获取入口
InterfaceManager XPLUGIN_EXPORT &xPluginInterface();

// 获取插件版本
QString XPLUGIN_EXPORT GetPluginVersion();
// 创建中心窗体
int XPLUGIN_EXPORT CreateCenterWidget(const QString &pluginName, QWidget *widget);
// 创建状态栏
int XPLUGIN_EXPORT CreateStatusBar(const QString &pluginName, QWidget *widget, int index = 1);
// 创建常驻窗体
int XPLUGIN_EXPORT CreateRightExpandWidgte(const QString &pluginName, QWidget *widget, const QString &name = "");
// 切换常驻窗体 全空时关闭扩展窗体
int XPLUGIN_EXPORT SwitchRightExpandWidgte(const QString &pluginName = "", const QString &name = "");
// 创建物理按键
int XPLUGIN_EXPORT CreatePhyButtonList(const QString &pluginName, const QString& type, const QList<PhysicalButton>& left_buttons,
                                       const QList<PhysicalButton>& right_buttons, QStringList center_texts = QStringList(""));
// 启用/关闭物理按键
int XPLUGIN_EXPORT ChangePhyButtonStatus(const QString &pluginName, const QString& type, bool enable);

// 启用/关闭模态状态 - 控制物理按键是否响应
int XPLUGIN_EXPORT ChangeModelWindowStatus(const QString &pluginName, bool status);

// 启用/关闭全屏显示
int XPLUGIN_EXPORT ChangeFullScreen(const QString &pluginName, bool isFull);

// 获取右侧扩展窗口
int XPLUGIN_EXPORT GetRightExpandSideBar(QWidget **out_right_expand_side_widget);

// 抛出日志
int XPLUGIN_EXPORT ThrowLog(const QString &pluginName, enThrowLogLevel level, const QString &msg);

// 获取插件数据路径
int XPLUGIN_EXPORT GetPluginDataPath(QString &dataPath, const QString &pluginName);
}

#endif // INTERFACEMANAGER_H
