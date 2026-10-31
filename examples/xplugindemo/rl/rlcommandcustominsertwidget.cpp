#include "rlcommandcustominsertwidget.h"
#include "ui_rlcommandcustominsertwidget.h"
#include <QDebug>
#include "log/logmanager.h"

using namespace xplugin;
RLCommandCustomInsertWidget::RLCommandCustomInsertWidget(const QString &type, QWidget *parent) :
    RLContentBase(parent),
    ui(new Ui::RLCommandCustomInsertWidget)
{
    ui->setupUi(this);
    setType(type);
}

RLCommandCustomInsertWidget::~RLCommandCustomInsertWidget()
{
    delete ui;
}

void RLCommandCustomInsertWidget::FillWith(const QString &str)
{
    ILOG << "RLCommandCustomInsertWidget::FillWith(const QString &str)";
}

QString RLCommandCustomInsertWidget::ToString()
{
    int index1 = ui->spinBox_1->value();

    QString speed = ui->comboBox_1->currentText();
    QString zone = ui->comboBox_2->currentText();
    QString tool = ui->comboBox_3->currentText();
    QString wobj = ui->comboBox_4->currentText();

    QString p1 = ui->lineEdit_1->text().trimmed();
    QString j1 = ui->lineEdit_2->text().trimmed();
    QString j2 = ui->lineEdit_3->text().trimmed();
    QString j3 = ui->lineEdit_4->text().trimmed();

    QString switch1 = ui->lineEdit->text().trimmed();

    QString str = m_type + QString("(%1%2%3%4%5%6%7%8%9%10);");
    str = ("" == switch1) ? str.arg("") : str.arg(QString("%1,").arg(switch1));
    str = (0 == index1) ? str.arg("") : str.arg(QString("%1, ").arg(index1));
    str = ("" == p1) ? str.arg("") : str.arg(QString("%1,").arg(p1));
    str = ("" == speed) ? str.arg("") : str.arg(QString("%1,").arg(speed));
    str = ("" == zone) ? str.arg("") : str.arg(QString("%1,").arg(zone));
    str = ("" == tool) ? str.arg("") : str.arg(QString("%1,").arg(tool));
    str = ("" == wobj) ? str.arg("") : str.arg(QString("%1,").arg(wobj));
    str = ("" == j1) ? str.arg("") : str.arg(QString(", %1").arg(j1));
    str = ("" == j2) ? str.arg("") : str.arg(QString(", %1").arg(j2));
    str = ("" == j3) ? str.arg("") : str.arg(QString(", %1").arg(j3));
    return str;
}

bool RLCommandCustomInsertWidget::CheckInsert()
{
    return true;
}
