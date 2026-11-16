/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SaveFeedback.cpp
 * @brief 保存结果提示。
 *
 * 成功提示在 0.5 秒后关闭，失败提示由用户关闭。
 */

#include "SaveFeedback.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>

namespace screw {

namespace {
class FailureDialog : public QDialog
{
public:
    FailureDialog(QWidget *parent, const QString &detail, const QString &title) : QDialog(parent)
    {
        setObjectName(QStringLiteral("save_failure_dialog"));
        setWindowTitle(title.isEmpty() ? tr("保存失败") : title);
        setWindowFlag(Qt::WindowContextHelpButtonHint, false);
        setWindowFlag(Qt::WindowCloseButtonHint, true);

        // 显示页面返回的失败原因，帮助用户区分客户端本地文件保存失败和设备寄存器写入失败。
        auto *layout = new QVBoxLayout(this);
        auto *label = new QLabel(detail, this);
        label->setObjectName(QStringLiteral("lbl_save_failure"));
        label->setTextFormat(Qt::PlainText);
        label->setWordWrap(true);
        layout->addWidget(label);

        auto *buttons = new QDialogButtonBox(this);
        auto *ok = buttons->addButton(tr("确定"), QDialogButtonBox::AcceptRole);
        ok->setObjectName(QStringLiteral("btn_save_failure_ok"));
        ok->setDefault(true);
        layout->addWidget(buttons);
        connect(ok, &QPushButton::clicked, this, &QDialog::accept);
        setMinimumWidth(360);
        setMaximumWidth(640);
    }

};
} // namespace

QDialog *showSaveSuccess(QWidget *parent)
{
    auto *message = new QDialog(parent);
    message->setObjectName(QStringLiteral("save_success_dialog"));
    message->setWindowTitle(QObject::tr("保存成功"));
    message->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    auto *layout = new QVBoxLayout(message);
    auto *label = new QLabel(QObject::tr("保存成功"), message);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    message->setMinimumWidth(180);

    // 使用非阻塞的窗口模态提示，阻止所属窗口输入，并在关闭时自动销毁。
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->setWindowModality(Qt::WindowModal);
    message->show();

    // 成功提示在 0.5 秒后自动关闭，关闭后可继续操作页面。
    auto *timer = new QTimer(message);
    timer->setObjectName(QStringLiteral("save_success_timer"));
    timer->setSingleShot(true);
    timer->setTimerType(Qt::PreciseTimer);
    QObject::connect(timer, &QTimer::timeout, message, &QDialog::accept);
    timer->start(500);
    return message;
}

QDialog *showSaveFailure(QWidget *parent, const QString &detail, const QString &title)
{
    auto *message = new FailureDialog(parent, detail, title);
    message->setAttribute(Qt::WA_DeleteOnClose);
    message->setWindowModality(Qt::WindowModal);
    message->show();
    return message;
}

} // namespace screw
