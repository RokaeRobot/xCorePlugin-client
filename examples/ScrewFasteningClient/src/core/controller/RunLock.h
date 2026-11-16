/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RunLock.h
 * @brief 程序运行期间的编辑和设备操作限制。
 */

#ifndef RUN_LOCK_H
#define RUN_LOCK_H

#include <QList>
#include <QObject>
#include <QPointer>

#include <functional>

namespace screw {

/**
 * @brief 管理程序运行期间的编辑和设备操作限制。
 *
 * 在界面线程查询、更新和订阅状态，订阅对象也在该线程销毁。
 */
class RunLock final
{
public:
    /** @brief 返回插件共用的运行锁，首次调用时创建，随插件模块销毁。 */
    static RunLock &instance();

    /** @brief 返回当前是否限制编辑和设备操作。 */
    bool isLocked() const;

    /** @brief 更新限制状态。仅当状态变化时同步通知订阅者。 */
    void setLocked(bool locked);

    /**
     * @brief 订阅限制状态变化，注册时立即通知当前状态。
     * @param[in] owner 订阅对象，销毁时自动移除该对象的订阅。
     * @param[in] apply 接收状态的回调，参数为 true 表示操作受限。
     */
    void subscribe(QObject *owner, const std::function<void(bool)> &apply);

private:
    Q_DISABLE_COPY(RunLock)

    struct Subscription
    {
        quint64 id;
        QPointer<QObject> owner;
        std::function<void(bool)> apply;
        QMetaObject::Connection cleanup;
    };

    RunLock() = default;
    ~RunLock();

    void removeSubscription(quint64 id);

    bool m_locked = false;
    QList<Subscription> m_subscriptions;
    quint64 m_nextSubscriptionId = 0;
};

} // namespace screw

#endif // RUN_LOCK_H
