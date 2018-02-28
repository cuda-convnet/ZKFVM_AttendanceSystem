#ifndef REGNEW_H
#define REGNEW_H

#include <QDialog>

namespace Ui {
class regNew;
}

class regNew : public QDialog
{
    Q_OBJECT

public:
    explicit regNew(QWidget *parent = 0);
    ~regNew();
public slots:
    void setLcdNumber(int);

private:
    Ui::regNew *ui;
};

#endif // REGNEW_H
