/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsData.cpp
 * @brief 页面参数与配方集合的 JSON 转换。
 */

#include "SettingsData.h"

QJsonObject PageFields::toJson() const
{
    QJsonObject fields;
    for (auto it = this->fields.constBegin(); it != this->fields.constEnd(); ++it) {
        QJsonObject cell;
        cell.insert(QStringLiteral("registerName"), it.value().registerName);
        cell.insert(QStringLiteral("value"), it.value().value);
        fields.insert(it.key(), cell);
    }

    QJsonObject object;
    object.insert(QStringLiteral("fields"), fields);
    return object;
}

PageFields PageFields::fromJson(const QJsonObject &object)
{
    PageFields data;
    const QJsonObject fields = object.value(QStringLiteral("fields")).toObject();
    for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
        const QJsonObject cell = it.value().toObject();
        FieldCell field;
        field.registerName = cell.value(QStringLiteral("registerName")).toString();
        field.value = cell.value(QStringLiteral("value")).toString();
        data.fields.insert(it.key(), field);
    }
    return data;
}

RecipeBook RecipeBook::fromJson(const QJsonObject &object)
{
    RecipeBook book;
    const QJsonObject recipes = object.value(QStringLiteral("recipes")).toObject();
    for (auto it = recipes.constBegin(); it != recipes.constEnd(); ++it) {
        book.recipes.insert(it.key(), PageFields::fromJson(it.value().toObject()));
    }
    return book;
}
