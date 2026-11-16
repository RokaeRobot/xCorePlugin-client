/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RecipeSync.cpp
 * @brief 将客户端保存的完整配方集合发送到控制器。
 */

#include "RecipeSync.h"

#include <QDateTime>
#include <QScopedValueRollback>
#include "interface/interfacemanager.h"
#include "RunLock.h"
#include "core/settings/SettingsStore.h"

RecipeSync::RecipeSync(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(2000);
    connect(&m_timer, &QTimer::timeout, this, [this]() { tick(); });
    connect(&xplugin::xPluginInterface(), &xplugin::InterfaceManager::sigNetworkDisConnect, this,
            [this](bool) {
                ++m_connectionGeneration;
                m_robot.clear();
                m_acknowledged.clear();
                m_retryRevision.clear();
                m_retryAt = 0;
                m_reportedError.clear();
            });
    m_timer.start();
}

bool RecipeSync::synchronize(QString &error)
{
    if (!xplugin::xPluginInterface().getRobotConnected()) {
        error = tr("配方已保存在示教器本地；当前未连接控制器，连接后将自动同步完整配方集合。");
        return false;
    }
    QJsonObject settings;
    if (!screw::SettingsStore::read(settings)) {
        error = tr("本地 settings.json 读取失败，控制器配方未更新。");
        return false;
    }
    QJsonObject collection;
    if (!screw::makeRecipeCollection(settings, xplugin::xPluginInterface().getRegisterValue().toArray(),
                                     collection, error))
        return false;
    return sendCollection(collection, error);
}

bool RecipeSync::sendCollection(const QJsonObject &collection, QString &error)
{
    auto &interface = xplugin::xPluginInterface();
    if (!interface.getRobotConnected()) {
        error = tr("控制器连接已断开，完整配方集合将在重新连接后同步。");
        return false;
    }
    if (m_busy || screw::RunLock::instance().isLocked()) {
        error = tr("正在等待配方同步完成或程序停止，请稍后重试。");
        return false;
    }

    const QScopedValueRollback<bool> guard(m_busy, true);
    const QString robot = interface.getRobotID() + QLatin1Char('@') + interface.getRobotIP();
    if (m_robot != robot) {
        m_robot = robot;
        m_acknowledged.clear();
        m_retryRevision.clear();
        m_retryAt = 0;
        m_reportedError.clear();
    }
    // 同步调用可能处理断线事件；回复必须同时属于当前控制器和当前连接代次。
    const quint64 generation = m_connectionGeneration;
    const QJsonObject request{{QStringLiteral("replace_recipes"), collection}};
    const auto reply = interface.commandCustomData(this, QStringLiteral("ScrewFastening"), request);
    if (!interface.getRobotConnected() || generation != m_connectionGeneration ||
        robot != interface.getRobotID() + QLatin1Char('@') + interface.getRobotIP()) {
        error = tr("同步期间控制器连接已变化，完整配方集合将在重新连接后同步。");
        return false;
    }
    if (!screw::recipesAcknowledged(reply, collection, error))
        return false;

    m_acknowledged = collection.value(QStringLiteral("revision")).toString();
    m_retryRevision.clear();
    m_retryAt = 0;
    m_reportedError.clear();
    return true;
}

void RecipeSync::tick()
{
    auto &interface = xplugin::xPluginInterface();
    if (!interface.getRobotConnected()) {
        m_robot.clear();
        m_acknowledged.clear();
        m_retryRevision.clear();
        m_retryAt = 0;
        m_reportedError.clear();
        return;
    }
    if (m_busy || screw::RunLock::instance().isLocked())
        return;

    const QString robot = interface.getRobotID() + QLatin1Char('@') + interface.getRobotIP();
    if (robot != m_robot) {
        m_robot = robot;
        m_acknowledged.clear();
        m_retryRevision.clear();
        m_retryAt = 0;
        m_reportedError.clear();
    }

    QJsonObject settings;
    QJsonObject collection;
    QString error;
    if (!screw::SettingsStore::read(settings)) {
        error = tr("本地 settings.json 读取失败，控制器配方未更新。");
    } else if (settings.isEmpty()) {
        // 尚未保存本地配置时不清空控制器，导入明确的空配方集合时允许清空控制器。
        return;
    } else if (screw::makeRecipeCollection(settings, interface.getRegisterValue().toArray(), collection, error)) {
        const QString revision = collection.value(QStringLiteral("revision")).toString();
        if (revision == m_acknowledged ||
            (revision == m_retryRevision && m_retryAt > QDateTime::currentMSecsSinceEpoch()))
            return;
        if (sendCollection(collection, error))
            return;
        // 同一集合失败后等待 10 秒；内容变化生成新 revision，可在下一轮立即发送。
        m_retryRevision = revision;
        m_retryAt = QDateTime::currentMSecsSinceEpoch() + 10000;
    }
    if (error != m_reportedError) {
        m_reportedError = error;
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Error,
                          tr("完整配方同步未完成：%1").arg(error));
    }
}
