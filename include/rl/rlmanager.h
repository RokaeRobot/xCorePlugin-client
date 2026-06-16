#ifndef RLMANAGER_H
#define RLMANAGER_H
/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @ file rlmanager.h
 * @ brief Plugin rl manager！
 * @ version 1.0.0
 * @ author ROKAE Development Team
 *****************************************************************************/

#include <QString>
#include <QList>
#include <QObject>
#include "xplugin_global.h"

namespace xplugin
{
class RLContentBase;
class XPLUGIN_EXPORT RLManager : public QObject
{
    Q_OBJECT

public:
    RLManager(QObject * parent = nullptr);
    /**
     * @brief 创建指令组
     * @param lab 指令组的标签
     * @param groupName 指令组名称
     */
    void createGroups(const QString &lab, const QString &groupName);

    /**
     * @brief 快速创建一个普通指令
     * @param groupName 指令组名称
     * @param type 指令
     * @param desc 描述文本
     * @param func1 插入窗体
     * @param func2 属性窗体
     */
    void createData(const QString &groupName, const QString &type, const QString &desc, std::function<RLContentBase* ()> func1 = []{return nullptr;}, std::function<RLContentBase* ()> func2 = []{return nullptr;});

    /**
     * @brief 将指令添加到指令组中
     * @param groupName 指令组名称
     * @param type 指令
     */
    void createTypeToGroup(const QString &groupName, const QString &type);
    void createTypeToGroup(const QString &groupName, const QStringList &typeList);

    /**
     * @brief 创建一个指令名
     * @param type 指令名
     */
    void createTypeKey(const QString &type);

    /**
     * @brief 创建解释器文本
     * @param type 指令
     * @param str 解释器文本
     */
    void createSkeleton(const QString &type, const QString &str);

    /**
     * @brief 创建指令文本描述
     * @param type 指令
     * @param str 描述文本
     */
    void createInsDescribe(const QString &type, const QString &str);

    /**
     * @brief 创建默认输出文本
     * @param type 指令
     * @param strDef 输出文本
     */
    void createInsDefault(const QString &type, const QString &strDef);

    /**
     * @brief 创建正则匹配工具
     * @param type 指令
     * @param str 正则匹配工具
     */
    void createPattern(const QString &type, const QString &str);

    /**
     * @brief 创建插入窗体
     * @param type 指令
     * @param func 插入窗体
     */
    void createInsertWidget(const QString &type, std::function<RLContentBase* ()> func = []{return nullptr;});

    /**
     * @brief 创建属性窗体
     * @param type 指令
     * @param func 属性窗体
     */
    void createPropertyWidget(const QString &type, std::function<RLContentBase* ()> func = []{return nullptr;});

private:
    inline static QString PName(const QString& name) { return "p_" + name; }
    inline static QString CName(const QString& name) { return "c_" + name; }
    inline static QString NName(const QString& name) { return "n_" + name; }
    inline static QString SName(const QString& name) { return "s_" + name; }

};
}
#endif // RLMANAGER_H
