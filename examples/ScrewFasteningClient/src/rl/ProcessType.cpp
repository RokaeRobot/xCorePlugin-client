/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ProcessType.cpp
 * @brief 维护 SetProcess 工艺表并实现查询接口。
 */

#include "ProcessType.h"

namespace xplugin {

struct ProcessTypeTool::Entry
{
    ProcessType type;
    QString name;
    bool usesRecipeIndex;
};

int ProcessTypeTool::count()
{
    return static_cast<int>(entries().size());
}

ProcessType ProcessTypeTool::at(int index)
{
    if (index < 0 || index >= count()) {
        return InvalidProcess;
    }
    return entries().begin()[index].type;
}

QString ProcessTypeTool::name(ProcessType type)
{
    const Entry *entry = find(type);
    return entry != nullptr ? entry->name : QString();
}

ProcessType ProcessTypeTool::fromName(const QString &name)
{
    for (const Entry &entry : entries()) {
        if (entry.name == name) {
            return entry.type;
        }
    }
    return InvalidProcess;
}

bool ProcessTypeTool::usesRecipeIndex(ProcessType type)
{
    const Entry *entry = find(type);
    return entry != nullptr && entry->usesRecipeIndex;
}

// 工艺表在首次访问时初始化，供插件内的调用方共用，随插件卸载销毁。
// at() 按表的行顺序遍历，SetProcess 编辑页也按此顺序显示选项。
const std::initializer_list<ProcessTypeTool::Entry> &ProcessTypeTool::entries()
{
    static const std::initializer_list<Entry> table = {
        {TorqueSetting, QStringLiteral("TorqueSetting"), true},
        {SuctionProcess, QStringLiteral("SuctionProcess"), false},
        {BlowProcess, QStringLiteral("BlowProcess"), false},
        {ScrewPoint, QStringLiteral("ScrewPoint"), true},
        {IoOutput, QStringLiteral("IoOutput"), true},
        {IoWait, QStringLiteral("IoWait"), true},
    };
    return table;
}

const ProcessTypeTool::Entry *ProcessTypeTool::find(ProcessType type)
{
    for (const Entry &entry : entries()) {
        if (entry.type == type) {
            return &entry;
        }
    }
    return nullptr;
}

} // namespace xplugin
