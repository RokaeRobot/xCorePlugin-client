#ifndef PLUGINCOMMON_H
#define PLUGINCOMMON_H
/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @ file plugincommon.h
 * @ brief Plugin management framework with registration mechanism
 * @ version 1.0.0
 * @ author ROKAE Development Team
 *****************************************************************************/

#include <QHash>
#include <QObject>
#include <functional>
#include "xplugin_global.h"

/**
 * @def XPLUGIN_RUN(content)
 * @brief Run the plugin created by user
 */
#define XPLUGIN_RUN(_CONTENT)  \
    namespace { \
    int runAll() { _CONTENT; return 0; } \
    int rel = runAll(); \
    }

/**
 * @def XPLUGIN_REGISTER(key, className)
 * @brief Registers a plugin class with the factory
 * @param key Unique plugin identifier string
 * @param className Name of plugin class to register
 *
 * Usage:
 * @code
 *   XPLUGIN_REGISTER("PluginName", PluginClassName)
 * @endcode
 */
#define XPLUGIN_REGISTER(KEY, CLASS_NAME) \
    XPLUGIN_RUN( \
    xPluginFactory().regist(KEY, [](QObject *parent) -> xplugin::PluginBase* { \
        return new xplugin::CLASS_NAME(parent); \
    }))

namespace xplugin {

/**
 * @enum enLanguage
 * @brief Language type.
 */
enum enLanguage{
   English = 0,
   Chinese = 1,
   OtherLanguage = 0xFF
};

/**
 * @enum enThrowLogLevel
 * @brief Log Level.
 */
enum enThrowLogLevel{
   Info = 0,
   Warning,
   Error,
   NoneLogType = 0XFF
};

/**
 * @class PluginBase
 * @brief Abstract base class for all plugins
 *
 * Inherit from this class to implement custom plugins. Override the
 * lifecycle methods to implement plugin-specific behavior.
 */
class XPLUGIN_EXPORT PluginBase : public QObject
{
    Q_OBJECT
public:
    explicit PluginBase(QObject *parent = nullptr);

    virtual ~PluginBase();

    PluginBase(const PluginBase&) = delete;
    PluginBase& operator=(const PluginBase&) = delete;

public:
    /**
      * @brief Init ！ Step 1
      */
    virtual void init() {}
    /**
      * @brief After the Init ！ Step 2
      */
    virtual void afterInit() {}
    /**
      * @brief render！ Step 3
      */
    virtual void render() {}
    /**
      * @brief Already ！ Step 3
      */
    virtual void idle() {}
    /**
      * @brief next detach ！ Step 3
      */
    virtual void detach() {}

public:
    /**
      * @brief set global sheet used
      * @warning only useful in Constructor
      */
    void setDefaultGlobalSheet(bool useful = true);
    /**
      * @brief get global sheet used
      */
    bool getDefaultGlobalSheet() const;
    /**
      * @brief set plugin Label
      * @warning only useful in Constructor
      */
    void setPluginLabel(const QString &lbl);
    /**
      * @brief get plugin Label
      */
   QString getPluginLabel() const;

   /**
     * @brief set unique plugin widget
     */
   void setUniqueStatus(bool status = true);
   bool getUniqueStatus();

private:
    class PluginBasePrivate;
    PluginBasePrivate* m_p;
};

/**
 * @class PluginRegistry
 * @brief Central registry for plugin creators
 *
 * Maintains mapping between plugin IDs and their factory functions.
 * Should not be used directly - use registration macros instead.
 */

class XPLUGIN_EXPORT PluginRegister
{
public:
    explicit PluginRegister() {}
    ~PluginRegister() {}

    /**
      * @brief Register a plugin creator function
      * @param full_key Unique plugin identifier
      * @param creator Factory function for plugin instantiation
      */
    void regist(const QString &full_key, std::function<xplugin::PluginBase*(QObject *)> func);
    /**
      * @brief Unregister a plugin creator function
      * @param full_key Unique plugin identifier
      */
    void cancel(const QString &full_key);
    /**
      * @brief Create plugin instance
      * @param pluginId Unique plugin identifier
      * @param parent Parent QObject
      * @return New plugin instance or nullptr if not found
      */
    PluginBase* create(const QString &full_key, QObject* parent);

private:
    QHash<QString, std::function<xplugin::PluginBase*(QObject *)>> m_funcs;
};
}

/**
 * @brief Get the global plugin registry instance
 * @return Singleton instance of PluginRegistry
 */
XPLUGIN_EXPORT xplugin::PluginRegister & xPluginFactory();

#endif // PLUGINCOMMON_H
