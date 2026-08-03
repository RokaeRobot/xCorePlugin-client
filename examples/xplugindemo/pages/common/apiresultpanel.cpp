#include "apiresultpanel.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QStyle>
#include <QVariant>
#include <QVBoxLayout>

ApiResultPanel *ApiResultPanel::s_active = nullptr;

ApiResultPanel::ApiResultPanel(QWidget *parent)
    : QWidget(parent)
    , m_edit(new QPlainTextEdit(this))
    , m_btnClear(new QPushButton(tr("Clear"), this))
{
    setObjectName(QStringLiteral("ApiResultPanel"));

    m_edit->setObjectName(QStringLiteral("demoResultEdit"));
    m_edit->setReadOnly(true);
    m_edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_edit->setUndoRedoEnabled(false);

    QLabel *title = new QLabel(tr("Result"), this);
    title->setProperty("func", QVariant(QStringLiteral("title_h2")));

    m_btnClear->setProperty("func", QVariant(QStringLiteral("primary")));
    m_btnClear->setMinimumWidth(80);
    m_btnClear->setMaximumWidth(100);
    m_btnClear->setCursor(Qt::PointingHandCursor);
    if (m_btnClear->style()) {
        m_btnClear->style()->unpolish(m_btnClear);
        m_btnClear->style()->polish(m_btnClear);
    }

    QHBoxLayout *top = new QHBoxLayout;
    top->setContentsMargins(14, 12, 14, 6);
    top->setSpacing(8);
    top->addWidget(title);
    top->addStretch(1);
    top->addWidget(m_btnClear);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(4, 4, 4, 10);
    lay->setSpacing(4);
    lay->addLayout(top);
    lay->addWidget(m_edit, 1);

    connect(m_btnClear, &QPushButton::clicked, this, &ApiResultPanel::clear);
    setAsActive();
}

ApiResultPanel::~ApiResultPanel()
{
    if (s_active == this) {
        s_active = nullptr;
    }
}

void ApiResultPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setAsActive();
}

void ApiResultPanel::setAsActive()
{
    s_active = this;
}

ApiResultPanel *ApiResultPanel::active()
{
    return s_active;
}

void ApiResultPanel::appendToActive(const QString &api, bool ok, const QString &detail)
{
    if (s_active) {
        s_active->append(api, ok, detail);
    }
}

void ApiResultPanel::appendInfoToActive(const QString &msg)
{
    if (s_active) {
        s_active->appendInfo(msg);
    }
}

void ApiResultPanel::clear()
{
    m_edit->clear();
}

void ApiResultPanel::append(const QString &api, bool ok, const QString &detail)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"));
    const QString tag = ok ? QStringLiteral("[OK]") : QStringLiteral("[FAIL]");
    m_edit->appendPlainText(QStringLiteral("%1  %2  %3").arg(ts, tag, api));
    if (!detail.isEmpty()) {
        m_edit->appendPlainText(QStringLiteral("    %1").arg(detail));
    }
    m_edit->appendPlainText(QString());
}

void ApiResultPanel::appendInfo(const QString &msg)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"));
    m_edit->appendPlainText(QStringLiteral("%1  [INFO]  %2").arg(ts, msg));
    m_edit->appendPlainText(QString());
}
