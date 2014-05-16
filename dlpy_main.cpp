#include "dlpy_main.h"
#include "ui_dlpy_main.h"
#include "utils.h"

dlpy_main::dlpy_main(int state,string filename,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dlpy_main)
{

    ui->setupUi(this);
    setGeometry(697, 50, 220, 480);

    wid = new widget();
    wid->setWindowFlags(Qt::FramelessWindowHint);
    wid->show();

    frame = new dlpy_frames(state,filename);
    frame->setWindowFlags(Qt::FramelessWindowHint);     //隐藏标题栏
    frame->show();
    connect(this,SIGNAL(setparasignal(int,int,int,int,int)),frame,SLOT(setparaslot(int,int,int,int,int)));
    connect(frame,SIGNAL(setalertsignal(int)),this,SLOT(setalertslot(int)));    //每一帧图像信号槽初始化警报参数
    connect(this,SIGNAL(setlinesignal(int)),frame,SLOT(setlineslot(int)));


/////////////////////////////////////////////////////////////////////////////////////////////
    //img_alert: 显示所有
    //img_alert_1: 红色警报
    //img_alert_3: 无警报
    //img_temp:所有
    //img:所有
////////////////////////////////////////////////////////////////////////////////////////////

    img_alert = cv::imread("/home/hugo/code/bin/alert.bmp");
    img_alert_1 = cv::imread("/home/hugo/code/bin/alert_2.bmp");
    img_alert_2 = cv::imread("/home/hugo/code/bin/alert_3.bmp");
    img_temp = cv::imread("/home/hugo/code/bin/alert.bmp");
    roi_l=Rect(0, 0, (img_alert.cols-1)/2, img_alert.rows-1);
    roi_r=Rect((img_alert.cols-1)/2, 0, (img_alert.cols-1)/2, img_alert.rows-1);
    QGraphicsScene *scene = new QGraphicsScene;
    QImage img;
    img.load("/home/hugo/code/bin/alert.bmp");
    QPixmap mp;
    mp=mp.fromImage(img);

    times_alert=0;
    last_alert=0;
    w0=weatherobj(10,20,30,40,50);
    w1=weatherobj(60,70,80,90,100);
    w2=weatherobj(100,110,120,130,140);
    w3=weatherobj(10,60,0,42,100);
    w4=weatherobj(150,160,170,180,190);
    w5=weatherobj(200,210,220,230,240);
    w6=weatherobj(10,25,10,32,90);
    w7=weatherobj(200,240,220,250,123);

    QImage *qimg=new QImage;
    *qimg=mat2qimage(img_temp);
    /*cvtColor(img_temp, img_temp, CV_BGR2RGB);
    qimg = new QImage((unsigned char*)img_temp.data, // uchar* data
            img_temp.cols, img_temp.rows, // width height
            img_temp.step, //bytesPerLine
            QImage::Format_RGB888); //format*/
    scene->addPixmap(QPixmap::fromImage(*qimg));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->resize(qimg->width() + 10, qimg->height() + 10);
    ui->graphicsView->show();
     timer = new QTimer(this);
     timer->setInterval(30);
     connect(timer,SIGNAL(timeout()),this,SLOT(alertFrame()));      //每隔30s读取一次报警图片
     timer->start();

}

dlpy_main::~dlpy_main()
{
    delete ui;
}

//设置警报参数
void dlpy_main::setalertslot(int alert){
    times_alert=3;
    last_alert=alert;
}

void dlpy_main::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//警报显示
void dlpy_main::alertFrame()
{
    Mat roi_alert,roi_alert_tmp,roi_temp;
    if(times_alert>0)
    {
        if(times_alert%2==0)
        {
            if(last_alert==1){roi_temp=img_temp(roi_l);roi_alert_tmp = img_alert_1(roi_l);}
            if(last_alert==2){roi_temp=img_temp(roi_r);roi_alert_tmp = img_alert_1(roi_r);}
            if(last_alert==3){roi_temp=img_temp(roi_l);roi_alert_tmp = img_alert_2(roi_l);}
            if(last_alert==4){roi_temp=img_temp(roi_r);roi_alert_tmp = img_alert_2(roi_r);}
            roi_alert_tmp.copyTo(roi_temp);
        }else{
            img_alert.copyTo(img_temp);
        }
        times_alert--;      //控制闪烁
        QGraphicsScene *scene = new QGraphicsScene;
        QImage *qimg=new QImage;
        *qimg=mat2qimage(img_temp);
        scene->addPixmap(QPixmap::fromImage(*qimg));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->resize(qimg->width() + 10, qimg->height() + 10);
        ui->graphicsView->show();
    }else{

    }
}


