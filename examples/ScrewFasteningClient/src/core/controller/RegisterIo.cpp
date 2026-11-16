/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RegisterIo.cpp
 * @brief 客户端的寄存器读写。
 *
 * 供参数下发和反馈页面共用，统一处理数值转换及实时刷新。
 */

#include "RegisterIo.h"
#include "RunLock.h"

#include <QEvent>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QPointer>
#include <QScopedValueRollback>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#include "interface/interfacemanager.h"

namespace {
using RegisterValues = QMap<int, QString>;

QHash<QString, RegisterValues> g_liveValues; // 按寄存器名称和元素序号保存实时值。
bool g_reading = false;               // 是否正在读取寄存器，避免重复发送读取请求。
QString g_liveRobot;                  // 最近一次读取的控制器标识。
quint64 g_connectionGeneration = 0;    // 连接断开后使正在返回的旧回复失效。

// 保留数组元素的原始序号，缺失元素不会使后续角度和扭矩错位。
RegisterValues valueElements(const QJsonObject &object)
{
    RegisterValues elements;
    const QJsonValue value = object.value(QStringLiteral("value"));
    if (value.isObject()) {
        const QJsonObject values = value.toObject();
        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            if (!it.key().startsWith(QLatin1Char('#')))
                continue;
            bool ok = false;
            const int index = it.key().mid(1).toInt(&ok);
            if (ok && index >= 0)
                elements.insert(index, it.value().toVariant().toString());
        }
    } else if (value.isArray()) {
        const QJsonArray values = value.toArray();
        for (int index = 0; index < values.size(); ++index)
            elements.insert(index, values.at(index).toVariant().toString());
    } else if (!value.isUndefined() && !value.isNull()) {
        elements.insert(0, value.toVariant().toString());
    }
    if (!elements.contains(0) && object.contains(QStringLiteral("value_single"))) {
        elements.insert(0, object.value(QStringLiteral("value_single")).toVariant().toString());
    }
    return elements;
}

// 实时显示只使用读取结果，不从通信配置列表补入旧数值。
RegisterValues liveElements(const QString &name)
{
    return g_liveValues.value(name);
}

// 来源改变后重新收集名称，避免空选择触发整组读取。
QStringList registerNames(const QStringList &sources)
{
    QStringList names;
    for (const QString &source : sources) {
        const QString name = source.trimmed();
        if (!name.isEmpty() && !names.contains(name))
            names.append(name);
    }
    return names;
}

class LiveRefresh : public QObject
{
public:
    /**
     * @brief 为页面绑定可见性驱动的轮询，使用页面作为生命周期上下文。
     * @param[in] page 需要刷新数值的页面指针，同时作为父对象。
     * @param[in] sources 返回当前页面所需寄存器名称的函数。
     * @param[in] apply 读取成功后更新页面显示的函数。
     * @param[in] clear 进入页面、断线或读取失败时调用的函数。
     * @param[in] onRead 页面自行处理读取结果的函数，允许为空。
     */
    LiveRefresh(QWidget *page, const std::function<QStringList()> &sources,
                const std::function<void()> &apply, const std::function<void()> &clear,
                const std::function<void(RegisterIo::ReadResult)> &onRead = {})
        : QObject(page), m_sources(sources), m_apply(apply), m_clear(clear), m_onRead(onRead)
    {
        m_timer.setInterval(1000);
        connect(&m_timer, &QTimer::timeout, this, [this]() { poll(); });
        page->installEventFilter(this);

        // 断线后清除旧连接的数据，并丢弃尚未发出的页面请求。
        connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect,
                this, [this](bool) {
            ++g_connectionGeneration;
            g_liveValues.clear();
            ++m_epoch;
            m_busy = false;
            if (m_onRead)
                m_onRead(RegisterIo::ReadResult::Disconnected);
            else if (m_clear)
                m_clear();
        });
    }

