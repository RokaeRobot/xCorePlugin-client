/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PresetSectionPage.cpp
 * @brief 预设分类页面。
 */

#include "PresetSectionPage.h"
#include "core/ui/EditSaveControls.h"
#include "core/ui/FormPageBuilder.h"
#include "core/controller/RegisterIo.h"
#include "core/settings/SettingsStore.h"
#include "core/ui/SettingsEditor.h"
#include "pages/common/PageFieldWidgets.h"
#include "interface/interfacemanager.h"
#include <QPointer>
#include <QVariant>

namespace {

// 寄存器名称和数值均已填写的参数才参与设备写入。
bool hasRegisterAndValue(const FieldCell &field)
{
    return !field.registerName.trimmed().isEmpty() && !field.value.trimmed().isEmpty();
}

// 预设按名称和值均完整的参数逐项写入；单项失败继续统计，不提供事务回滚。
screw::RegisterWriteResult writeFilledParameters(const PageFields &fields)
{
    screw::RegisterWriteResult result;
    for (const FieldCell &field : fields.fields) {
        if (!hasRegisterAndValue(field))
            continue;
        if (RegisterIo::writeValue(field.registerName, field.value))
            ++result.written;
        else
            ++result.failed;
    }
    return result;
}

} // namespace

namespace xplugin {
PresetSectionPage::PresetSectionPage(const QString &section, const char *pageName, const char *formName,
                                     const QVector<FormGroup> &groups, QWidget *parent)
    : QWidget(parent), m_section(section)
{
    setObjectName(QString::fromLatin1(pageName));
    setProperty(screw::kSettingsSectionProperty, section);
    screw::buildFormContent(this, formName, groups, false);
    screw::bindSettingsRestore(this, [this]() { restore(); });
}

QWidget *PresetSectionPage::createSaveControls(QWidget *parent)
{
    return screw::createEditSaveControls(parent, this, QString(), [this]() { return save(); });
}

void PresetSectionPage::restore()
{
    screw::applyPageFields(this, screw::SettingsStore::loadPage(m_section));
}

bool PresetSectionPage::hasSavedParameters() const
{
    const PageFields saved = screw::SettingsStore::loadPage(m_section);
    for (const FieldCell &field : saved.fields) {
        if (hasRegisterAndValue(field))
            return true;
    }
    return false;
}

screw::RegisterWriteResult PresetSectionPage::writeSavedParameters() const
{
    return writeFilledParameters(screw::SettingsStore::loadPage(m_section));
}

screw::SaveResult PresetSectionPage::save()
{
    using screw::SaveStatus;
    const PageFields fields = screw::collectPageFields(this);
    if (!screw::SettingsStore::savePage(m_section, fields))
        return {SaveStatus::NotSaved, {}};

    // 本地文件已保存，设备写入失败时仍保留本地保存记录。
    screw::markSettingsSaved(this);
    const QPointer<PresetSectionPage> pageGuard(this);
    const auto count = writeFilledParameters(fields);
    if (count.failed > 0) {
        return {SaveStatus::RegisterWriteIncomplete,
                QObject::tr("设置已保存在示教器本地，写入锁丝机未完成：成功 %1 项，失败 %2 项。"
                            "请检查连接和寄存器配置后重试。")
                    .arg(count.written)
                    .arg(count.failed)};
    }
    if (!pageGuard)
        return {SaveStatus::Saved, {}};
    if (count.written == 0) {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Info,
                          QObject::tr("已保存到示教器，没有可写入锁丝机的数值"));
    } else {
        xplugin::ThrowLog(QString::fromLatin1(PLUGIN_NAME), xplugin::Info,
                          QObject::tr("已保存到示教器，并写入锁丝机 %1 项").arg(count.written));
    }
    return {SaveStatus::Saved, {}};
}
} // namespace xplugin
