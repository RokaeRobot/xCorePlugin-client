#ifndef XPLUGIN_EVENT_H
#define XPLUGIN_EVENT_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QHash>
#include <QUuid>
#include "xplugin_global.h"

namespace xplugin {

/**
 * @brief XPlugin 事件类型枚举
 *
 * 事件类型分为以下几个区间：
 * - 0x0000 - 0x0FFF: 系统事件 (核心框架事件)
 * - 0x1000 - 0x1FFF: 硬件事件 (物理按钮、IO等)
 * - 0x2000 - 0x2FFF: 网络事件 (连接状态、数据接收等)
 * - 0x3000 - 0x3FFF: XService 事件 (数据变化等)
 * - 0x4000 - 0x4FFF: RC 事件 (数据变化等)
 * - 0x5000 - 0x5FFF: UI (数据变化等)
 * - 0x8000 - 0xFFFF: 用户自定义事件 (插件可动态注册)
 */
enum XPluginEventType {
    // ==================== 系统事件 (0x0000 - 0x0FFF) ====================
    SystemEventStart    = 0x0000,
    None                = 0x0000,   // 无事件/空事件

    // ==================== 硬件事件 (0x1000 - 0x1FFF) ====================
    HardEventStart      = 0x1000,

    // ==================== 网络事件 (0x2000 - 0x2FFF) ====================
    NetworkEventStart   = 0x2000,
    NetworkConnected    = 0x2001,   // 网络已连接
    NetworkDisconnected = 0x2002,   // 网络已断开
    NetworkError        = 0x2003,   // 网络错误

    // ==================== XService 事件 (0x3000 - 0x3FFF) ====================
    XServiceEventStart  = 0x3000,
    XServiceDataChange  = 0x3001,   // XService 节点数据变化

    // ==================== RC事件 (0x4000 - 0x4FFF) ====================
    RCEventStart        = 0x4000,
    RCPowerStatusChange = 0x4001,   // 上电状态变化
    RCRunModelChange    = 0x4011,   // 手自动模式切换
    RCRunStart          = 0x4021,   // 程序启动运行信号
    RCRunStop           = 0x4022,   // 程序停止运行信号
    RCBeforeRunStart    = 0x4023,   // 程序启动运行前信号
    RCBeforeRunStop     = 0x4024,   // 程序停止运行前信号
    RCNotifyDataChange  = 0x4031,   // RC notify 数据变化（通用）


    // ==================== UI 事件 (0x5000 - 0x5FFF) ====================
    UIEventStart        = 0x5000,

    // ==================== 用户自定义事件起始 (0x8000) ====================
    UserEventStart      = 0x8000,

    // ==================== 终止事件符 (0xFFFF) ====================
    MaxEventType        = 0xFFFF
};
/**
 * @brief 获取事件类型的字符串名称
 * @param type 事件类型
 * @return 事件类型名称
 */
XPLUGIN_EXPORT QString eventTypeToString(XPluginEventType type);

/**
 * @brief 判断事件类型是否有效
 * @param type 事件类型
 * @return 是否有效
 */
inline bool isValidEventType(XPluginEventType type) {
    return static_cast<quint32>(type) <= static_cast<quint32>(XPluginEventType::MaxEventType);
}

// 给枚举提供哈希值
inline uint qHash(xplugin::XPluginEventType key, uint seed = 0) noexcept
{
    return ::qHash(static_cast<uint>(key), seed);
}

/**
 * @brief 判断是否为系统事件
 * @param type 事件类型
 * @return 是否为系统事件
 */
inline bool isSystemEvent(XPluginEventType type) {
    auto val = static_cast<quint32>(type);
    return val >= 0x0000 && val < 0x1000;
}

/**
 * @brief 判断是否为用户自定义事件
 * @param type 事件类型
 * @return 是否为用户自定义事件
 */
inline bool isUserEvent(XPluginEventType type) {
    auto val = static_cast<quint32>(type);
    return val >= 0x8000 && val <= 0xFFFF;
}

/**
 * @brief 判断是否为XService相关事件
 * @param type 事件类型
 * @return 是否为XService事件
 */
inline bool isXServiceEvent(XPluginEventType type) {
    auto val = static_cast<quint32>(type);
    return val >= 0x3000 && val < 0x4000;
}

/**
 * @brief XPlugin 事件数据类
 * 
 * 封装了事件的完整信息，包括类型、来源、数据、时间戳等
 * 支持动态属性扩展，可用于传递任意附加信息
 */
class XPLUGIN_EXPORT XPluginEvent
{
public:
    /**
     * @brief 默认构造函数
     */
    XPluginEvent();
    
