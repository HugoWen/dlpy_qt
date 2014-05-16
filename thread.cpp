#include "thread.h"

Thread::Thread(QString com, QObject *parent)
    :QThread(parent)
{
    myCom = new Posix_QextSerialPort(com, QextSerialBase::Polling);
    myCom->open(QIODevice::ReadWrite);
    stopped = 1;
    myCom->setBaudRate(BAUD9600);
    myCom->setDataBits(DATA_8);
    myCom->setParity(PAR_NONE);
    myCom->setStopBits(STOP_1);
    myCom->setFlowControl(FLOW_OFF);
    myCom->setTimeout(100);
}

Thread::~Thread()
{
}

void Thread::run()
{
    while(stopped)
    {
        //msleep(5000);
        QByteArray temp = myCom->readAll();
        //if(temp.size() == 8)
        //{
            emit this->serialFinished(temp);
       // }

    }
}

void Thread::stopThread()
{
    stopped = 0;
}
