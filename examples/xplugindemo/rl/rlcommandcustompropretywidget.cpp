#include "rlcommandcustompropretywidget.h"
#include "ui_rlcommandcustompropretywidget.h"
#include <QDebug>
#include "log/logmanager.h"
#include "rl/rlmanager.h"

using namespace xplugin;
RLCommandCustomPropretyWidget::RLCommandCustomPropretyWidget(const QString &type, QWidget *parent) :
    RLContentBase(parent),
    ui(new Ui::RLCommandCustomPropretyWidget)
{
    ui->setupUi(this);
    setType(type);
}

RLCommandCustomPropretyWidget::~RLCommandCustomPropretyWidget()
{
    delete ui;
}

void RLCommandCustomPropretyWidget::FillWith(const QString &str)
{
    RlCustomCommandValues commamd;
    RLManager::parseCustomCommandLine(m_type, str, commamd);

    ILOG << "RLCommandCustomPropretyWidget::FillWith(const QString &str) : " << commamd.assignTarget;
    ILOG << "RLCommandCustomPropretyWidget::FillWith(const QString &str) : " << commamd.fields;

    ui->spinBox_1->setValue(commamd.fields.value("vPointIndex").toInt());

    ui->lineEdit_1->setText(commamd.fields.value("vPos"));

    ui->comboBox_1->setCurrentText(commamd.fields.value("vSpeed"));
    ui->comboBox_2->setCurrentText(commamd.fields.value("vZone"));
    ui->comboBox_3->setCurrentText(commamd.fields.value("vTool"));
    ui->comboBox_4->setCurrentText(commamd.fields.value("vWobj"));

    ui->lineEdit_2->setText(commamd.fields.value("vPosJoint1"));
    ui->lineEdit_3->setText(commamd.fields.value("vPosJoint2"));
    ui->lineEdit_4->setText(commamd.fields.value("vPosJoint3"));

    ILOG << "RLCommandCustomPropretyWidget::FillWith(const QString &str) : " << str;
}

QString RLCommandCustomPropretyWidget::ToString()
{
    ILOG << "RLCommandCustomPropretyWidget::ToString()";
    return "";
}

bool RLCommandCustomPropretyWidget::CheckInsert()
{
    return true;
}