void dlpy_main::on_hs_zthreshold_valueChanged(int value)
{
    ui->label_z->setText(QString::number(ui->hs_zthreshold->value(), 10));
    ui->label_c->setText(QString::number(ui->hs_cthreshold->value(), 10));
    ui->label_t->setText(QString::number(ui->hs_threshold->value(), 10));
    ui->label_tl->setText(QString::number(ui->hs_thresholdline->value(), 10));
    ui->label_a->setText(QString::number(ui->hs_alpha->value(), 10));
}

void dlpy_main::on_hs_cthreshold_valueChanged(int value)
{
    ui->label_z->setText(QString::number(ui->hs_zthreshold->value(), 10));
    ui->label_c->setText(QString::number(ui->hs_cthreshold->value(), 10));
    ui->label_t->setText(QString::number(ui->hs_threshold->value(), 10));
    ui->label_tl->setText(QString::number(ui->hs_thresholdline->value(), 10));
    ui->label_a->setText(QString::number(ui->hs_alpha->value(), 10));

}

void dlpy_main::on_hs_threshold_valueChanged(int value)
{
    ui->label_z->setText(QString::number(ui->hs_zthreshold->value(), 10));
    ui->label_c->setText(QString::number(ui->hs_cthreshold->value(), 10));
    ui->label_t->setText(QString::number(ui->hs_threshold->value(), 10));
    ui->label_tl->setText(QString::number(ui->hs_thresholdline->value(), 10));
    ui->label_a->setText(QString::number(ui->hs_alpha->value(), 10));
}

void dlpy_main::on_hs_thresholdline_valueChanged(int value)
{
    ui->label_z->setText(QString::number(ui->hs_zthreshold->value(), 10));
    ui->label_c->setText(QString::number(ui->hs_cthreshold->value(), 10));
    ui->label_t->setText(QString::number(ui->hs_threshold->value(), 10));
    ui->label_tl->setText(QString::number(ui->hs_thresholdline->value(), 10));
    ui->label_a->setText(QString::number(ui->hs_alpha->value(), 10));
}

void dlpy_main::on_hs_alpha_valueChanged(int value)
{
    ui->label_z->setText(QString::number(ui->hs_zthreshold->value(), 10));
    ui->label_c->setText(QString::number(ui->hs_cthreshold->value(), 10));
    ui->label_t->setText(QString::number(ui->hs_threshold->value(), 10));
    ui->label_tl->setText(QString::number(ui->hs_thresholdline->value(), 10));
    ui->label_a->setText(QString::number(ui->hs_alpha->value(), 10));
}


void dlpy_main::on_hs_zthreshold_sliderReleased()
{

    emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());

}

void dlpy_main::on_hs_cthreshold_sliderReleased()
{
     emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
}

void dlpy_main::on_hs_threshold_sliderReleased()
{
     emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
}

void dlpy_main::on_hs_thresholdline_sliderReleased()
{
     emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
}

void dlpy_main::on_hs_alpha_sliderReleased()
{
     emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
}

//A线定位
void dlpy_main::on_button_A_clicked()
{
    emit setlinesignal(1);
}


