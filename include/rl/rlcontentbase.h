#ifndef RLCONTENTBASE_H
#define RLCONTENTBASE_H

/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @ file rlcontentnase.h
 * @ brief Plugin rl base widget
 * @ version 1.0.0
 * @ author ROKAE Development Team
 *****************************************************************************/

#include <QObject>
#include <QWidget>
#include "xplugin_global.h"

namespace xplugin {
class XPLUGIN_EXPORT RLContentBase : public QWidget
{
    Q_OBJECT

public:
    explicit RLContentBase(QWidget *parent = nullptr);
    virtual ~RLContentBase();

    /**
     * @brief 设置RL指令类型
     * @param type 类型
     */
    void setType(const QString& type) { m_type = type; }
    const QString &getType() { return m_type; }

    /**
     * @brief 填充当前窗口的字符串
     * @param str 填充字符串
     */
    virtual void FillWith(const QString &str);

    /**
     * @brief 当前窗口对外输出的字符串
     * @return QString 输出字符串
     */
    virtual QString ToString();

    /**
     * @brief 判断是否作为对外输出窗口的插入值
     * @return bool 判断结果
     */
    virtual bool CheckInsert();

    /**
     * @brief 判断是否作为最对内填充窗口的值
     * @return bool 判断结果
     */
    virtual bool CheckReplace();
protected:
    QString m_type;
};
}

#endif // RLCONTENTBASE_H
