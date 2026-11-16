/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsTransfer.cpp
 * @brief 插件配置的导入和导出。
 */

#include "SettingsTransfer.h"

#include <QComboBox>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPointer>
#include <QSaveFile>
#include <QVariant>
#include <cmath>
#include <limits>

#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <unistd.h>
#endif

#include "core/settings/SettingsStore.h"
#include "core/controller/RecipeProtocol.h"
#include "core/controller/RecipeSync.h"
#include "core/controller/RunLock.h"
#include "SettingsEditor.h"
#include "WidgetFactory.h"
#include "interface/interfacemanager.h"
#include "SettingsFileDialog.h"

namespace {
const char kTransferBusy[] = "settingsTransferBusy"; // 记录配置导入或导出是否正在进行的属性名称。

// 文件选择和确认会处理界面事件；作用域内阻止重入，退出时清除状态。
class TransferGuard
{
public:
    explicit TransferGuard(QWidget *root) : m_root(root)
    {
        root->setProperty(kTransferBusy, true);
    }
    ~TransferGuard()
    {
        if (m_root)
            m_root->setProperty(kTransferBusy, false);
    }

private:
    QPointer<QWidget> m_root; // 插件主界面指针，主界面销毁后自动变为空指针。
};

// 保存配置文件中单个参数的名称、寄存器名称和设定值。
struct StoredField
{
    QString key;   // 参数名称，用于对应页面中的配置项。
    QString name;  // 参数使用的寄存器名称。
    QString value; // 参数设定值。
    QString section; // 参数所属的配置页面。
};

// 检查参数配置格式，收集参数名称、寄存器名称和设定值。
bool appendFields(const QJsonObject &fields, QList<StoredField> &references, const QString &section)
{
    for (auto it = fields.constBegin(); it != fields.constEnd(); ++it) {
        if (!it.value().isObject())
            return false;
        const QJsonObject field = it.value().toObject();
        for (const QString &key : {QStringLiteral("registerName"), QStringLiteral("value")}) {
            if (field.contains(key) && !field.value(key).isString())
                return false;
        }
        references.append({it.key(), field.value(QStringLiteral("registerName")).toString(),
                           field.value(QStringLiteral("value")).toString(), section});
    }
    return true;
}

// 检查页面及配方配置的结构，收集待检查的参数。
bool collectReferences(const QJsonObject &root, QList<StoredField> &references)
{
    if (!screw::SettingsStore::isDocument(root) || !root.contains(QStringLiteral("pages")))
        return false;
    const QJsonObject pages = root.value(QStringLiteral("pages")).toObject();
    for (auto page = pages.constBegin(); page != pages.constEnd(); ++page) {
        if (!page.value().isObject())
            return false;
        const QJsonObject section = page.value().toObject();
        if (section.contains(QStringLiteral("fields")) &&
            (!section.value(QStringLiteral("fields")).isObject() ||
             !appendFields(section.value(QStringLiteral("fields")).toObject(), references, page.key())))
            return false;
        if (!section.contains(QStringLiteral("recipes")))
            continue;
        if (!section.value(QStringLiteral("recipes")).isObject())
            return false;
        const QJsonObject recipes = section.value(QStringLiteral("recipes")).toObject();
        for (auto recipe = recipes.constBegin(); recipe != recipes.constEnd(); ++recipe) {
            bool ok = false;
            const int index = recipe.key().toInt(&ok);
            const QJsonObject data = recipe.value().toObject();
            if (!ok || index < 1 || index > 30 || !recipe.value().isObject() ||
                !data.value(QStringLiteral("fields")).isObject() ||
                !appendFields(data.value(QStringLiteral("fields")).toObject(), references, page.key()))
                return false;
        }
    }
    return true;
}

// 检查设定值是否符合寄存器的数据类型和数值范围，空设定值允许通过。
bool numericValueValid(const QString &text, const QString &type)
{
    if (text.trimmed().isEmpty())
        return true;
    bool ok = false;
    if (type == QLatin1String("float")) {
        const double value = text.toDouble(&ok);
        return ok && std::isfinite(value) && std::abs(value) <= std::numeric_limits<float>::max();
    }
    const qlonglong value = text.toLongLong(&ok);
    if (!ok)
        return false;
    if (type == QLatin1String("bool") || type == QLatin1String("bit"))
        return value == 0 || value == 1;
    if (type == QLatin1String("byte"))
        return value >= 0 && value <= 255;
    if (type == QLatin1String("int16"))
        return value >= -32768 && value <= 32767;
    return type == QLatin1String("int32") && value >= std::numeric_limits<qint32>::min() &&
           value <= std::numeric_limits<qint32>::max();
}

// 检查导入配置引用的寄存器及运行控制配方是否符合要求。
bool registersMatch(QWidget *root, const QJsonObject &settings, const QList<StoredField> &references)
{
    // 按页面登记界面已经定义的参数，导入文件不能增加界面中不存在的参数。
    QMap<QString, QMap<QString, bool>> knownFields;
    for (QComboBox *combo : root->findChildren<QComboBox *>()) {
        if (!combo->property(screw::kRegisterComboProperty).toBool())
            continue;
        QString section;
        for (QWidget *parent = combo->parentWidget(); parent != nullptr; parent = parent->parentWidget()) {
            if (parent->property(screw::kSettingsSectionProperty).isValid()) {
                section = parent->property(screw::kSettingsSectionProperty).toString();
                break;
            }
            if (parent == root)
                break;
        }
        const QString key = combo->objectName().mid(6);
        QLineEdit *edit = combo->parentWidget()->findChild<QLineEdit *>(QStringLiteral("edit_") + key);
        knownFields[section].insert(key, edit != nullptr && !edit->property(screw::kSettingsReadOnlyProperty).toBool());
    }
    const QJsonObject pages = settings.value(QStringLiteral("pages")).toObject();
    for (auto page = pages.constBegin(); page != pages.constEnd(); ++page) {
        if (!knownFields.contains(page.key()))
            return false;
    }
    // 取得当前寄存器配置，按寄存器名称查找数据类型、读写属性和数组长度。
    const QJsonArray registers = xplugin::xPluginInterface().getRegisterValue().toArray();
    QMap<QString, QJsonObject> byName;
    for (const QJsonValue &item : registers) {
        const QJsonObject object = item.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        if (byName.contains(name))
            return false;
        byName.insert(name, object);
    }
    // 曲线数据、输入状态、输出状态和运行文件名称需要读取完整的寄存器数组。
    const QMap<QString, int> lengths = {{QStringLiteral("curve_data"), 400},
                                        {QStringLiteral("st_io_in_state"), 36},
                                        {QStringLiteral("st_io_out_state"), 24},
                                        {QStringLiteral("st_run_file"), 10}};
    const QStringList types = {QStringLiteral("int16"), QStringLiteral("int32"), QStringLiteral("float"),
                               QStringLiteral("bit"),   QStringLiteral("bool"),  QStringLiteral("byte")};
    for (const StoredField &field : references) {
        const auto fields = knownFields.value(field.section);
        if (!fields.contains(field.key))
            return false;
        const QString name = field.name.trimmed();
        // 可写参数必须同时提供寄存器和数值，整行空白允许导入。
        const bool needsWrite = fields.value(field.key);
        if (needsWrite && (name.isEmpty() != field.value.trimmed().isEmpty()))
            return false;
        if (name.isEmpty())
            continue;
        if (!byName.contains(name))
            return false;
        const QJsonObject object = byName.value(name);
        const QString type = object.value(QStringLiteral("type")).toString();
        const QString rw = object.value(QStringLiteral("rw")).toString();
        if (!types.contains(type) ||
            object.value(QStringLiteral("len")).toInt() < lengths.value(field.key, 1) ||
            (needsWrite ? (rw != QLatin1String("rdwr") && rw != QLatin1String("wr"))
                        : (rw != QLatin1String("rdwr") && rw != QLatin1String("rd"))) ||
            (needsWrite && !numericValueValid(field.value, type)))
            return false;
    }
    const PageFields curve = PageFields::fromJson(pages.value(QStringLiteral("curve")).toObject());
    if (curve.fields.value(QStringLiteral("curve_group_count")).registerName.trimmed().isEmpty() !=
        curve.fields.value(QStringLiteral("curve_data")).registerName.trimmed().isEmpty())
        return false;
    QJsonObject collection;
    QString error;
    if (!screw::makeRecipeCollection(settings, registers, collection, error))
        return false;
    // 任务链执行寄存器（通信地址348）必须允许写入。
    for (const screw::SavedProcessRecipe &saved : screw::savedProcessRecipes(settings)) {
        QJsonObject recipe;
        QString error;
        if (!screw::makeProcessRecipe(saved.type, saved.index, saved.fields, registers, recipe, error))
            return false;
        const QJsonObject trigger = byName.value(
            recipe.value(QStringLiteral("trigger")).toObject().value(QStringLiteral("name")).toString());
        const QString rw = trigger.value(QStringLiteral("rw")).toString();
        if (rw != QLatin1String("rdwr") && rw != QLatin1String("wr"))
            return false;
    }
    return true;
}

// 先将完整配置写入临时文件，内容完整写入后替换目标文件。
bool writeExportFile(const QString &path, const QJsonObject &root)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size()) {
        file.cancelWriting();
        return false;
    }
    if (!file.commit())
        return false;
