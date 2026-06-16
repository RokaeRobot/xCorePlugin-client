#ifndef XPLUGIN_EVENT_SYSTEM_H
#define XPLUGIN_EVENT_SYSTEM_H

#include <QObject>
#include <QList>
#include "xplugin_global.h"
#include "xpluginevent.h"

// 前向声明
namespace xplugin {

// 内部类前向声明
class XPluginEventHandler;
class XPluginEventSystemPrivate;

/**
 * @brief XPlugin 统一事件系统
 * 
 * 整合所有回调机制，提供类似 Qt 信号槽的发布-订阅模式。
 * 特点：
 * - 统一API：所有回调使用 subscribe/publish 接口
 * - 解耦设计：发布者和订阅者完全解耦
 * - 灵活扩展：支持运行时动态注册新事件类型
 * - 生命周期管理：基于 QObject 的自动清理
 * - 向后兼容：提供兼容层保留现有API
 */
class XPLUGIN_EXPORT XPluginEventSystem : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取事件系统单例实例
     */
    static XPluginEventSystem* instance();
    static void destroyInstance();

    // ==================== 订阅 API ====================
    
    /**
     * @brief 订阅事件
     * @param type 事件类型
     * @param handler 处理函数
     * @param pluginName 插件名称（用于标识订阅者）
     * @param context QObject 上下文，销毁时自动取消订阅（可选）
     * @return 订阅ID，用于后续取消订阅
     */
    QString subscribe(XPluginEventType type,
                      std::function<void(const XPluginEvent&)> handler,
                      const QString& pluginName,
                      QObject* context = nullptr);
    
    /**
     * @brief 带过滤器的订阅
     * @param type 事件类型
     * @param filter 过滤函数，返回 true 才执行 handler
     * @param handler 处理函数
     * @param pluginName 插件名称
     * @param context QObject 上下文（可选）
     * @return 订阅ID
     */
    QString subscribeFiltered(XPluginEventType type,
                              std::function<bool(const XPluginEvent&)> filter,
                              std::function<void(const XPluginEvent&)> handler,
                              const QString& pluginName,
                              QObject* context = nullptr);
    
    /**
     * @brief 一次性订阅（只处理一次后自动取消）
     * @param type 事件类型
     * @param handler 处理函数
     * @param pluginName 插件名称
     * @param context QObject 上下文（可选）
     * @return 订阅ID
     */
    QString subscribeOnce(XPluginEventType type,
                          std::function<void(const XPluginEvent&)> handler,
                          const QString& pluginName,
                          QObject* context = nullptr);
    
    /**
     * @brief 取消指定订阅
     * @param subscriptionId 订阅ID
     * @return 是否成功取消
     */
    bool unsubscribe(const QString& subscriptionId);
    
    /**
     * @brief 取消指定插件的所有订阅
     * @param pluginName 插件名称
     * @return 取消的订阅数量
     */
    int unsubscribeByPlugin(const QString& pluginName);
    
    /**
     * @brief 取消指定事件类型的所有订阅
     * @param eventType 事件类型
     * @return 取消的订阅数量
     */
    int unsubscribeByType(XPluginEventType eventType);

    /**
     * @brief 取消指定插件的指定事件类型的所有订阅
     * @param pluginName 插件名称 eventType 事件类型
     * @return 取消的订阅数量
     */
    int unsubscribeByTypeAndPlugin(const QString& pluginName, XPluginEventType eventType);

    // ==================== 发布 API ====================
    
    /**
     * @brief 发布事件
     * @param event 事件对象
     */
    void publish(const XPluginEvent& event);
    
    /**
     * @brief 发布事件（便捷方法）
     * @param type 事件类型
     * @param data 事件数据
     * @param source 事件源标识
     */
    void publish(XPluginEventType type, 
                 const QVariant& data = QVariant(),
                 const QString& source = QString());
    
    /**
     * @brief 异步发布事件（线程安全）
     * @param event 事件对象
     */
    void publishAsync(const XPluginEvent& event);
    
    /**
     * @brief 异步发布事件（便捷方法）
     * @param type 事件类型
     * @param data 事件数据
     * @param source 事件源标识
     */
    void publishAsync(XPluginEventType type,
                      const QVariant& data = QVariant(),
                      const QString& source = QString());

    /**
     * @brief 注册所有事件监听
     */
    void registerAllEvents();
    
    // ==================== 自定义事件类型 API ====================
    
    /**
     * @brief 注册自定义事件类型
     * @param name 事件类型名称（用于调试）
     * @return 分配的事件类型值，如果超出范围返回 None
     */
    XPluginEventType registerCustomEventType(const QString& name);
    
    /**
     * @brief 获取自定义事件类型名称
     * @param type 事件类型
     * @return 类型名称，如果不是自定义类型返回空字符串
     */
    QString customEventTypeName(XPluginEventType type) const;

    // ==================== 查询 API ====================
    
    /**
     * @brief 获取指定事件类型的订阅数量
     */
    int subscriptionCount(XPluginEventType type) const;
    
    /**
     * @brief 获取指定插件的订阅数量
     */
    int subscriptionCount(const QString& pluginName) const;
    
    /**
     * @brief 获取总订阅数量
     */
    int totalSubscriptionCount() const;
    
    /**
     * @brief 检查是否存在指定订阅
     */
    bool hasSubscription(const QString& subscriptionId) const;
    
    /**
     * @brief 获取已注册的所有事件类型
     */
    QList<XPluginEventType> registeredEventTypes() const;

    /**
     * @brief 获取已注册的目标插件的所有事件id
     */
    QList<QString> totalSubscriptionIds(const QString &name) const;
    
    /**
     * @brief 清理所有已失效的订阅
     * @return 清理的订阅数量
     */
    int cleanupInvalidSubscriptions();

    // ==================== 统计信息 ====================
    
    /**
     * @brief 获取已发布的事件总数
     */
    quint64 totalEventsPublished() const;
    
    /**
     * @brief 获取已处理的事件总数
     */
    quint64 totalEventsHandled() const;
    
    /**
     * @brief 重置统计信息
     */
    void resetStatistics();

signals:
    /**
     * @brief 事件发布信号（用于调试和监控）
     */
    void eventPublished(const XPluginEvent& event);
    
    /**
     * @brief 订阅添加信号
     */
    void subscriptionAdded(const QString& subscriptionId, XPluginEventType type, const QString& pluginName);
    
    /**
     * @brief 订阅移除信号
     */
    void subscriptionRemoved(const QString& subscriptionId, XPluginEventType type, const QString& pluginName);

protected:
    explicit XPluginEventSystem(QObject* parent = nullptr);
    ~XPluginEventSystem() override;

private slots:
    void onContextDestroyed(QObject* obj);

private:
    QString doSubscribe(XPluginEventType type,
                        QSharedPointer<XPluginEventHandler> handler,
                        const QString& pluginName,
                        QObject* context);

    void dispatchEvent(const XPluginEvent& event);


    bool doUnsubscribe(const QString& subscriptionId);
    bool doUnsubscribe(const QString& subscriptionId,
                        XPluginEventType& outType,
                        QString& outPluginName);

private:
    // PIMPL 指针 - 所有实现细节隐藏在私有类中
    QScopedPointer<XPluginEventSystemPrivate> d;
    static XPluginEventSystem* s_instance;
};


XPluginEventSystem XPLUGIN_EXPORT &xPluginEvent();

} // namespace xplugin

#endif // XPLUGIN_EVENT_SYSTEM_H
