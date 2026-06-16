#include "showstatusbar.h"
#include "ui_showstatusbar.h"

ShowStatusBar::ShowStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowStatusBar)
{
    ui->setupUi(this);

    ui->progressBar->setValue(67);

}

ShowStatusBar::~ShowStatusBar()
{
    delete ui;
}

void ShowStatusBar::updateText(const QString &text)
{
    ui->label->setText(text);
}