#ifdef Q_OS_UNIX
    // 在 Unix 平台等待文件内容保存到存储设备后，再报告导出成功。
    const int descriptor = ::open(QFile::encodeName(path).constData(), O_RDONLY);
    if (descriptor < 0)
        return false;
    const bool synchronized = ::fsync(descriptor) == 0;
    ::close(descriptor);
    if (!synchronized)
        return false;
#endif
    return true;
}

// 检查是否允许导入配置，程序运行期间显示提示并拒绝导入。
bool importAllowed(QWidget *root)
{
    if (!screw::RunLock::instance().isLocked())
        return true;
    QMessageBox::warning(root, QObject::tr("导入配置"),
                         QObject::tr("程序运行中，暂不能导入配置。请停止程序后再操作。"));
    return false;
}
} // namespace

void screw::exportSettings(QWidget *root)
{
    if (root == nullptr || root->property(kTransferBusy).toBool())
        return;
    const TransferGuard operation(root);
    const QPointer<QWidget> page(root);
    // 只读取已经保存的配置，页面中尚未保存的修改不参与导出。
    QJsonObject saved;
    if (!SettingsStore::read(saved)) {
        QMessageBox::warning(root, QObject::tr("导出配置"), QObject::tr("已保存的配置读取失败，未导出。"));
        return;
    }
    if (saved.isEmpty()) {
        QMessageBox::information(root, QObject::tr("导出配置"), QObject::tr("尚无已保存的配置可导出。"));
        return;
    }
    const auto selection = selectSettingsFile(root, false);
    if (!page || selection.path.isEmpty())
        return;
    // 文件选择窗口打开期间可能发生配置保存，写入导出文件前重新读取已保存的配置。
    const bool success = SettingsStore::read(saved) && settingsFileAvailable(selection, false) &&
                         writeExportFile(selection.path, saved);
    if (success)
        QMessageBox::information(root, QObject::tr("导出配置"), QObject::tr("已保存的配置已导出。"));
    else
        QMessageBox::warning(root, QObject::tr("导出配置"),
                             QObject::tr("配置导出失败，请检查保存位置是否可写及存储设备是否可用。"));
}

