/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RecipeProtocol.cpp
 * @brief 客户端配方与控制器通信数据之间的转换。
 *
 * 集中处理配方格式及保存确认检查，供手动保存和自动同步共用。
 */

#include "RecipeProtocol.h"
#include "ProcessFields.h"
#include "core/settings/SettingsStore.h"

#include <QCryptographicHash>
#include <QJsonDocument>
#include <QMap>
#include <QObject>
#include <QVector>
#include <algorithm>

namespace screw {

namespace {
// 检查寄存器配置，提取名称、类型和位偏移。
bool registerDescriptor(const QJsonObject &source, QJsonObject &target, QString &error)
{
    const QString name = source.value(QStringLiteral("name")).toString().trimmed();
    const QString type = source.value(QStringLiteral("type")).toString();
    const QStringList types = {QStringLiteral("int16"), QStringLiteral("int32"), QStringLiteral("float"),
                               QStringLiteral("bool"),  QStringLiteral("bit"),   QStringLiteral("byte")};
    if (name.isEmpty() || !types.contains(type)) {
        error = QObject::tr("寄存器名称或类型无效：%1").arg(name);
        return false;
    }

    // 单个位寄存器的配置位号从 1 开始，发送给控制器的位偏移从 0 开始。
    int bias = 0;
    if (type == QLatin1String("bit") && source.value(QStringLiteral("len")).toInt() == 1) {
        bias = qMax(0, source.value(QStringLiteral("bit_bias")).toInt() - 1);
    }
    target.insert(QStringLiteral("name"), name);
    target.insert(QStringLiteral("type"), type);
    target.insert(QStringLiteral("bias"), bias);
    return true;
}

// 判断配方是否填写了寄存器名称或数值。
bool hasContent(const PageFields &fields)
{
    for (const FieldCell &cell : fields.fields) {
        if (!cell.registerName.trimmed().isEmpty() || !cell.value.trimmed().isEmpty())
            return true;
    }
    return false;
}

// 检查文件中的参数名称和格式，避免无效参数在读取时被当作空参数跳过。
bool processFieldsValid(xplugin::ProcessType type, const QJsonObject &page, QString &error)
{
    if (!page.value(QStringLiteral("fields")).isObject() && !page.isEmpty()) {
        error = QObject::tr("工艺 %1 的参数格式不正确。").arg(xplugin::ProcessTypeTool::name(type));
        return false;
    }
    const QJsonObject fields = page.value(QStringLiteral("fields")).toObject();
    for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
        const QJsonObject cell = it.value().toObject();
        if (!processFieldAllowed(xplugin::ProcessTypeTool::name(type).toStdString(), it.key().toStdString()) ||
            !it.value().isObject() ||
            (cell.contains(QStringLiteral("registerName")) && !cell.value(QStringLiteral("registerName")).isString()) ||
            (cell.contains(QStringLiteral("value")) && !cell.value(QStringLiteral("value")).isString())) {
            error = QObject::tr("工艺 %1 的参数名称或格式不正确：%2。")
                        .arg(xplugin::ProcessTypeTool::name(type), it.key());
            return false;
        }
    }
    return true;
}

} // namespace

QString processSection(xplugin::ProcessType type)
{
    switch (type) {
    case xplugin::InvalidProcess:
        return {};
    case xplugin::TorqueSetting:
        return QStringLiteral("run.torque");
    case xplugin::SuctionProcess:
        return QStringLiteral("run.suction");
    case xplugin::BlowProcess:
        return QStringLiteral("run.blow");
    case xplugin::ScrewPoint:
        return QStringLiteral("run.screw");
    case xplugin::IoOutput:
        return QStringLiteral("run.io_out");
    case xplugin::IoWait:
        return QStringLiteral("run.io_wait");
    }
    return {};
}

