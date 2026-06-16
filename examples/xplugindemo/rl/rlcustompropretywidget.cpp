#include "rlcustompropretywidget.h"
#include "ui_rlcustompropretywidget.h"
#include <QDebug>

using namespace xplugin;
RLCustomPropretyWidget::RLCustomPropretyWidget(const QString &type, QWidget *parent) :
    RLContentBase(parent),
    ui(new Ui::RLCustomPropretyWidget)
{
    ui->setupUi(this);
    setType(type);
}

RLCustomPropretyWidget::~RLCustomPropretyWidget()
{
    delete ui;
}

void RLCustomPropretyWidget::FillWith(const QString &str)
{
    qDebug() << "RLCustomInsertWidget::FillWith : " << m_type << " : " << str;
}

QString RLCustomPropretyWidget::ToString()
{
    QString lab1 = ui->line1->text();
    QString lab2 = ui->line2->text();
    return m_type + QString(" %1,%2").arg(lab1).arg(lab2);
}

bool RLCustomPropretyWidget::CheckReplace()
{
    return true;
}
