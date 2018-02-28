#include "adminui.h"
#include "ui_adminui.h"

adminUi::adminUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adminUi)
{
    ui->setupUi(this);
    adminID=-1;
}

adminUi::~adminUi()
{
    delete ui;
}

void adminUi::setLogData(QVector<QVariantMap> allLog)
{

    tableWidget=new QTableWidget(allLog.length(),2);
    tableWidget->setColumnCount(2);

    QStringList header;
    header<<"dateTime"<<"name";
    tableWidget->setHorizontalHeaderLabels(header);

    int count=0;
    while(allLog.length())
    {
        QVariantMap oneLog=allLog.front();
        allLog.pop_front();
        QString  name=oneLog.value("name").toString();
        QString  dateTime=oneLog.value("datetime").toString();
        tableWidget->setItem(count,0,new QTableWidgetItem(dateTime));
        tableWidget->setItem(count,1,new QTableWidgetItem(name));
        count++;
    }
}

void adminUi::on_pushButton_clicked()
{
    QString name=ui->lineEdit->text();
    emit regnew(name,adminID);
}

void adminUi::on_pushButton_4_clicked()
{
    tableWidget->show();
}
