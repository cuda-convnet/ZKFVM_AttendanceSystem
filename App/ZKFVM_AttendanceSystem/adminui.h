#ifndef ADMINUI_H
#define ADMINUI_H

#include <QWidget>
#include <QTableWidget>

namespace Ui {
class adminUi;
}

class adminUi : public QWidget
{
    Q_OBJECT

public:
    explicit adminUi(QWidget *parent = 0);
    ~adminUi();
    int adminID;
    void setLogData(QVector<QVariantMap>);

    QTableWidget * tableWidget;

signals:
    void regnew(QString,int);
    void s_addUser();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::adminUi *ui;

};

#endif // ADMINUI_H