protected:
    /**
     * @brief 显示时启动轮询，隐藏时使当前批次失效并停止后续请求。
     * @param[in] watched 被监视的对象指针。
     * @param[in] event 事件指针。
     * @return 是否停止事件继续传递。
     */
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == parent()) {
            if (event->type() == QEvent::Show) {
                ++m_epoch;
                m_busy = false;
                // 配置恢复完成后，只读取当前页面实际选择的数据来源。
                const QPointer<LiveRefresh> guard(this);
                if (m_clear)
                    m_clear();
                if (!guard)
                    return true;
                m_timer.start();
                QTimer::singleShot(0, this, [this]() {
                    if (static_cast<QWidget *>(parent())->isVisible())
                        poll();
                });
            } else if (event->type() == QEvent::Hide) {
                m_timer.stop();
                ++m_epoch;
                m_busy = false;
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    /**
     * @brief 收集当前来源并启动一轮读取，已有批次未结束时跳过。
     */
    void poll()
    {
        if (m_busy || !static_cast<QWidget *>(parent())->isVisible()) {
            return;
        }

        m_busy = true;
        m_names = registerNames(m_sources());
        m_next = 0;
        readNext(m_epoch);
    }

    // 同步请求可能触发事件循环；请求前后都校验批次、可见性和来源，避免应用过期回复。
    void readNext(quint64 epoch)
    {
        if (epoch != m_epoch || !static_cast<QWidget *>(parent())->isVisible())
            return;
        if (registerNames(m_sources()) != m_names) {
            // 用户已修改来源，旧批次不再继续；下一次轮询读取新来源。
            m_busy = false;
            return;
        }
        if (m_next == m_names.size()) {
            finish(RegisterIo::ReadResult::Success);
            return;
        }

        const QPointer<LiveRefresh> guard(this);
        const RegisterIo::ReadResult result = RegisterIo::readValues({m_names.at(m_next)});
        if (!guard || epoch != m_epoch)
            return;
        if (!static_cast<QWidget *>(parent())->isVisible() || registerNames(m_sources()) != m_names) {
            m_busy = false;
            return;
        }
        if (result != RegisterIo::ReadResult::Success) {
            finish(result);
            return;
        }
        ++m_next;
        // 每个名称只读取一次；请求之间归还事件循环，切页后不再发送旧页剩余请求。
        QTimer::singleShot(0, this, [this, epoch]() { readNext(epoch); });
    }

    void finish(RegisterIo::ReadResult result)
    {
        m_busy = false;
        if (result == RegisterIo::ReadResult::Busy) {
            return;
        }
        if (m_onRead) {
            m_onRead(result);
        } else if (result == RegisterIo::ReadResult::Success) {
            if (m_apply) {
                m_apply();
            }
        } else if (m_clear) {
            m_clear();
        }
    }

    QTimer m_timer;                // 页面数值刷新定时器。
    std::function<QStringList()> m_sources; // 当前页面的寄存器来源。
    QStringList m_names;           // 本轮去重后的来源，空名称不会发送请求。
    int m_next = 0;                // 下一项待读取的来源。
    quint64 m_epoch = 0;           // 页面隐藏或断线后使旧批次失效。
    std::function<void()> m_apply; // 读取成功后更新页面显示的函数。
    std::function<void()> m_clear; // 进入页面、断线或读取失败时调用的函数。
    std::function<void(RegisterIo::ReadResult)> m_onRead; // 页面自行处理读取结果的函数。
    bool m_busy = false;           // 是否正在完成一次读取和页面更新。
};

} // namespace

bool RegisterIo::writeValue(const QString &name, const QString &value)
{
    if (screw::RunLock::instance().isLocked()) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          QObject::tr("程序运行中，暂不能手动写入寄存器。请停止程序后再操作。"));
        return false;
    }
    const QString registerName = name.trimmed();
    const QString text = value.trimmed();
    if (registerName.isEmpty() || text.isEmpty()) {
        return false;
    }

    // 根据页面选择的寄存器名称查找类型和位偏移，写入目标必须已在HMI的通信页面配置。
    QString type = QStringLiteral("int16");
    int bias = 0;
    bool found = false;
    const QJsonArray list = xplugin::xPluginInterface().getRegisterValue().toArray();
    for (const QJsonValue &item : list) {
        const QJsonObject object = item.toObject();
        if (object.value(QStringLiteral("name")).toString() != registerName) {
            continue;
        }
        found = true;
        type = object.value(QStringLiteral("type")).toString(type);
        if (type == QLatin1String("bit") && object.value(QStringLiteral("len")).toInt() == 1) {
            const int bitBias = object.value(QStringLiteral("bit_bias")).toInt();
            if (bitBias != 0) {
                bias = bitBias - 1;
            }
        }
        break;
    }
    if (!found) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          QObject::tr("未找到寄存器：%1").arg(registerName));
        return false;
    }

    // 按寄存器类型转换页面中的数值文本，转换失败时不发送设备写入请求。
    bool ok = false;
    QJsonValue numeric;
    if (type == QLatin1String("float")) {
        numeric = text.toFloat(&ok);
    } else if (type == QLatin1String("int32")) {
        numeric = text.toInt(&ok);
    } else {
        numeric = text.toShort(&ok);
    }
    if (!ok) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          QObject::tr("请输入正确的修改值"));
        return false;
    }

    // 组合单次寄存器写入请求，连接有效时才发送。
    QJsonObject data;
    data.insert(QStringLiteral("name"), registerName);
    data.insert(QStringLiteral("type"), type);
    data.insert(QStringLiteral("bias"), bias);
    data.insert(QStringLiteral("value"), numeric);
    if (!xplugin::xPluginInterface().getRobotConnected()) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          QObject::tr("写入寄存器失败：%1").arg(registerName));
        return false;
    }

    // 等待当前寄存器写入请求返回，调用方据此决定是否继续写入后续参数。
    const QJsonValue reply = xplugin::xPluginInterface().rcExecRequest(
        QStringLiteral("fieldbus"), QStringLiteral("register.set_value"), data, false);
    // 当前接口按 true 或对象回复判定成功，不进一步解析对象中的业务字段。
    const bool wrote = (reply.isBool() && reply.toBool()) || reply.isObject();
    if (!wrote) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          QObject::tr("写入寄存器失败：%1").arg(registerName));
        return false;
    }
    return true;
}

