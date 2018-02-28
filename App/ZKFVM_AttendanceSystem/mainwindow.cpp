#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FV.m_Init();
    FV.m_Conn();

    QObject::connect(&FV,SIGNAL(pressed(char*)),this,SLOT(setPhoto(char*)));
    QObject::connect(&ad,SIGNAL(regnew(QString,int)),this,SLOT(regNewOne(QString,int)));
    QObject::connect(&FV,SIGNAL(restTimes(int)),&reg,SLOT(setLcdNumber(int)));
    QObject::connect(&FV,SIGNAL(regOver()),&reg,SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPhoto(char * pImg)
{
    qDebug()<<"setPhoto";
    QImage tmp((uchar *)pImg,320,240,320,QImage::Format_Grayscale8);
    tmp.save("2.BMP");
    QPixmap newFvImage;
    newFvImage.convertFromImage(tmp);
    ui->label_3->setPixmap(newFvImage);
}

void MainWindow::regNewOne(QString name,int id)
{
    FV.database.addUser(name,(qint32)id);

    FV.m_Enroll(FV.database.getUserId(name));
    reg.show();
}

void MainWindow::on_pushButton_clicked()
{
    int adminID=FV.database.getAdminId(ui->lineEdit_admin_name->text(),ui->lineEdit_admin_passwd->text());
    if(-1==adminID)
    {
        QMessageBox::information(this,"warning_1","wrong name or passwd");
        return;
    }
    ad.setLogData(FV.database.getALLLog());
    ad.show();
    ad.adminID=adminID;

}

void MainWindow::on_pushButton_2_clicked()
{
    //开始识别
    FV.loadFromDataBase();

}
