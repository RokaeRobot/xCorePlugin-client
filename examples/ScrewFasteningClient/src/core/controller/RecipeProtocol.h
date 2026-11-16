/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RecipeProtocol.h
 * @brief 把示教器保存的工艺配置转换成控制器配方，并核对控制器是否保存成功。
 */

#ifndef RECIPE_PROTOCOL_H
#define RECIPE_PROTOCOL_H

#include <QJsonArray>
#include <QJsonObject>
#include <QVector>

#include "models/SettingsData.h"
#include "rl/ProcessType.h"

namespace screw {

/** @brief 通信寄存器表中任务链执行寄存器的地址。客户端据此找到寄存器名称并传给控制器，由控制器写入该寄存器。 */
constexpr int kTaskChainExecutionRegisterAddress = 348;

/**
 * @brief 返回工艺在 settings.json 的 pages 中的分区名。
 * @param[in] type 工艺类型。
 * @return 例如扭矩工艺返回 run.torque。无法识别的工艺返回空字符串。
 */
QString processSection(xplugin::ProcessType type);

/** @brief 从本地配置中取出的一份已经填写过的工艺配方。 */
struct SavedProcessRecipe
{
    xplugin::ProcessType type; // 工艺类型。
    int index;                 // 配方号；吸式、吹式工艺固定为 0。
    PageFields fields;         // 寄存器名称和参数值。
};

/**
 * @brief 把一份本地 settings.json 里的工艺配方转换成控制器执行时使用的 JSON。
 *
 * 本地配置只有寄存器名称和数值。这里按通信寄存器表补上类型和位偏移，并写明地址 348 对应的触发寄存器。
 * 寄存器名称和数值都为空的参数行会跳过；只填写其中一项则转换失败。
 * @param[in] type 工艺类型。
 * @param[in] index 配方号。需要配方号的工艺必须是 1 到 30。吸式和吹式工艺不使用这个编号。
 * @param[in] fields 已保存的工艺参数。
 * @param[in] registers 当前通信寄存器表。
 * @param[out] recipe 转换成功后的配方。只在返回 true 时有效。
 * @param[out] error 失败原因。只在返回 false 时有效。
 * @return 转换并通过检查返回 true，否则返回 false。
 */
bool makeProcessRecipe(xplugin::ProcessType type, int index, const PageFields &fields,
                       const QJsonArray &registers, QJsonObject &recipe, QString &error);

/**
 * @brief 从 settings.json 中取出已经填写内容的工艺配方。
 *
 * 按工艺表的顺序返回。同一工艺有多份配方时，按配方号的文本排序。完全没填的配方不返回。
 * @param[in] settings settings.json 的完整内容。
 * @return 非空配方列表。没有可取出的配方时返回空列表。
 * @note 这里只读取数据，不判断能否下发。发送前还要用 makeRecipeCollection() 检查格式和编号。
 */
QVector<SavedProcessRecipe> savedProcessRecipes(const QJsonObject &settings);

/**
 * @brief 检查本地工艺配置，并生成要发给控制器的完整配方集合。
 *
 * 集合带有 SHA-256 内容标识。控制器确认保存时必须原样回传这个标识，用来确认保存的是这一份集合。
 * @param[in] settings 已保存的本地配置。
 * @param[in] registers 当前通信寄存器表，用来填写寄存器类型、位偏移和地址 348 对应的触发寄存器。
 * @param[out] collection 可以发送的完整集合。其中 recipes 为空时，表示删除控制器上的全部配方。
 * @param[out] error 失败原因。只在返回 false 时有效。
 * @return 全部配方都合法时返回 true。有一份不合法就整套都不生成。
 */
bool makeRecipeCollection(const QJsonObject &settings, const QJsonArray &registers,
                          QJsonObject &collection, QString &error);

/**
 * @brief 判断控制器是否确认保存了本次发送的配方集合。
 *
 * 回复中要有保存成功标志，而且回传的内容标识必须和本次集合一致。标识不同时，不能把别的集合的回复当成这次保存成功。
 * @param[in] reply 控制器返回的数据。
 * @param[in] collection 本次发送的完整配方集合。
 * @param[out] error 失败原因。只在返回 false 时有效。
 * @return 收到与本次集合匹配的保存成功回复时返回 true，否则返回 false。
 */
bool recipesAcknowledged(const QJsonArray &reply, const QJsonObject &collection, QString &error);

} // namespace screw

#endif // RECIPE_PROTOCOL_H
