/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsStore.cpp
 * @brief 读写示教器本地的 settings.json。
 */

#include "SettingsStore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QSaveFile>

namespace screw {

namespace {
QString settingsFilePath()
{
    return QDir(QCoreApplication::applicationDirPath())
        .filePath(QStringLiteral("xplugins/client/") + QString::fromLatin1(PLUGIN_NAME) +
                  QStringLiteral("/settings.json"));
}

QJsonObject pageObject(const QJsonObject &root, const QString &section)
{
    return root.value(QStringLiteral("pages")).toObject().value(section).toObject();
}

bool writePage(QJsonObject &root, const QString &section, const QJsonObject &page)
{
    QJsonObject pages = root.value(QStringLiteral("pages")).toObject();
    // 分区结构无效时拒绝写入，避免覆盖已保存的配置。
    if (section.isEmpty() || (pages.contains(section) && !pages.value(section).isObject()))
        return false;
    pages.insert(section, page);
    root.insert(QStringLiteral("pages"), pages);
    return SettingsStore::write(root);
}

} // namespace

bool SettingsStore::isDocument(const QJsonObject &root)
{
    return root.isEmpty() || (root.size() == 1 && root.value(QStringLiteral("pages")).isObject());
}

bool SettingsStore::read(QJsonObject &root)
{
    root = QJsonObject();
    QFile file(settingsFilePath());
    if (!file.exists())
        return true;
    if (!file.open(QIODevice::ReadOnly))
        return false;

    // 空文件可作为首次使用的配置。读取出错时返回失败，防止后续保存覆盖原文件。
    const QByteArray bytes = file.readAll();
    if (file.error() != QFileDevice::NoError)
        return false;
    if (bytes.trimmed().isEmpty())
        return true;

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject())
        return false;

    const QJsonObject candidate = document.object();
    if (!isDocument(candidate))
        return false;
    root = candidate;
    return true;
}

bool SettingsStore::write(const QJsonObject &root)
{
    if (!isDocument(root))
        return false;

    const QString path = settingsFilePath();
    if (!QDir().mkpath(QFileInfo(path).absolutePath())) {
        return false;
    }

    // 通过 QSaveFile 完整写入临时文件，提交成功后才替换本地设置。
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size()) {
        file.cancelWriting();
        return false;
    }
    return file.commit();
}

PageFields SettingsStore::loadPage(const QString &section)
{
    QJsonObject root;
    if (!read(root))
        return {};
    return PageFields::fromJson(pageObject(root, section));
}

PageFields SettingsStore::loadRecipe(const QString &section, int number)
{
    QJsonObject root;
    if (!read(root))
        return {};
    const QJsonObject recipes = pageObject(root, section).value(QStringLiteral("recipes")).toObject();
    return PageFields::fromJson(recipes.value(QString::number(number)).toObject());
}

bool SettingsStore::savePage(const QString &section, const PageFields &fields)
{
    QJsonObject root;
    return read(root) && writePage(root, section, fields.toJson());
}

bool SettingsStore::saveRecipe(const QString &section, int number, const PageFields &fields)
{
    QJsonObject root;
    if (number < 1 || number > 30 || !read(root))
        return false;

    QJsonObject page = pageObject(root, section);
    const QJsonValue stored = page.value(QStringLiteral("recipes"));
    if (page.contains(QStringLiteral("fields")) ||
        (page.contains(QStringLiteral("recipes")) && !stored.isObject()))
        return false;

    QJsonObject recipes = stored.toObject();
    recipes.insert(QString::number(number), fields.toJson());
    page.insert(QStringLiteral("recipes"), recipes);
    return writePage(root, section, page);
}

bool SettingsStore::saveField(const QString &section, const QString &key, const FieldCell &field,
                              PageFields &saved)
{
    QJsonObject root;
    if (!read(root))
        return false;

    QJsonObject page = pageObject(root, section);
    const QJsonValue stored = page.value(QStringLiteral("fields"));
    if (page.contains(QStringLiteral("recipes")) ||
        (page.contains(QStringLiteral("fields")) && !stored.isObject()))
        return false;
    QJsonObject fields = stored.toObject();
    QJsonObject row = fields.value(key).toObject();
    row.insert(QStringLiteral("registerName"), field.registerName);
    row.insert(QStringLiteral("value"), field.value);
    fields.insert(key, row);
    page.insert(QStringLiteral("fields"), fields);
    if (!writePage(root, section, page))
        return false;

    saved = PageFields::fromJson(page);
    return true;
}

} // namespace screw
