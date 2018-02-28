#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fvdevice.h"
#include "adminui.h"
#include "regnew.h"

#include <QMainWindow>
#include <QMessageBox>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    FVDevice FV;
    adminUi ad;
    regNew reg;

public slots:
    void setPhoto(char *);
    void regNewOne(QString,int);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
