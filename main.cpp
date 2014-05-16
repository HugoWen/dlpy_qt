#include <QApplication>
#include "dlpy_main.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
//    if(argc==3&&(string)argv[1]=="-r"){
//        std::cout<<"read"<<std::endl;
//    dlpy_main *w=new dlpy_main(1,(string)argv[2]);
//    w->show();
//    }else if(argc==3&&(string)argv[1]=="-w"){
//    dlpy_main *w=new dlpy_main(2,(string)argv[2]);
//    w->show();
//    }else{
//    std::cout<<"example: ./dlpy_qt -r xxx.avi"<<std::endl;
//    return 0;
//    }
//    sensor_data *data = new sensor_data();
//    data->show();

//    widget *wid = new widget();
//    wid->show();
    dlpy_main *w = new dlpy_main(1,"/home/hugo/code/bin/test12.avi");
    w->setWindowFlags(Qt::FramelessWindowHint);
    w->show();

    return a.exec();
}