    /**
     * @brief 完整构造函数
     * @param type 事件类型
     * @param source 事件源标识（如插件名、节点路径等）
     * @param data 事件数据
     */
    XPluginEvent(XPluginEventType type,
                 const QString& source = QString(),
                 const QVariant& data = QVariant());
    
    /**
     * @brief 拷贝构造函数
     */
    XPluginEvent(const XPluginEvent& other);
    
    /**
     * @brief 移动构造函数
     */
    XPluginEvent(XPluginEvent&& other) noexcept;
    
    /**
     * @brief 析构函数
     */
    ~XPluginEvent();
    
    /**
     * @brief 拷贝赋值运算符
     */
    XPluginEvent& operator=(const XPluginEvent& other);
    
    /**
     * @brief 移动赋值运算符
     */
    XPluginEvent& operator=(XPluginEvent&& other) noexcept;
    
    // ==================== 属性访问 ====================
    
    /**
     * @brief 获取事件类型
     */
    XPluginEventType type() const { return m_type; }
    
    /**
     * @brief 设置事件类型
     */
    void setType(XPluginEventType type) { m_type = type; }
    
    /**
     * @brief 获取事件源标识
     * @return 事件源，如插件名、XService节点路径等
     */
    QString source() const { return m_source; }
    
    /**
     * @brief 设置事件源标识
     */
    void setSource(const QString& source) { m_source = source; }
    
    /**
     * @brief 获取事件数据
     */
    QVariant data() const { return m_data; }
    
    /**
     * @brief 设置事件数据
     */
    void setData(const QVariant& data) { m_data = data; }
    
    /**
     * @brief 获取事件时间戳
     */
    QDateTime timestamp() const { return m_timestamp; }
    
    /**
     * @brief 获取事件序列号（全局唯一递增）
     */
    quint64 sequenceId() const { return m_sequenceId; }
    
    /**
     * @brief 获取事件唯一标识
     */
    QString eventId() const { return m_eventId; }
    
    // ==================== 动态属性 ====================
    
    /**
     * @brief 设置动态属性
     * @param key 属性名
     * @param value 属性值
     */
    void setProperty(const QString& key, const QVariant& value);
    
    /**
     * @brief 获取动态属性
     * @param key 属性名
     * @param defaultValue 默认值
     * @return 属性值，如果不存在则返回默认值
     */
    QVariant property(const QString& key, const QVariant& defaultValue = QVariant()) const;
    
    /**
     * @brief 检查是否存在指定属性
     */
    bool hasProperty(const QString& key) const;
    
    /**
     * @brief 移除动态属性
     */
    void removeProperty(const QString& key);
    
    /**
     * @brief 获取所有动态属性名
     */
    QStringList propertyNames() const;
    
    // ==================== 便捷方法 ====================
    
    /**
     * @brief 转换为字符串表示（用于调试）
     */
    QString toString() const;
    
    /**
     * @brief 判断事件是否为特定类型
     */
    bool isType(XPluginEventType type) const { return m_type == type; }
    
    /**
     * @brief 判断事件数据是否为空
     */
    bool isEmpty() const { return !m_data.isValid() || m_data.isNull(); }
    
    /**
     * @brief 获取数据并转换为指定类型
     * @tparam T 目标类型
     * @return 转换后的值，如果失败返回默认构造
     */
    template<typename T>
    T dataAs() const {
        if (m_data.canConvert<T>()) {
            return m_data.value<T>();
        }
        return T{};
    }

private:
    static quint64 generateSequenceId();
    
    XPluginEventType m_type = XPluginEventType::None;
    QString m_source;
    QVariant m_data;
    QDateTime m_timestamp;
    quint64 m_sequenceId = 0;
    QString m_eventId;
    
    QHash<QString, QVariant> m_properties;
};

} // namespace xplugin

Q_DECLARE_METATYPE(xplugin::XPluginEvent)
Q_DECLARE_METATYPE(xplugin::XPluginEventType)

#endif // XPLUGIN_EVENT_H
