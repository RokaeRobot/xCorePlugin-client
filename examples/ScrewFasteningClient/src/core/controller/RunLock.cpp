/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RunLock.cpp
 * @brief 保存运行锁状态并通知订阅者。
 */

#include "RunLock.h"

namespace screw {

RunLock &RunLock::instance()
{
    static RunLock lock;
    return lock;
}

bool RunLock::isLocked() const
{
    return m_locked;
}

void RunLock::setLocked(bool locked)
{
    if (m_locked == locked) {
        return;
    }

    // 先更新状态，供订阅回调在执行期间查询。
    m_locked = locked;

    // 回调可能注册或销毁订阅对象。遍历快照，通知前检查对象是否仍存活。
    const QList<Subscription> subscriptions = m_subscriptions;
    for (const Subscription &subscription : subscriptions) {
        if (subscription.owner) {
            subscription.apply(locked);
        }
    }
}

void RunLock::subscribe(QObject *owner, const std::function<void(bool)> &apply)
{
    if (owner == nullptr || !apply) {
        return;
    }

    // QPointer 可能在 destroyed 信号发出前清空，使用订阅编号定位待删除记录。
    const quint64 id = ++m_nextSubscriptionId;
    const auto cleanup = QObject::connect(owner, &QObject::destroyed, [this, id]() {
        removeSubscription(id);
    });
    m_subscriptions.append(Subscription{id, owner, apply, cleanup});
    apply(m_locked);
}

RunLock::~RunLock()
{
    // 订阅对象可能仍然存活。断开清理连接，避免其销毁时访问已释放的运行锁。
    for (const Subscription &subscription : m_subscriptions) {
        QObject::disconnect(subscription.cleanup);
    }
}

void RunLock::removeSubscription(quint64 id)
{
    for (int index = m_subscriptions.size() - 1; index >= 0; --index) {
        if (m_subscriptions.at(index).id == id) {
            m_subscriptions.removeAt(index);
            return;
        }
    }
}

} // namespace screw
