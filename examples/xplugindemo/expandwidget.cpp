#include "expandwidget.h"
#include "ui_expandwidget.h"
#include "log/logmanager.h"
#include "interface/interfacemanager.h"

ExpandWidget::ExpandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExpandWidget)
{
    ui->setupUi(this);

    setMinimumSize(400,400);
}

ExpandWidget::~ExpandWidget()
{
    delete ui;
}

void ExpandWidget::on_pushButton_clicked()
{
    ELOG << "click btn 1";
}


void ExpandWidget::on_pushButton_2_clicked()
{
    xplugin::SwitchRightExpandWidgte();
}

