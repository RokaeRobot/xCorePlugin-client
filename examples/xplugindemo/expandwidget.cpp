#include "expandwidget.h"

#include "interface/interfacemanager.h"
#include "log/logmanager.h"
#include "pages/common/apiresultpanel.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

ExpandWidget::ExpandWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("ExpandWidget"));
    setMinimumWidth(280);
    setMinimumHeight(320);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 14, 14, 14);
    root->setSpacing(12);

    QLabel *title = new QLabel(tr("Right Expand"), this);
    title->setObjectName(QStringLiteral("expandTitle"));
    root->addWidget(title);

    QLabel *hint = new QLabel(tr("Demo side panel actions"), this);
    hint->setObjectName(QStringLiteral("expandHint"));
    hint->setWordWrap(true);
    root->addWidget(hint);

    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    QPushButton *btnDemo = addActionBtn(tr("Demo Action"), false);
    connect(btnDemo, &QPushButton::clicked, this, &ExpandWidget::onDemoClicked);
    grid->addWidget(btnDemo, 0, 0);

    QPushButton *btnClose = addActionBtn(tr("Close Panel"), true);
    connect(btnClose, &QPushButton::clicked, this, &ExpandWidget::onCloseClicked);
    grid->addWidget(btnClose, 0, 1);

    QPushButton *btnA = addActionBtn(tr("Action A"));
    QPushButton *btnB = addActionBtn(tr("Action B"));
    QPushButton *btnC = addActionBtn(tr("Action C"));
    QPushButton *btnD = addActionBtn(tr("Action D"));
    connect(btnA, &QPushButton::clicked, this, &ExpandWidget::onActionClicked);
    connect(btnB, &QPushButton::clicked, this, &ExpandWidget::onActionClicked);
    connect(btnC, &QPushButton::clicked, this, &ExpandWidget::onActionClicked);
    connect(btnD, &QPushButton::clicked, this, &ExpandWidget::onActionClicked);
    grid->addWidget(btnA, 1, 0);
    grid->addWidget(btnB, 1, 1);
    grid->addWidget(btnC, 2, 0);
    grid->addWidget(btnD, 2, 1);

    root->addLayout(grid);
    root->addStretch(1);

    applyStyle();
}

QPushButton *ExpandWidget::addActionBtn(const QString &text, bool primary)
{
    QPushButton *btn = new QPushButton(text, this);
    btn->setObjectName(primary ? QStringLiteral("expandBtnPrimary")
                                : QStringLiteral("expandBtn"));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(36);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return btn;
}

void ExpandWidget::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QWidget#ExpandWidget {"
        "  background-color: #F5F5F5;"
        "  font: 9pt \"Microsoft YaHei UI\";"
        "}"
        "QLabel#expandTitle {"
        "  color: #091C36;"
        "  font: bold 12pt \"Microsoft YaHei UI\";"
        "  padding: 2px 0 4px 0;"
        "}"
        "QLabel#expandHint {"
        "  color: #567C9F;"
        "  font: 9pt \"Microsoft YaHei UI\";"
        "}"
        "QPushButton#expandBtn {"
        "  border: none;"
        "  border-radius: 6px;"
        "  color: #3B3B3B;"
        "  background-color: #D3DCE6;"
        "  min-height: 36px;"
        "  padding: 0 12px;"
        "  font: 9pt \"Microsoft YaHei UI\";"
        "}"
        "QPushButton#expandBtn:hover { background-color: #c5d0dc; }"
        "QPushButton#expandBtn:pressed {"
        "  background-color: #224D75;"
        "  color: #ffffff;"
        "}"
        "QPushButton#expandBtnPrimary {"
        "  border: none;"
        "  border-radius: 6px;"
        "  color: #ffffff;"
        "  background-color: #224D75;"
        "  min-height: 36px;"
        "  padding: 0 12px;"
        "  font: 9pt \"Microsoft YaHei UI\";"
        "}"
        "QPushButton#expandBtnPrimary:hover { background-color: #2a5f8f; }"
        "QPushButton#expandBtnPrimary:pressed {"
        "  background-color: #D9E6F2;"
        "  color: #091C36;"
        "}"
    ));
}

void ExpandWidget::reportClick(const QString &action, const QString &detail)
{
    ILOG << "ExpandWidget:" << action;
    ApiResultPanel::appendToActive(
        QStringLiteral("ExpandWidget::%1").arg(action),
        true,
        detail.isEmpty() ? tr("Right expand button clicked.") : detail);
}

void ExpandWidget::onDemoClicked()
{
    reportClick(QStringLiteral("DemoAction"), tr("Demo Action executed."));
}

void ExpandWidget::onCloseClicked()
{
    reportClick(QStringLiteral("ClosePanel"), tr("Close right expand panel."));
    xplugin::SwitchRightExpandWidgte();
}

void ExpandWidget::onActionClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    const QString name = btn ? btn->text() : QStringLiteral("Action");
    reportClick(name, tr("Action button clicked: %1").arg(name));
}