bool makeProcessRecipe(xplugin::ProcessType type, int index, const PageFields &fields,
                       const QJsonArray &registers, QJsonObject &recipe, QString &error)
{
    if (xplugin::ProcessTypeTool::name(type).isEmpty() ||
        (xplugin::ProcessTypeTool::usesRecipeIndex(type) && (index < 1 || index > 30))) {
        error = QObject::tr("配方类型无效，或配方号不在 1 到 30 之间。");
        return false;
    }

    // 按寄存器名称补充类型信息，任务链执行寄存器（通信地址348）必须唯一。
    QMap<QString, QJsonObject> byName;
    QJsonObject trigger;
    int triggerCount = 0;
    for (const QJsonValue &item : registers) {
        const QJsonObject object = item.toObject();
        byName.insert(object.value(QStringLiteral("name")).toString(), object);
        if (object.contains(QStringLiteral("addr")) &&
            object.value(QStringLiteral("addr")).toInt(-1) == kTaskChainExecutionRegisterAddress) {
            trigger = object;
            ++triggerCount;
        }
    }
    if (triggerCount != 1) {
        error = QObject::tr("请在寄存器表中配置唯一的任务链执行寄存器（通信地址%1），当前找到 %2 个。")
                    .arg(kTaskChainExecutionRegisterAddress)
                    .arg(triggerCount);
        return false;
    }

    // 任务链执行寄存器（通信地址348）只接收整数类型，执行配方时用于选择锁丝机工艺。
    QJsonObject triggerDescriptor;
    if (!registerDescriptor(trigger, triggerDescriptor, error))
        return false;
    const QString triggerType = triggerDescriptor.value(QStringLiteral("type")).toString();
    if (triggerType != QLatin1String("int16") && triggerType != QLatin1String("int32") &&
        triggerType != QLatin1String("byte")) {
        error = QObject::tr("任务链执行寄存器（通信地址%1）必须为 int16、int32 或 byte 类型。")
                    .arg(kTaskChainExecutionRegisterAddress);
        return false;
    }

    // 按参数名称固定写入项的排列顺序，便于比较配方内容是否变化。
    QStringList keys = fields.fields.keys();
    std::sort(keys.begin(), keys.end());
    QJsonArray writes;
    for (const QString &key : keys) {
        if (!processFieldAllowed(xplugin::ProcessTypeTool::name(type).toStdString(), key.toStdString())) {
            error = QObject::tr("工艺 %1 没有参数 %2，配方未发送。")
                        .arg(xplugin::ProcessTypeTool::name(type), key);
            return false;
        }
        const FieldCell cell = fields.fields.value(key);
        const QString name = cell.registerName.trimmed();
        const QString value = cell.value.trimmed();

        // 跳过全空参数行。只填写名称或只填写数值时拒绝转换。
        if (name.isEmpty() && value.isEmpty())
            continue;
        if (name.isEmpty() || value.isEmpty()) {
            error = QObject::tr("当前有信息未设置：%1。请完整填写寄存器和数值。").arg(key);
            return false;
        }
        if (!byName.contains(name)) {
            error = QObject::tr("寄存器表中未找到：%1").arg(name);
            return false;
        }

        // 任务链执行寄存器（通信地址348）保留给工艺触发，避免参数写入时提前启动任务链。
        const QJsonObject source = byName.value(name);
        if (source.contains(QStringLiteral("addr")) &&
            source.value(QStringLiteral("addr")).toInt(-1) == kTaskChainExecutionRegisterAddress) {
            error = QObject::tr("配方参数不能使用任务链执行寄存器（通信地址%1）。")
                        .arg(kTaskChainExecutionRegisterAddress);
            return false;
        }
        QJsonObject write;
        if (!registerDescriptor(source, write, error))
            return false;
        write.insert(QStringLiteral("field"), key);
        write.insert(QStringLiteral("value"), value);
        writes.append(write);
    }
    if (writes.isEmpty()) {
        error = QObject::tr("当前配方没有可下发的参数。");
        return false;
    }

    // 不使用配方号的工艺统一以 0 作为控制器配方集合中的编号。
    recipe = QJsonObject();
    recipe.insert(QStringLiteral("schemaVersion"), 1);
    recipe.insert(QStringLiteral("type"), xplugin::ProcessTypeTool::name(type));
    recipe.insert(QStringLiteral("index"), xplugin::ProcessTypeTool::usesRecipeIndex(type) ? index : 0);
    recipe.insert(QStringLiteral("trigger"), triggerDescriptor);
    recipe.insert(QStringLiteral("writes"), writes);
    return true;
}

QVector<SavedProcessRecipe> savedProcessRecipes(const QJsonObject &settings)
{
    const QJsonObject pages = settings.value(QStringLiteral("pages")).toObject();
    QVector<SavedProcessRecipe> result;
    for (int i = 0; i < xplugin::ProcessTypeTool::count(); ++i) {
        const auto type = xplugin::ProcessTypeTool::at(i);
        const QJsonObject page = pages.value(processSection(type)).toObject();

        // 吸式和吹式工艺各保存一份参数，不按配方号拆分。
        if (!xplugin::ProcessTypeTool::usesRecipeIndex(type)) {
            const auto fields = PageFields::fromJson(page);
            if (hasContent(fields))
                result.append({type, 0, fields});
            continue;
        }
        const RecipeBook book = RecipeBook::fromJson(page);

        // 带配方号的页面逐份取出已经保存的配方，全空配方不参与同步。
        QStringList keys = book.recipes.keys();
        std::sort(keys.begin(), keys.end());
        for (const QString &key : keys) {
            bool ok = false;
            const int index = key.toInt(&ok);
            if (hasContent(book.recipes.value(key)))
                result.append({type, ok ? index : -1, book.recipes.value(key)});
        }
    }
    return result;
}

