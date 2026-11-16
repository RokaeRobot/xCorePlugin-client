/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FeedbackPage.cpp
 * @brief 指令反馈页面。
 *
 * 配置反馈寄存器并刷新反馈数值。
 */

#include "FeedbackPage.h"
#include "core/ui/SettingsEditor.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include "core/ui/FormPageBuilder.h"
#include "core/ui/FormSpec.h"
#include "core/ui/LabelColumnAligner.h"
#include "core/ui/NavBar.h"
#include "core/controller/RegisterIo.h"
#include "core/ui/StyleTokens.h"
#include "core/ui/WidgetFactory.h"

xplugin::FeedbackPage::FeedbackPage(QWidget *parent) : QWidget(parent)
{
    setProperty(screw::kSettingsSectionProperty, QStringLiteral("feedback"));
    // 按反馈项目建立页面，只保存每项反馈对应的寄存器来源。
    const QVector<FormGroup> feedbackGroups = {
        {"feedback",
         QString(),
         {
             {"feedback_process_cmd", tr("运行过程命令反馈")},
             {"feedback_clear_ctrl", tr("清零控制反馈")},
             {"feedback_torque_check", tr("扭矩校验控制反馈")},
             {"feedback_motion_cmd", tr("运动指令控制反馈")},
             {"feedback_task_chain", tr("任务链执行反馈")},
         }},
    };
    setObjectName(QStringLiteral("pnl_feedback_page"));
    const auto rows = screw::buildFormContent(this, "pnl_feedback_page_form", feedbackGroups, true, true);

    // 反馈值仅用于显示，保存时只记录寄存器选择。
    RegisterIo::attachLiveRefresh(this, [rows]() {
        QStringList names;
        for (const auto &widgets : rows) {
            if (widgets.registerCombo != nullptr)
                names.append(widgets.registerCombo->currentText());
        }
        return names;
    }, [rows]() {
        for (const auto &widgets : rows) {
            QComboBox *combo = widgets.registerCombo;
            QLineEdit *edit = widgets.valueEdit;
            if (combo == nullptr || edit == nullptr) {
                continue;
            }
            const QString name = combo->currentText().trimmed();
            const QString value = name.isEmpty() ? QString() : RegisterIo::currentValue(name);
            if (edit->text() != value)
                edit->setText(value);
        }
    }, [rows]() {
        for (const auto &widgets : rows) {
            if (widgets.valueEdit != nullptr)
                widgets.valueEdit->clear();
        }
    });
}
