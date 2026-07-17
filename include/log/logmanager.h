/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @ file logmanager.h
 * @ brief Plugin management log
 * @ version 1.0.0
 * @ author ROKAE Development Team
 *****************************************************************************/

#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "xplugin_global.h"
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QMutex>
#include <QMap>

#ifndef PLUGIN_LOG_FILE_NAME_PREFIX
#ifdef PLUGIN_NAME
#define PLUGIN_LOG_FILE_NAME_PREFIX QString("./log/xplugin/%1/%1").arg(PLUGIN_NAME)
#else
#define PLUGIN_LOG_FILE_NAME_PREFIX QString("./log/xplugin/xplugin")
#endif
#endif
#ifndef LOG_FILE_NAME_SUFFIX
#define LOG_FILE_NAME_SUFFIX QString("log")
#endif

//!
//! \brief The log level defines are globally used in log defines
//!
#define XPLUGIN_LOG_LEVEL_DEBUG 0
#define XPLUGIN_LOG_LEVEL_INFO 1
#define XPLUGIN_LOG_LEVEL_WARNING 2
#define XPLUGIN_LOG_LEVEL_ERROR 3
#define XPLUGIN_LOG_LEVEL_SUDO 4

//! The min log level can be defined in pro file.
//! \a Q_LEVEL is the log level of console.
//! \a FILE_LEVEL is the log level of log file.
#ifndef LOG_MIN_Q_LEVEL
#define LOG_MIN_Q_LEVEL XPLUGIN_LOG_LEVEL_DEBUG
#endif
#ifndef LOG_MIN_FILE_LEVEL
#define LOG_MIN_FILE_LEVEL XPLUGIN_LOG_LEVEL_INFO
#endif

#if LOG_MIN_Q_LEVEL > XPLUGIN_LOG_LEVEL_DEBUG
#define LOG_NO_Q_DEBUG
#endif
#if LOG_MIN_Q_LEVEL > XPLUGIN_LOG_LEVEL_INFO
#define LOG_NO_Q_INFO
#endif
#if LOG_MIN_Q_LEVEL > XPLUGIN_LOG_LEVEL_WARNING
#define LOG_NO_Q_WARNING
#endif
#if LOG_MIN_Q_LEVEL > XPLUGIN_LOG_LEVEL_ERROR
#define LOG_NO_Q_ERROR
#endif

#if LOG_MIN_FILE_LEVEL > XPLUGIN_LOG_LEVEL_DEBUG
#define LOG_NO_F_DEBUG
#endif
#if LOG_MIN_FILE_LEVEL > XPLUGIN_LOG_LEVEL_INFO
#define LOG_NO_F_INFO
#endif
#if LOG_MIN_FILE_LEVEL > XPLUGIN_LOG_LEVEL_WARNING
#define LOG_NO_F_WARNING
#endif
#if LOG_MIN_FILE_LEVEL > XPLUGIN_LOG_LEVEL_ERROR
#define LOG_NO_F_ERROR
#endif

#ifndef LOG_COLOR_STR_DEBUG
#define LOG_COLOR_STR_DEBUG "\033[32m"
#endif
#ifndef LOG_COLOR_STR_INFO
#define LOG_COLOR_STR_INFO "\033[36m"
#endif
#ifndef LOG_COLOR_STR_WARNING
#define LOG_COLOR_STR_WARNING "\033[33m"
#endif
#ifndef LOG_COLOR_STR_ERROR
#define LOG_COLOR_STR_ERROR "\033[31m"
#endif
#ifndef LOG_COLOR_STR_SUDO
#define LOG_COLOR_STR_SUDO "\033[35m"
#endif

#define XPLUGIN_LOG_LEVEL_STR_DEBUG QString("[D|%1]").arg(QDateTime::currentDateTime().toString("MM/dd hh:mm:ss.zzz"))
#define XPLUGIN_LOG_LEVEL_STR_INFO QString("[I|%1]").arg(QDateTime::currentDateTime().toString("MM/dd hh:mm:ss.zzz"))
#define XPLUGIN_LOG_LEVEL_STR_WARNING QString("[W|%1]").arg(QDateTime::currentDateTime().toString("MM/dd hh:mm:ss.zzz"))
#define XPLUGIN_LOG_LEVEL_STR_ERROR QString("[E|%1]").arg(QDateTime::currentDateTime().toString("MM/dd hh:mm:ss.zzz"))
#define XPLUGIN_LOG_LEVEL_STR_SUDO QString("[S|%1]").arg(QDateTime::currentDateTime().toString("MM/dd hh:mm:ss.zzz"))

