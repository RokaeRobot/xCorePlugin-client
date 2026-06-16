#include "rlcustominsertwidget.h"
#include "ui_rlcustominsertwidget.h"
#include <QDebug>

using namespace xplugin;
RLCustomInsertWidget::RLCustomInsertWidget(const QString &type, QWidget *parent) :
    RLContentBase(parent),
    ui(new Ui::RLCustomInsertWidget)
{
    ui->setupUi(this);
    setType(type);
}

RLCustomInsertWidget::~RLCustomInsertWidget()
{
    delete ui;
}

void RLCustomInsertWidget::FillWith(const QString &str)
{
    qDebug() << "RLCustomInsertWidget::FillWith : " << m_type << " : " << str;
}

QString RLCustomInsertWidget::ToString()
{
    QString lab1 = ui->line1->text();
    QString lab2 = ui->line2->text();
    return m_type + QString(" %1,%2").arg(lab1).arg(lab2);
}

bool RLCustomInsertWidget::CheckInsert()
{
    return true;
}