void screw::importSettings(QWidget *root)
{
    if (root == nullptr || root->property(kTransferBusy).toBool())
        return;
    const TransferGuard operation(root);
    const QPointer<QWidget> page(root);
    if (!importAllowed(root) || !page)
        return;
    const auto selection = selectSettingsFile(root, true);
    if (!page || selection.path.isEmpty())
        return;
    // 读取用户选择的配置文件并检查格式，检查失败时保留当前已保存的配置。
    QFile file(selection.path);
    if (!settingsFileAvailable(selection, true) || !file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(root, QObject::tr("导入配置"), QObject::tr("配置文件读取失败，未导入。"));
        return;
    }
    const QByteArray bytes = file.readAll();
    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    QList<StoredField> references;
    if (file.error() != QFileDevice::NoError || error.error != QJsonParseError::NoError ||
        !document.isObject() || !collectReferences(document.object(), references)) {
        QMessageBox::warning(root, QObject::tr("导入配置"), QObject::tr("配置文件格式不正确，未导入。"));
        return;
    }
    file.close();
    // 替换配置前检查页面中尚未保存的修改，并确认是否替换已保存的配置。
    if (!confirmSettingsLeave(root) || !page)
        return;
    const int answer = QMessageBox::question(root, QObject::tr("导入配置"),
                                             QObject::tr("导入配置将替换当前已保存的配置，是否继续？"),
                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    // 文件选择和确认期间程序可能开始运行，保存导入配置前再次检查运行状态。
    if (!page || answer != QMessageBox::Yes || !importAllowed(root) || !page)
        return;
    const QJsonObject imported = document.object();
    // 参数名称、所属页面或寄存器配置不符合要求时保留原配置。
    if (!registersMatch(root, imported, references)) {
        QMessageBox::warning(root, QObject::tr("导入配置"),
                             QObject::tr("配置的参数名称、所属页面或寄存器配置不符合要求，未导入。"));
        return;
    }
    // 全部检查通过后保存配置，保存成功后立即刷新页面。
    if (!SettingsStore::write(imported)) {
        QMessageBox::warning(root, QObject::tr("导入配置"), QObject::tr("配置保存失败，未导入。"));
        return;
    }
    reloadSettings(root);
    if (!page)
        return;
    const auto sync = root->findChild<RecipeSync *>();
    QString syncError;
    if (sync != nullptr && sync->synchronize(syncError)) {
        if (page)
            QMessageBox::information(root, QObject::tr("导入配置"),
                                     QObject::tr("配置已导入，控制器已确认保存完整配方集合。"));
    } else if (page) {
        if (sync == nullptr)
            syncError = QObject::tr("配方同步对象不可用。");
        QMessageBox::warning(root, QObject::tr("配方同步未完成"),
                             QObject::tr("配置已保存在示教器本地，控制器完整配方同步尚未确认。\n%1")
                                 .arg(syncError));
    }
}