#ifdef LOG_ENABLE_DETAILS
#define LOG_DETAILS_STR QString("{%1@%2}").arg(__FUNCTION__).arg(__LINE__)
#else
#define LOG_DETAILS_STR QString()
#endif

namespace xplugin {

// 日志文件管理器 - 单例模式
class XPLUGIN_EXPORT PluginLogFileManager : public QObject
{
    Q_OBJECT

public:
    static PluginLogFileManager& instance();

    // 获取合适的日志文件名，内部会按日期和文件大小自动滚动。
    QString getProperLogFileName(const QString& file_name_prefix, const QString& file_name_suffix);

    // 更新当前日志文件大小，并在总日志大小超过上限时触发旧日志清理。
    void updateFileSize(const QString& fileName, qint64 additionalSize);

    // 构建日志文件名：prefix_yyyyMMdd.log 或 prefix_yyyyMMdd_index.log。
    QString buildFileName(const QString& prefix, const QString& dateStr, int index, const QString& suffix);

    // 写日志失败时的兜底恢复：即使总量未超限，也会尝试删除至少一个旧日志。
    void recoverFromWriteFailure(const QString& file_name_prefix);

private:
    PluginLogFileManager(QObject* parent = nullptr);
    ~PluginLogFileManager() = default;

    // 第一次写日志时扫描现有日志，避免只统计本次进程新写入的文件。
    void initializeLogRoot(const QString& file_name_prefix);

    // 重新统计 xplugin 日志根目录下所有 *.log 文件的总大小。
    void rescanTotalLogSize(const QString& logRootPath);

    // 清理旧日志。
    // targetTotalSize：希望清理到的目标总量；
    // forceDeleteOne：用于写失败恢复，保证至少尝试删除一个最旧日志。
    void cleanupOldFiles(const QString &dirPath, qint64 targetTotalSize, bool forceDeleteOne);

private:
    QMutex m_mutex;
    bool m_logRootInitialized;
    QList<QString> m_moduleList;
    QMap<QString, int> m_currentFileIndex; // 每个日志模块当天正在写入的滚动文件索引
    QMap<QString, qint64> m_currentFileSize; // 每个日志模块当天正在写入的文件大小
    qint64 m_totalLogSize; // xplugin 日志根目录下所有 *.log 文件的总大小
};

/**
 * @brief The QLog class outputs log to console by means of qDebug()
 */
class XPLUGIN_EXPORT QLog
{
public:
    explicit QLog(const QString &color_str, const QString &level_str, const QString &detail_str);
    ~QLog();

    QLog & space();

    QLog & operator << (const QString &str);
    QLog & operator << (const QVariant &var);
    QLog & operator << (const void *ptr);
    template<typename T>
    inline QLog & operator << (const T &t)
    {
        m_q_debug << t;
        return *this;
    }

private:
    //copy of qDebug
    QDebug m_q_debug;
};

/**
 * @brief The FLog class outputs log to local files with QDebug()
 * @brief Max File Content 10M
 * @brief Max Total Log Size 200M
 */
class XPLUGIN_EXPORT FLog
{
public:
    explicit FLog(const QString &level_str, const QString &detail_str, const QString &file_name_prefix, const QString &file_name_suffix);
    ~FLog();

    FLog & space();

    FLog & operator << (const QString &str);
    FLog & operator << (const QVariant &var);
    FLog & operator << (const void *ptr);
    template<typename T>
    inline FLog & operator << (const T &t)
    {
        if (m_log_file.isOpen()) m_f_debug << t;
        return *this;
    }

private:
    //log file
    QFile m_log_file;
    //debug for save text
    QDebug m_f_debug;
    QString m_file_name;
    QString m_file_name_prefix;
};

/**
 * @brief The QFLog class outputs log to both console and file
 */
class XPLUGIN_EXPORT QFLog
{
public:
    explicit QFLog(const QString &color_str, const QString &level_str, const QString &detail_str,
                   const QString &file_name_prefix, const QString &file_name_suffix);

