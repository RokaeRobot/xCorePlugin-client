/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file StatusPage.cpp
 * @brief 状态读取页面。
 *
 * 按设备状态、螺丝报表和耗材使用情况显示寄存器数值。
 */

#include "StatusPage.h"
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

xplugin::StatusPage::StatusPage(QWidget *parent) : QWidget(parent)
{
    setProperty(screw::kSettingsSectionProperty, QStringLiteral("status"));
    // 状态按设备状态、单颗螺丝报表和耗材使用次数分组，数值只读。
    const QVector<FormGroup> statusGroups = {
        {"st_device",
         tr("设备状态"),
         {
             {"st_x_axis_pos", tr("X轴（推轴）当前位置")},
             {"st_x_axis_pos_3rd", tr("X轴（推轴）当前位置（第三位小数）")},
             {"st_run_state", tr("状态(0准备好,1运行,2暂停,3急停,4有错误)")},
             {"st_io_in_state", tr("IO输入状态(36)")},
             {"st_io_out_state", tr("IO输出状态(24)")},
             {"st_run_file", tr("运行文件 20字符占10个地址ASCII")},
             {"st_error_code", tr("错误代码")},
             {"st_screw_total", tr("螺丝锁付总颗数")},
             {"st_screw_ok", tr("螺丝OK颗数")},
             {"st_screw_float", tr("螺丝浮锁颗数")},
             {"st_screw_strip", tr("螺丝滑牙颗数")},
             {"st_feed_fail", tr("供料失败颗数")},
         }},
        {"st_screw_report",
         tr("单颗螺丝报表"),
         {
             {"st_report_x_axis_pos", tr("X轴（推轴）当前位置")},
             {"st_report_x_axis_pos_3rd", tr("X轴（推轴）当前位置（第三位小数）")},
             {"st_lock_state", tr("锁付状态")},
             {"st_screw_index", tr("螺丝序号")},
             {"st_lock_torque", tr("锁付扭矩")},
             {"st_lock_angle", tr("锁付角度")},
             {"st_lock_duration", tr("锁付时长")},
             {"st_min_angle", tr("最小角度")},
             {"st_max_angle", tr("最大角度")},
             {"st_min_torque", tr("最小扭矩")},
             {"st_max_torque", tr("最大扭矩")},
             {"st_torque_unit", tr("扭矩单位")},
         }},
        {"st_consumable",
         tr("耗材使用次数"),
         {
             {"st_bit_usage", tr("披头使用次数")},
             {"st_nozzle_usage", tr("夹嘴/吸嘴使用次数")},
             {"st_filter_usage", tr("滤芯使用次数")},
         }},
    };
    setObjectName(QStringLiteral("pnl_status_page"));
    const auto rows = screw::buildFormContent(this, "pnl_status_page_form", statusGroups, true, true);

    // 状态值仅用于显示，保存时只记录寄存器选择。
    RegisterIo::attachLiveRefresh(this, [rows]() {
        QStringList names;
        for (const auto &widgets : rows) {
            if (widgets.registerCombo != nullptr)
                names.append(widgets.registerCombo->currentText());
        }
        return names;
    }, [rows]() {
        // 单值状态直接使用对应寄存器的最近数值。
        const QStringList singleKeys = {
            QStringLiteral("st_x_axis_pos"),
            QStringLiteral("st_x_axis_pos_3rd"),
            QStringLiteral("st_run_state"),
            QStringLiteral("st_error_code"),
            QStringLiteral("st_screw_total"),
            QStringLiteral("st_screw_ok"),
            QStringLiteral("st_screw_float"),
            QStringLiteral("st_screw_strip"),
            QStringLiteral("st_feed_fail"),
            QStringLiteral("st_report_x_axis_pos"),
            QStringLiteral("st_report_x_axis_pos_3rd"),
            QStringLiteral("st_lock_state"),
            QStringLiteral("st_screw_index"),
            QStringLiteral("st_lock_torque"),
            QStringLiteral("st_lock_angle"),
            QStringLiteral("st_lock_duration"),
            QStringLiteral("st_min_angle"),
            QStringLiteral("st_max_angle"),
            QStringLiteral("st_min_torque"),
            QStringLiteral("st_max_torque"),
            QStringLiteral("st_torque_unit"),
            QStringLiteral("st_bit_usage"),
            QStringLiteral("st_nozzle_usage"),
            QStringLiteral("st_filter_usage"),
        };
        for (const QString &key : singleKeys) {
            const auto widgets = rows.value(key);
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

        // 输入、输出和文件名分别读取所选寄存器数组的前 36、24、10 项，不跨寄存器拼接。
        const struct Block
        {
            const char *key;
            int count;
            bool ascii;
        } blocks[] = {
            {"st_io_in_state", 36, false},
            {"st_io_out_state", 24, false},
            {"st_run_file", 10, true},
        };
        for (const Block &block : blocks) {
            const auto widgets = rows.value(QString::fromLatin1(block.key));
            QComboBox *combo = widgets.registerCombo;
            QLineEdit *edit = widgets.valueEdit;
            if (combo == nullptr || edit == nullptr) {
                continue;
            }
            if (combo->currentText().trimmed().isEmpty()) {
                if (!edit->text().isEmpty())
                    edit->clear();
                continue;
            }
            const QStringList values = RegisterIo::valuesFrom(combo->currentText().trimmed(), block.count);
            if (!block.ascii) {
                const QString text = values.join(QStringLiteral(","));
                if (edit->text() != text)
                    edit->setText(text);
                continue;
            }

            // 文件名每个数组元素编码两个 ASCII 字符，低字节在前、高字节在后。
            QString text;
            for (const QString &word : values) {
                bool ok = false;
                const int number = word.toInt(&ok);
                if (!ok) {
                    continue;
                }
                text.append(QChar(number & 0xFF));
                text.append(QChar((number >> 8) & 0xFF));
            }
            if (edit->text() != text)
                edit->setText(text);
        }
    }, [rows]() {
        for (const auto &widgets : rows) {
            if (widgets.valueEdit != nullptr)
                widgets.valueEdit->clear();
        }
    });
}
