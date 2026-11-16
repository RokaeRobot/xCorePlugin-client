/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsData.h
 * @brief 页面参数与配方集合的数据结构。
 */

#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include <QHash>
#include <QJsonObject>
#include <QString>

/** @brief 单个参数的寄存器名称和待写入数值。 */
struct FieldCell
{
    QString registerName; // 参数对应的寄存器名称。
    QString value;        // 待写入的数值文本。

    /**
     * @brief 比较参数是否相同。
     * @param[in] other 待比较的参数。
     * @return 寄存器名称和值的原始文本均相同返回 true，不修剪空白或换算数值。
     */
    bool operator==(const FieldCell &other) const
    {
        return registerName == other.registerName && value == other.value;
    }
};

/** @brief 按参数名组织的一组页面参数，可用于单个页面或一份配方。 */
struct PageFields
{
    QHash<QString, FieldCell> fields; // 按参数名称标识保存的寄存器和数值。

    /**
     * @brief 比较页面配置是否相同。
     * @param[in] other 待比较的页面配置。
     * @return 全部参数相同返回 true，否则返回 false。
     */
    bool operator==(const PageFields &other) const
    {
        return fields == other.fields;
    }
    /**
     * @brief 比较页面配置是否不同。
     * @param[in] other 待比较的页面配置。
     * @return 存在不同参数返回 true，否则返回 false。
     */
    bool operator!=(const PageFields &other) const
    {
        return !(*this == other);
    }

    /**
     * @brief 将页面配置转换为示教器本地配置文件使用的 JSON 数据。
     * @return 包含 fields 段的 JSON 对象。
     */
    QJsonObject toJson() const;

    /**
     * @brief 将示教器本地配置文件中的 JSON 数据转换为页面配置。
     * @param[in] object 包含 fields 段的 JSON 对象。
     * @return 页面参数。缺失的寄存器名称或数值按空字符串处理。
     */
    static PageFields fromJson(const QJsonObject &object);
};

/** @brief 以配方号文本为键的页面配置集合。 */
struct RecipeBook
{
    QHash<QString, PageFields> recipes; // 按配方号保存的页面配置。

    /**
     * @brief 比较配方集合是否相同。
     * @param[in] other 待比较的配方集合。
     * @return 全部配方相同返回 true，否则返回 false。
     */
    bool operator==(const RecipeBook &other) const
    {
        return recipes == other.recipes;
    }
    /**
     * @brief 比较配方集合是否不同。
     * @param[in] other 待比较的配方集合。
     * @return 存在不同配方返回 true，否则返回 false。
     */
    bool operator!=(const RecipeBook &other) const
    {
        return !(*this == other);
    }

    /**
     * @brief 将示教器本地配置文件中的 JSON 数据转换为配方集合。
     * @param[in] object 包含 recipes 段的 JSON 对象。
     * @return 配方集合；缺少 recipes 段时返回空集合。
     */
    static RecipeBook fromJson(const QJsonObject &object);
};

#endif // SETTINGS_DATA_H
