/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file TorqueSettingPage.cpp
 * @brief 扭矩设置页面。
 */

#include "TorqueSettingPage.h"
#include "core/ui/LabelColumnAligner.h"
#include "core/ui/SettingsEditor.h"
#include "core/ui/WidgetFactory.h"
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>

namespace xplugin {
TorqueSettingPage::TorqueSettingPage(RecipeSync *recipeSync, QWidget *parent)
    : ProcessPage(TorqueSetting, recipeSync, parent)
{
    auto *pnl_torque_set = this;
    pnl_torque_set->setObjectName(QStringLiteral("pnl_torque_set"));

    auto *torqueSetLayout = new QVBoxLayout(pnl_torque_set);
    const QVector<FormRow> torqueRows = {
        {"tooth_feed_speed", tr("认牙速度")},
        {"tooth_feed_angle", tr("认牙角度")},
        {"tighten_speed", tr("拧紧速度")},
        {"tighten_angle", tr("拧紧角度")},
        {"approach_speed", tr("接近速度")},
        {"initial_torque", tr("初始扭矩")},
        {"target_torque", tr("目标扭矩")},
        {"float_lock_angle", tr("浮锁角度")},
        {"strip_angle", tr("滑牙角度")},
        {"reverse_angle", tr("反转角度")},
        {"reverse_speed", tr("反转速度")},
        {"torque_compensation", tr("扭矩补偿")},
        {"force_speed_ratio", tr("力速比(1-3)")},
        {"self_tapping_mode", tr("自攻/机制(0-1)")},
        {"coefficient_correction", tr("系数修正")},
    };

    // 扭矩工艺号和扭矩参数使用相同的标签列宽，使输入区域对齐。
    QStringList torqueLabelTexts;
    torqueLabelTexts << tr("扭矩工艺号");
    for (const FormRow &row : torqueRows) {
        torqueLabelTexts << row.text;
    }
    const int torqueLabelWidth = screw::labelColumnWidth(pnl_torque_set, torqueLabelTexts);
    pnl_torque_set->installEventFilter(new LabelColumnAligner(pnl_torque_set));

    auto *row_process_no = new QWidget(pnl_torque_set);
    row_process_no->setObjectName(QStringLiteral("row_process_no"));
    auto *processRowLayout = screw::createNavRow(row_process_no);
    auto *lbl_torque_process_no = new QLabel(tr("扭矩工艺号"), row_process_no);
    lbl_torque_process_no->setObjectName(QStringLiteral("lbl_torque_process_no"));
    lbl_torque_process_no->setMinimumWidth(torqueLabelWidth);
    processRowLayout->addWidget(lbl_torque_process_no);
    auto *combo_torque_process_no = new QComboBox(row_process_no);
    combo_torque_process_no->setObjectName(QStringLiteral("combo_torque_process_no"));
    combo_torque_process_no->setProperty(screw::kSettingsSelectorProperty, true);
    combo_torque_process_no->setEditable(true);
    combo_torque_process_no->setInsertPolicy(QComboBox::NoInsert);
    for (int processNo = 1; processNo <= 30; ++processNo) {
        combo_torque_process_no->addItem(QString::number(processNo));
    }
    processRowLayout->addWidget(combo_torque_process_no, 1);
    torqueSetLayout->addWidget(row_process_no);

    auto *grp_torque = new QGroupBox(QString(), pnl_torque_set);
    grp_torque->setObjectName(QStringLiteral("grp_torque"));
    grp_torque->setProperty("func", "no_title");
    grp_torque->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *torqueGroupLayout = new QVBoxLayout(grp_torque);
    for (const FormRow &row : torqueRows) {
        const auto paramRow = screw::createParamRow(grp_torque, row.key, row.text, torqueLabelWidth);
        torqueGroupLayout->addWidget(paramRow.widget);
    }
    torqueSetLayout->addWidget(grp_torque);
    torqueSetLayout->addStretch();

    finishSetup(combo_torque_process_no);
}

} // namespace xplugin
