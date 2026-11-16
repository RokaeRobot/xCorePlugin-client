/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RecipeSync.h
 * @brief 将客户端保存的完整配方集合同步到控制器。
 *
 * 重新连接或更换控制器后重新同步。RL 指令程序运行期间暂停同步。
 * 同步过程不写入锁丝机寄存器。
 */

#ifndef RECIPE_SYNC_H
#define RECIPE_SYNC_H

#include <QObject>
#include <QTimer>

#include "RecipeProtocol.h"

class RecipeSync : public QObject
{
    Q_OBJECT
public:
    explicit RecipeSync(QObject *parent);

    /**
     * @brief 立即将客户端已保存的完整配方集合同步到控制器。
     *
     * 手动同步会尝试重新发送完整集合。控制器校验全部配方后一次替换完整集合。
     * RL 指令程序运行、已有同步请求正在等待回复或连接发生变化时返回失败。
     * @param[out] error 失败原因，仅在返回 false 时使用。
     * @return 控制器已确认保存返回 true，否则返回 false。
     */
    bool synchronize(QString &error);

private:
    /**
     * @brief 每 2 秒检查是否需要同步本地保存的配方集合。
     *
     * 尚无本地保存的配置时跳过同步，当前连接已确认的版本无需重发。
     * 自动发送失败后，同一版本至少间隔 10 秒再重试。
     */
    void tick();

    /** @brief 发送完整配方集合，并检查控制器回复和连接状态。 */
    bool sendCollection(const QJsonObject &collection, QString &error);

    QTimer m_timer;                            // 配方同步检查定时器。
    bool m_busy = false;                       // 是否正在发送完整配方集合并等待确认。
    QString m_robot;                           // 当前控制器的编号和 IP 地址，用于识别连接变化。
    QString m_acknowledged;                   // 当前连接已经确认保存的完整配方内容标识。
    QString m_retryRevision;                  // 等待重试的完整配方内容标识。
    qint64 m_retryAt = 0;                      // 同一失败版本可重试的 Unix 时间，单位为毫秒。
    QString m_reportedError;                  // 最近记录的同步错误，避免重复输出日志。
    quint64 m_connectionGeneration = 0;        // 断线信号计数，用于丢弃旧连接的同步回复。
};

#endif // RECIPE_SYNC_H