void dlpy_main::on_horizontalScrollBar_sliderReleased()
{
    if(ui->horizontalScrollBar->value()==0){
    ui->hs_zthreshold->setValue(w0.getz());
    ui->hs_cthreshold->setValue(w0.getct());
    ui->hs_threshold->setValue(w0.gett());
    ui->hs_thresholdline->setValue(w0.getl());
    ui->hs_alpha->setValue(w0.geta());
     ui->label_w->setText(QString::fromUtf8("晴天"));
    }
    if(ui->horizontalScrollBar->value()==1){
        ui->hs_zthreshold->setValue(w1.getz());
        ui->hs_cthreshold->setValue(w1.getct());
        ui->hs_threshold->setValue(w1.gett());
        ui->hs_thresholdline->setValue(w1.getl());
        ui->hs_alpha->setValue(w1.geta());
         ui->label_w->setText(QString::fromUtf8("阴天"));
    }
    if(ui->horizontalScrollBar->value()==2){
        ui->hs_zthreshold->setValue(w2.getz());
        ui->hs_cthreshold->setValue(w2.getct());
        ui->hs_threshold->setValue(w2.gett());
        ui->hs_thresholdline->setValue(w2.getl());
        ui->hs_alpha->setValue(w2.geta());
        ui->label_w->setText(QString::fromUtf8("雨天"));
    }
    if(ui->horizontalScrollBar->value()==3){
        ui->hs_zthreshold->setValue(w3.getz());
        ui->hs_cthreshold->setValue(w3.getct());
        ui->hs_threshold->setValue(w3.gett());
        ui->hs_thresholdline->setValue(w3.getl());
        ui->hs_alpha->setValue(w3.geta());
        ui->label_w->setText(QString::fromUtf8("雪天"));
    }
    if(ui->horizontalScrollBar->value()==4){
        ui->hs_zthreshold->setValue(w4.getz());
        ui->hs_cthreshold->setValue(w4.getct());
        ui->hs_threshold->setValue(w4.gett());
        ui->hs_thresholdline->setValue(w4.getl());
        ui->hs_alpha->setValue(w4.geta());
        ui->label_w->setText(QString::fromUtf8("自定义1"));
    }
    if(ui->horizontalScrollBar->value()==5){
        ui->hs_zthreshold->setValue(w5.getz());
        ui->hs_cthreshold->setValue(w5.getct());
        ui->hs_threshold->setValue(w5.gett());
        ui->hs_thresholdline->setValue(w5.getl());
        ui->hs_alpha->setValue(w5.geta());
        ui->label_w->setText(QString::fromUtf8("自定义2"));
    }
    if(ui->horizontalScrollBar->value()==6){
        ui->hs_zthreshold->setValue(w6.getz());
        ui->hs_cthreshold->setValue(w6.getct());
        ui->hs_threshold->setValue(w6.gett());
        ui->hs_thresholdline->setValue(w6.getl());
        ui->hs_alpha->setValue(w6.geta());
       ui->label_w->setText(QString::fromUtf8("自定义3"));
    }
    if(ui->horizontalScrollBar->value()==7){
        ui->hs_zthreshold->setValue(w7.getz());
        ui->hs_cthreshold->setValue(w7.getct());
        ui->hs_threshold->setValue(w7.gett());
        ui->hs_thresholdline->setValue(w7.getl());
        ui->hs_alpha->setValue(w7.geta());
        ui->label_w->setText(QString::fromUtf8("自定义4"));
    }
    emit setparasignal(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
}

//载入保存的参数
void dlpy_main::on_button_load_clicked()
{
    std::ifstream rconfig;
    rconfig.open("fconfig.txt",std::ios::binary);
    rconfig.read((char *)&w0,sizeof(weatherobj));
    rconfig.read((char *)&w1,sizeof(weatherobj));
    rconfig.read((char *)&w2,sizeof(weatherobj));
    rconfig.read((char *)&w3,sizeof(weatherobj));
    rconfig.read((char *)&w4,sizeof(weatherobj));
    rconfig.read((char *)&w5,sizeof(weatherobj));
    rconfig.read((char *)&w6,sizeof(weatherobj));
    rconfig.read((char *)&w7,sizeof(weatherobj));
    rconfig.read((char *)&(frame->lineA_l),sizeof(stdline));
    rconfig.read((char *)&(frame->lineA_r),sizeof(stdline));
    rconfig.close();
}

//保存参数函数
void dlpy_main::on_button_save_clicked()
{
    if(ui->horizontalScrollBar->value()==0){
        w0=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==1){
        w1=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==2){
        w2=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==3){
        w3=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==4){
        w4=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==5){
        w5=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==6){
        w6=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }
    if(ui->horizontalScrollBar->value()==7){
        w7=weatherobj(ui->hs_zthreshold->value(),ui->hs_cthreshold->value(),ui->hs_threshold->value(),ui->hs_thresholdline->value(),ui->hs_alpha->value());
    }

    //将参数写到fconfig.txt文档中
    std::ofstream wconfig;
    wconfig.open("fconfig.txt",std::ios::binary);
    wconfig.write((char *)&w0,sizeof(weatherobj));
    wconfig.write((char *)&w1,sizeof(weatherobj));
    wconfig.write((char *)&w2,sizeof(weatherobj));
    wconfig.write((char *)&w3,sizeof(weatherobj));
    wconfig.write((char *)&w4,sizeof(weatherobj));
    wconfig.write((char *)&w5,sizeof(weatherobj));
    wconfig.write((char *)&w6,sizeof(weatherobj));
    wconfig.write((char *)&w7,sizeof(weatherobj));
    wconfig.write((char *)&(frame->lineA_l),sizeof(stdline));
    wconfig.write((char *)&(frame->lineA_r),sizeof(stdline));
    wconfig.close();
}

//退出函数
void dlpy_main::on_button_exit_clicked()
{
    exit(-1);
}

//高级按钮点击事件
void dlpy_main::on_button_ext_clicked()
{
    if(ui->button_ext->text()==QString::fromUtf8("高级")){
        setGeometry(430, 50, 420, 480);
        ui->button_ext->setText(QString::fromUtf8("普通"));
    }
    else{
        ui->button_ext->setText(QString::fromUtf8("高级"));
        setGeometry(697, 50, 220, 480);
    }
}