RegisterIo::ReadResult RegisterIo::readValues(const QStringList &sources)
{
    const QStringList names = registerNames(sources);
    if (names.isEmpty())
        return ReadResult::Success;
    if (g_reading) {
        return ReadResult::Busy;
    }

    auto &interface = xplugin::xPluginInterface();
    if (!interface.getRobotConnected()) {
        g_liveValues.clear();
        return ReadResult::Disconnected;
    }

    // 读取期间连接发生变化时丢弃回复，下次刷新重新取得完整数据。
    const QString robot = interface.getRobotID() + QLatin1Char('@') + interface.getRobotIP();
    const quint64 generation = g_connectionGeneration;
    const QScopedValueRollback<bool> reading(g_reading, true);
    if (robot != g_liveRobot)
        g_liveValues.clear();

    QHash<QString, RegisterValues> incoming;
    for (const QString &name : names) {
        // XService 支持按名称读取单个寄存器数组，不经过 modbus 的全量监控接口。
        const QJsonValue reply = interface.getXServiceData(QStringLiteral("register.values"), name);
        if (!interface.getRobotConnected() || generation != g_connectionGeneration ||
            robot != interface.getRobotID() + QLatin1Char('@') + interface.getRobotIP()) {
            g_liveValues.clear();
            return ReadResult::Disconnected;
        }
        // 整个寄存器返回数组或带 #序号的对象；标量是服务错误码，不能当作读数。
        if ((!reply.isArray() && !reply.isObject()) ||
            (reply.isObject() && reply.toObject().contains(QStringLiteral("error_code")))) {
            g_liveValues.clear();
            return ReadResult::Failed;
        }
        incoming.insert(name, valueElements(QJsonObject{{QStringLiteral("value"), reply}}));
    }
    // 每个名称的回复都是完整数组，不能用上次数组填补本次缺失的元素。
    for (auto it = incoming.constBegin(); it != incoming.constEnd(); ++it)
        g_liveValues.insert(it.key(), it.value());
    g_liveRobot = robot;
    return ReadResult::Success;
}

QString RegisterIo::currentValue(const QString &name)
{
    return liveElements(name).value(0);
}

QStringList RegisterIo::valuesFrom(const QString &startName, int count)
{
    QStringList result;
    if (count <= 0 || startName.isEmpty())
        return result;

    // 只读取所选寄存器的数组元素，避免按地址排序后混入其他寄存器的数据。
    const RegisterValues values = liveElements(startName);
    if (values.isEmpty())
        return result;
    const int last = qMin(count - 1, values.lastKey());
    for (int index = 0; index <= last; ++index)
        result.append(values.value(index));
    return result;
}

void RegisterIo::attachLiveRefresh(QWidget *page, const std::function<QStringList()> &sources,
                                   const std::function<void()> &apply,
                                   const std::function<void()> &clear)
{
    if (page == nullptr) {
        return;
    }
    new LiveRefresh(page, sources, apply, clear);
}

void RegisterIo::attachLiveRefresh(QWidget *page, const std::function<QStringList()> &sources,
                                   const std::function<void(ReadResult)> &onRead)
{
    if (page == nullptr)
        return;
    new LiveRefresh(page, sources, {}, {}, onRead);
}
