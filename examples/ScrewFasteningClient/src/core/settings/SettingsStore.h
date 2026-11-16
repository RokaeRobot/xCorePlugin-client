/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsStore.h
 * @brief 读写示教器本地的 settings.json。
 */

#ifndef SETTINGS_STORE_H
#define SETTINGS_STORE_H

#include "models/SettingsData.h"

namespace screw {

/** @brief 管理示教器本地设置文件，支持整份配置、页面、配方和单行参数的读写。 */
class SettingsStore final
{
public:
    /** @brief 检查设置文件顶层结构。允许空对象或仅包含 pages 对象的配置。 */
    static bool isDocument(const QJsonObject &root);

    /**
     * @brief 读取整份本地设置。
     * @param[out] root 读取前清空。文件不存在或为空时输出空对象。
     * @return 读取成功或文件尚无内容时返回 true。读取失败或格式无效时返回 false。
     */
    static bool read(QJsonObject &root);

    /** @brief 写完临时文件后替换整份本地设置，用于导入等整份更新操作。 */
    static bool write(const QJsonObject &root);

    /** @brief 读取指定页面的参数。页面缺失或读取失败时返回空参数。 */
    static PageFields loadPage(const QString &section);

    /** @brief 读取指定编号的配方参数。配方缺失或读取失败时返回空参数。 */
    static PageFields loadRecipe(const QString &section, int number);

    /** @brief 替换指定页面的参数，保留其他页面的配置。 */
    static bool savePage(const QString &section, const PageFields &fields);

    /** @brief 保存编号为 1 到 30 的指定配方，保留同页的其他配方。 */
    static bool saveRecipe(const QString &section, int number, const PageFields &fields);

    /**
     * @brief 更新指定页面的一行参数，保留同页的其他行。
     * @param[out] saved 写入成功后返回该页面已保存的参数，供调用方更新保存记录。
     */
    static bool saveField(const QString &section, const QString &key, const FieldCell &field,
                          PageFields &saved);

private:
    SettingsStore() = delete;
};

} // namespace screw

#endif // SETTINGS_STORE_H
