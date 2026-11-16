/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file CurvePage.cpp
 * @brief 扭矩曲线页面。
 *
 * 配置曲线数据来源，并显示读取到的角度和扭矩数据。
 */

#include "CurvePage.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPointF>
#include <QPushButton>
#include <QSizePolicy>
#include <QStringList>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include <cmath>

#include "core/ui/EditSaveControls.h"
#include "core/ui/LabelColumnAligner.h"
#include "pages/common/PageFieldWidgets.h"
#include "core/controller/RegisterIo.h"
#include "core/ui/SettingsEditor.h"
#include "core/ui/WidgetFactory.h"
#include "TorqueChartWidget.h"

namespace {
constexpr int kMaxCurveGroups = 200; // 扭矩区和角度区各保存 200 个数据。
}

xplugin::CurvePage::CurvePage(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("pnl_torque_curve_page"));

    auto *curvePageLayout = new QVBoxLayout(this);

    auto *curveBodyLayout = new QHBoxLayout;

    // 上方参数表单配置组数和曲线数据的寄存器来源。
    auto *pnl_torque_curve_form = new QGroupBox(this);
    pnl_torque_curve_form->setObjectName(QStringLiteral("pnl_torque_curve_form"));
    pnl_torque_curve_form->setProperty(screw::kSettingsSectionProperty, QStringLiteral("curve"));
    pnl_torque_curve_form->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto *curveFormLayout = new QVBoxLayout(pnl_torque_curve_form);
    const QStringList curveLabelTexts = {tr("组数"), tr("扭矩角度数据")};
    const int curveLabelWidth = screw::labelColumnWidth(pnl_torque_curve_form, curveLabelTexts);

    pnl_torque_curve_form->installEventFilter(new LabelColumnAligner(pnl_torque_curve_form));

    // 组数行保留只读数值框，显示组数寄存器的实时值。
    const auto groupRow =
        screw::createParamRow(pnl_torque_curve_form, "curve_group_count", tr("组数"), curveLabelWidth, true);
    m_groupCombo = groupRow.registerCombo;
    m_groupEdit = groupRow.valueEdit;
    curveFormLayout->addWidget(groupRow.widget);

    // 曲线数据行只提供寄存器选择，不创建数值输入框。
    auto *dataRow = new QWidget(pnl_torque_curve_form);
    dataRow->setObjectName(QStringLiteral("row_curve_data"));
    dataRow->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto *dataLayout = screw::createNavRow(dataRow);
    auto *dataLabel = new QLabel(tr("扭矩角度数据"), dataRow);
    dataLabel->setObjectName(QStringLiteral("lbl_curve_data"));
    dataLabel->setMinimumWidth(curveLabelWidth);
    dataLayout->addWidget(dataLabel);

    m_dataCombo = new QComboBox(dataRow);
    m_dataCombo->setObjectName(QStringLiteral("combo_curve_data"));
    m_dataCombo->setEditable(true);
    screw::watchRegisterCombo(m_dataCombo);
    dataLayout->addWidget(m_dataCombo);
    dataLayout->addStretch();
    curveFormLayout->addWidget(dataRow);

    curveBodyLayout->addWidget(pnl_torque_curve_form, 1);

    // 右侧操作按钮只保存曲线页面的来源配置，不将实时曲线数值写回设备。
    auto *pnl_curve_save = new QWidget(this);
    pnl_curve_save->setObjectName(QStringLiteral("pnl_curve_save"));

    auto *curveSaveLayout = new QVBoxLayout(pnl_curve_save);
    curveSaveLayout->addStretch();
    curveSaveLayout->addWidget(screw::createEditSaveControls(pnl_curve_save, pnl_torque_curve_form), 0,
                               Qt::AlignRight);
    curveBodyLayout->addWidget(pnl_curve_save, 0, Qt::AlignBottom);

    curvePageLayout->addLayout(curveBodyLayout);

    // 下方曲线区域显示读取后的角度和扭矩数据。
    m_chart = new TorqueChartWidget(tr("角度"), tr("扭矩"), this);
    m_chart->setObjectName(QStringLiteral("lbl_torque_chart"));
    curvePageLayout->addWidget(m_chart);
    curvePageLayout->addStretch();

    const auto sourceChanged = [this]() {
        clearCurve();
        m_chart->setStatusText(tr("数据来源已切换，等待有效数据"));
    };
    connect(m_groupCombo, &QComboBox::currentTextChanged, this, sourceChanged);
    connect(m_dataCombo, &QComboBox::currentTextChanged, this, sourceChanged);

    // 恢复配置后比较组数寄存器和曲线数据寄存器的名称，名称变化时清除原数据来源的曲线。
    screw::bindSettingsRestore(pnl_torque_curve_form, [this, pnl_torque_curve_form, sourceChanged]() {
        const QString group = m_groupCombo->currentText();
        const QString data = m_dataCombo->currentText();
        screw::restorePageSettings(pnl_torque_curve_form);
        if (group != m_groupCombo->currentText() || data != m_dataCombo->currentText())
            sourceChanged();
    });

    RegisterIo::attachLiveRefresh(
        this, [this]() { return QStringList{m_groupCombo->currentText(), m_dataCombo->currentText()}; },
        [this](RegisterIo::ReadResult result) { refreshCurve(result); });
}