bool makeRecipeCollection(const QJsonObject &settings, const QJsonArray &registers,
                          QJsonObject &collection, QString &error)
{
    if (!SettingsStore::isDocument(settings)) {
        error = QObject::tr("本地配置格式不正确，配方未发送。");
        return false;
    }
    const QJsonObject pages = settings.value(QStringLiteral("pages")).toObject();
    for (int index = 0; index < xplugin::ProcessTypeTool::count(); ++index) {
        const auto type = xplugin::ProcessTypeTool::at(index);
        const QString section = processSection(type);
        if (!pages.contains(section))
            continue;
        if (!pages.value(section).isObject()) {
            error = QObject::tr("工艺页面 %1 的格式不正确，配方未发送。").arg(section);
            return false;
        }
        const QJsonObject page = pages.value(section).toObject();
        if (!xplugin::ProcessTypeTool::usesRecipeIndex(type)) {
            if (!processFieldsValid(type, page, error))
                return false;
            continue;
        }
        if (!page.isEmpty() && (!page.value(QStringLiteral("recipes")).isObject() ||
                               page.contains(QStringLiteral("fields")))) {
            error = QObject::tr("工艺页面 %1 的配方集合格式不正确。").arg(section);
            return false;
        }
        const QJsonObject recipes = page.value(QStringLiteral("recipes")).toObject();
        for (auto it = recipes.constBegin(); it != recipes.constEnd(); ++it) {
            bool ok = false;
            const int number = it.key().toInt(&ok);
            if (!ok || number < 1 || number > 30 || QString::number(number) != it.key() ||
                !it.value().isObject()) {
                error = QObject::tr("工艺页面 %1 的配方号或格式不正确：%2。").arg(section, it.key());
                return false;
            }
            if (!processFieldsValid(type, it.value().toObject(), error))
                return false;
        }
    }
    QJsonObject recipes;
    for (const auto &saved : savedProcessRecipes(settings)) {
        QJsonObject recipe;
        if (!makeProcessRecipe(saved.type, saved.index, saved.fields, registers, recipe, error))
            return false;
        const QString type = xplugin::ProcessTypeTool::name(saved.type);
        QJsonObject book = recipes.value(type).toObject();
        book.insert(QString::number(recipe.value(QStringLiteral("index")).toInt()), recipe);
        recipes.insert(type, book);
    }
    collection = {{QStringLiteral("schemaVersion"), 1}, {QStringLiteral("recipes"), recipes}};
    // 标识只覆盖版本和配方内容，不包含 revision 自身；控制器必须原样回传以确认本次集合。
    const QByteArray bytes = QJsonDocument(collection).toJson(QJsonDocument::Compact);
    collection.insert(QStringLiteral("revision"), QString::fromLatin1(
        QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex()));
    return true;
}

bool recipesAcknowledged(const QJsonArray &reply, const QJsonObject &collection, QString &error)
{
    // 保存确认必须来自 replace_recipes 服务。
    for (const QJsonValue &item : reply) {
        const QJsonValue value = item.toObject()
                                     .value(QStringLiteral("ScrewFastening"))
                                     .toObject()
                                     .value(QStringLiteral("replace_recipes"));
        if (!value.isObject())
            continue;
        const QJsonObject response = value.toObject();

        // 内容标识必须匹配，避免把另一份配方集合的回复当作保存成功。
        if (response.value(QStringLiteral("ok")).isBool() && response.value(QStringLiteral("ok")).toBool() &&
            collection.value(QStringLiteral("revision")).isString() &&
            !collection.value(QStringLiteral("revision")).toString().isEmpty() &&
            response.value(QStringLiteral("revision")) == collection.value(QStringLiteral("revision")))
            return true;
        error = response.value(QStringLiteral("error")).toString();
        if (error.isEmpty())
            error = QObject::tr("控制器返回了无效的配方确认。");
        return false;
    }
    error = QObject::tr("未收到完整配方保存确认，请检查 ScrewFastening 控制器插件是否已安装并启用。");
    return false;
}

} // namespace screw
