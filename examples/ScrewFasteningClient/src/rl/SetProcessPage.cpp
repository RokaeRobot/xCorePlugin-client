/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SetProcessPage.cpp
 * @brief SetProcess 指令编辑页面。
 *
 * 选择工艺类型和配方号，完成指令文字与页面选项之间的转换。
 */

#include "SetProcessPage.h"

#include "rl/ProcessType.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "core/controller/RunLock.h"

namespace {
// 读取指令编辑页面选中的工艺类型。
xplugin::ProcessType selectedType(const QComboBox *combo)
{
    return static_cast<xplugin::ProcessType>(combo->currentData().toInt());
}

} // namespace

SetProcessPage::SetProcessPage(QWidget *parent) : xplugin::RLContentBase(parent)
{
    auto *layout = new QVBoxLayout(this);

    // 用协议名称显示工艺选项，用枚举值识别选中的工艺。
    auto *typeLabel = new QLabel(tr("类型"), this);
    m_type = new QComboBox(this);
    for (int index = 0; index < xplugin::ProcessTypeTool::count(); ++index) {
        const xplugin::ProcessType type = xplugin::ProcessTypeTool::at(index);
        m_type->addItem(xplugin::ProcessTypeTool::name(type), static_cast<int>(type));
    }

    // 配方号提供 1～30 的选项，吸式和吹式工艺不使用配方号。
    auto *indexLabel = new QLabel(tr("配方号"), this);
    m_index = new QComboBox(this);
    for (int number = 1; number <= 30; ++number) {
        m_index->addItem(QString::number(number));
    }
    layout->addWidget(typeLabel);
    layout->addWidget(m_type);
    layout->addWidget(indexLabel);
    layout->addWidget(m_index);

    // 切换工艺类型后更新配方号是否可选；程序运行时限制修改。
    connect(m_type, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [this](int) { applyInputLock(screw::RunLock::instance().isLocked()); });

    screw::RunLock::instance().subscribe(this, [this](bool locked) { applyInputLock(locked); });
}

void SetProcessPage::FillWith(const QString &str)
{
    // 缺少参数括号或带引号的工艺名称时，保留当前页面选择。
    const int open = str.indexOf(QLatin1Char('('));
    const int close = str.lastIndexOf(QLatin1Char(')'));
    if (open < 0 || close <= open) {
        return;
    }
    const QStringList parts = str.mid(open + 1, close - open - 1).split(QLatin1Char(','));
    if (parts.isEmpty()) {
        return;
    }
    const QString quotedName = parts.at(0).trimmed();
    if (quotedName.size() < 2 || !quotedName.startsWith(QLatin1Char('"')) ||
        !quotedName.endsWith(QLatin1Char('"'))) {
        return;
    }

    const QString name = quotedName.mid(1, quotedName.size() - 2);
    const xplugin::ProcessType type = xplugin::ProcessTypeTool::fromName(name);
    if (type != xplugin::InvalidProcess) {
        const int typeIndex = m_type->findData(static_cast<int>(type));
        if (typeIndex >= 0) {
            m_type->setCurrentIndex(typeIndex);
        }
    }

    // 配方号只有在 1～30 的现有选项中匹配成功时才更新。
    if (parts.size() > 1) {
        bool recipeValid = false;
        const int recipe = parts.at(1).trimmed().toInt(&recipeValid);
        const int recipeIndex = recipeValid ? m_index->findText(QString::number(recipe)) : -1;
        if (recipeIndex >= 0) {
            m_index->setCurrentIndex(recipeIndex);
        }
    }
}

QString SetProcessPage::ToString()
{
    return QStringLiteral("SetProcess(\"%1\",%2);")
        .arg(xplugin::ProcessTypeTool::name(selectedType(m_type)), m_index->currentText());
}

bool SetProcessPage::CheckInsert()
{
    return !screw::RunLock::instance().isLocked() &&
           m_type->findData(m_type->currentData()) == m_type->currentIndex();
}

bool SetProcessPage::CheckReplace()
{
    return CheckInsert();
}

void SetProcessPage::applyInputLock(bool locked)
{
    m_type->setEnabled(!locked);
    m_index->setEnabled(!locked && xplugin::ProcessTypeTool::usesRecipeIndex(selectedType(m_type)));
}
