#ifndef Thread_H
#define Thread_H

#include <QThread>
#include <QString>
#include "posix_qextserialport.h"
#include "qextserialbase.h"


class Thread : public QThread
{
    Q_OBJECT
public:
    Thread(QString com, QObject *parent);
    ~Thread();
    void run();
    void stopThread();

signals:
    void serialFinished(QByteArray temp);
private:
    Posix_QextSerialPort *myCom;
    int stopped;
};

#endif // Thread_H