    QFLog & space();

    QFLog & operator << (const QString &str);
    QFLog & operator << (const QVariant &var);
    QFLog & operator << (const void *ptr);
    template<typename T>
    inline QFLog & operator << (const T &t)
    {
        m_q_log << t;
        m_f_log << t;
        return *this;
    }

private:
    QLog m_q_log;
    FLog m_f_log;
};
}

//DLOG debug
#if defined(LOG_NO_Q_DEBUG) && defined(LOG_NO_F_DEBUG)
#define DLOG if (false) QNoDebug()
#elif defined(LOG_NO_Q_DEBUG)
#define DLOG xplugin::FLog(XPLUGIN_LOG_LEVEL_STR_DEBUG, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#elif defined(LOG_NO_F_DEBUG)
#define DLOG xplugin::QLog(LOG_COLOR_STR_DEBUG, XPLUGIN_LOG_LEVEL_STR_DEBUG, LOG_DETAILS_STR)
#else
#define DLOG xplugin::QFLog(LOG_COLOR_STR_DEBUG, XPLUGIN_LOG_LEVEL_STR_DEBUG, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#endif

#define DSLOG (DLOG).space()

//ILOG info
#if defined(LOG_NO_Q_INFO) && defined(LOG_NO_F_INFO)
#define ILOG if (false) QNoDebug()
#elif defined(LOG_NO_Q_INFO)
#define ILOG xplugin::FLog(XPLUGIN_LOG_LEVEL_STR_INFO, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#elif defined(LOG_NO_F_INFO)
#define ILOG xplugin::QLog(LOG_COLOR_STR_INFO, XPLUGIN_LOG_LEVEL_STR_INFO, LOG_DETAILS_STR)
#else
#define ILOG xplugin::QFLog(LOG_COLOR_STR_INFO, XPLUGIN_LOG_LEVEL_STR_INFO, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#endif

#define ISLOG (ILOG).space()

//WLOG warning
#if defined(LOG_NO_Q_WARNING) && defined(LOG_NO_F_WARNING)
#define WLOG if (false) QNoDebug()
#elif defined(LOG_NO_Q_WARNING)
#define WLOG xplugin::FLog(XPLUGIN_LOG_LEVEL_STR_WARNING, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#elif defined(LOG_NO_F_WARNING)
#define WLOG xplugin::QLog(LOG_COLOR_STR_WARNING, XPLUGIN_LOG_LEVEL_STR_WARNING, LOG_DETAILS_STR)
#else
#define WLOG xplugin::QFLog(LOG_COLOR_STR_WARNING, XPLUGIN_LOG_LEVEL_STR_WARNING, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#endif

#define WSLOG (WLOG).space()

//ELOG error
#if defined(LOG_NO_Q_ERROR) && defined(LOG_NO_F_ERROR)
#define ELOG if (false) QNoDebug()
#elif defined(LOG_NO_Q_ERROR)
#define ELOG xplugin::FLog(XPLUGIN_LOG_LEVEL_STR_ERROR, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#elif defined(LOG_NO_F_ERROR)
#define ELOG xplugin::QLog(LOG_COLOR_STR_ERROR, XPLUGIN_LOG_LEVEL_STR_ERROR, LOG_DETAILS_STR)
#else
#define ELOG xplugin::QFLog(LOG_COLOR_STR_ERROR, XPLUGIN_LOG_LEVEL_STR_ERROR, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#endif

#define ESLOG (ELOG).space()

//SLOG
#define SLOG xplugin::QFLog(LOG_COLOR_STR_SUDO, XPLUGIN_LOG_LEVEL_STR_SUDO, LOG_DETAILS_STR, PLUGIN_LOG_FILE_NAME_PREFIX, LOG_FILE_NAME_SUFFIX)
#define SSLOG (SLOG).space()

#endif // LOGMANAGER_H
