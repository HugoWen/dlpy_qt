#ifndef COM_H
#define COM_H


#include <QCom>
#include <posix_qextserialport.h>
#include <QTimer>

namespace Ui {
class Com;
}


class Com : public QCom
{
    Q_OBJECT

public:
    explicit Com(QWidget *parent = 0);
    ~Com();

private:
    Ui::Com *ui;
    Posix_QextSerialPort *myCom;
    QTimer *readTimer;

private slots:
    void on_pushButton_clicked();
    void readMyCom();
};

#endif // COM_H