void xplugin::CurvePage::clearCurve()
{
    if (!m_groupEdit->text().isEmpty())
        m_groupEdit->clear();
    m_chart->setPoints({});
    m_hasValidData = false;
    m_failureTimer.invalidate();
}

void xplugin::CurvePage::showReadFailure(const QString &reason)
{
    // 连续三秒没有取得有效数据时更换提示，期间保留上次有效显示。
    if (!m_failureTimer.isValid())
        m_failureTimer.start();
    QString text = reason;
    if (m_failureTimer.elapsed() >= 3000)
        text = tr("数据更新已中断\n") + reason;
    text += m_hasValidData ? tr("\n当前保留上次有效数据") : tr("\n尚未取得有效曲线数据");
    m_chart->setStatusText(text);
}

void xplugin::CurvePage::refreshCurve(RegisterIo::ReadResult result)
{
    if (result == RegisterIo::ReadResult::Busy)
        return;
    if (result == RegisterIo::ReadResult::Disconnected) {
        clearCurve();
        m_chart->setStatusText(tr("控制器已断开，等待重新连接"), true);
        return;
    }
    if (result == RegisterIo::ReadResult::Failed) {
        showReadFailure(tr("读取失败，正在重试"));
        return;
    }

    const QString groupRegister = m_groupCombo->currentText().trimmed();
    const QString dataRegister = m_dataCombo->currentText().trimmed();
    if (groupRegister.isEmpty()) {
        m_groupEdit->clear();
        m_failureTimer.invalidate();
        m_chart->setStatusText(tr("请选择组数和扭矩角度数据寄存器"));
        return;
    }

    const QString groupValue = RegisterIo::currentValue(groupRegister);
    // 组数显示独立于曲线校验，曲线数据不足时仍显示组数寄存器的读取结果。
    if (m_groupEdit->text() != groupValue)
        m_groupEdit->setText(groupValue);
    bool ok = false;
    int count = groupValue.trimmed().toInt(&ok);
    if (!ok || count < 0) {
        showReadFailure(groupValue.isEmpty()
                            ? tr("未读到组数寄存器 %1 的数值，正在重新读取").arg(groupRegister)
                            : tr("组数寄存器 %1 的值“%2”不是有效的非负整数")
                                  .arg(groupRegister, groupValue));
        return;
    }

    if (dataRegister.isEmpty()) {
        m_failureTimer.invalidate();
        m_chart->setStatusText(tr("请选择扭矩角度数据寄存器"));
        return;
    }

    // 零组清空曲线；非零组最多取 200 对，全部有效后整体替换，校验失败保留上次有效曲线。
    QVector<QPointF> points;
    if (count > 0) {
        count = qMin(count, kMaxCurveGroups);
        const QStringList values = RegisterIo::valuesFrom(dataRegister, kMaxCurveGroups + count);
        if (values.size() < kMaxCurveGroups + count) {
            showReadFailure(tr("曲线数据不足：需要 %1 个元素，已读到 %2 个元素。\n"
                               "请检查数据寄存器，完整数组应包含 400 个元素。")
                                .arg(kMaxCurveGroups + count).arg(values.size()));
            return;
        }
        points.reserve(count);
        // 前 200 项为放大 100 倍的扭矩，后 200 项为角度，相同序号组成一个曲线点。
        for (int index = 0; index < count; ++index) {
            bool torqueOk = false;
            bool angleOk = false;
            const double torque = values.at(index).toDouble(&torqueOk) / 100.0;
            const double angle = values.at(kMaxCurveGroups + index).toDouble(&angleOk);
            if (!torqueOk || !angleOk || !std::isfinite(torque) || !std::isfinite(angle)) {
                showReadFailure(tr("第 %1 组的扭矩或角度数据无效，正在重新读取").arg(index + 1));
                return;
            }
            points.append(QPointF(angle, torque));
        }
    }

    m_chart->setPoints(points);
    m_hasValidData = true;
    m_failureTimer.invalidate();
    m_chart->setStatusText({});
}
