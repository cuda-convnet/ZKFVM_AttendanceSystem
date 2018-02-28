#include "regnew.h"
#include "ui_regnew.h"

regNew::regNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::regNew)
{
    ui->setupUi(this);

    ui->lcdNumber->display(QString::number(3));

}

regNew::~regNew()
{
    delete ui;
}

void regNew::setLcdNumber(int T)
{
    ui->lcdNumber->display(QString::number(T));
}
