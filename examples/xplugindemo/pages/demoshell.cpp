#include "demoshell.h"

#include "common/demostyle.h"
#include "xplugin/pagehmishell.h"
#include "xplugin/pageioregister.h"
#include "xplugin/pagenetworkeventlog.h"
#include "xplugin/pageprojectpointvar.h"
#include "xplugin/pagerobotstatus.h"
#include "xplugin/pagexservicerc.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

DemoShell::DemoShell(QWidget *parent)
    : QWidget(parent)
    , m_nav(new QListWidget(this))
    , m_stack(new QStackedWidget(this))
{
    setObjectName(QStringLiteral("DemoShell"));
    m_nav->setObjectName(QStringLiteral("demoNavList"));
    m_stack->setObjectName(QStringLiteral("demoPageStack"));

    m_nav->setFixedWidth(268);
    m_nav->setSpacing(2);
    m_nav->setFocusPolicy(Qt::NoFocus);
    m_nav->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_nav->setTextElideMode(Qt::ElideNone);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_nav);
    lay->addWidget(m_stack, 1);

    addPage(tr("UI Layer"), new PageHmiShell(this));
    addPage(tr("Robot Status"), new PageRobotStatus(this));
    addPage(tr("Project / Point / Variable"), new PageProjectPointVar(this));
    addPage(tr("IO / Register"), new PageIoRegister(this));
    addPage(tr("XService / RC"), new PageXServiceRc(this));
    addPage(tr("Network / Event / Log"), new PageNetworkEventLog(this));

    demoApplyXmateStyle(this);

    connect(m_nav, &QListWidget::currentRowChanged, m_stack, &QStackedWidget::setCurrentIndex);
    if (m_nav->count() > 0) {
        m_nav->setCurrentRow(0);
    }
}

void DemoShell::addPage(const QString &title, QWidget *page)
{
    page->setObjectName(QStringLiteral("demoPageRoot"));
    m_nav->addItem(title);
    m_stack->addWidget(page);
}
