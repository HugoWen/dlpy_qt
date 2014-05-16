#ifndef DLPY_MAIN_H
#define DLPY_MAIN_H

#include <QMainWindow>
#include "dlpy_frames.h"
#include "weatherobj.h"
#include <fstream>
#include <iostream>
#include "widget.h"

namespace Ui {
    class dlpy_main;
}

class dlpy_main : public QMainWindow {
    Q_OBJECT
public:
    dlpy_main(int state,string filename,QWidget *parent = 0);
    ~dlpy_main();
    dlpy_frames *frame;
    widget *wid;

signals:
    void setparasignal(int z,int c,int t,int l,int a);
    void setlinesignal(int l);
public slots:
    void setalertslot(int alert);
protected:
    void changeEvent(QEvent *e);



private:
    Ui::dlpy_main *ui;
    Mat img_alert,img_alert_1,img_alert_2,img_temp;
    Rect roi_l,roi_r;
    int times_alert,last_alert;
    QTimer *timer;
    weatherobj w0,w1,w2,w3,w4,w5,w6,w7;



private slots:
    void on_button_exit_clicked();
    void on_button_save_clicked();
    void on_button_load_clicked();
    void on_horizontalScrollBar_sliderReleased();
    void on_button_A_clicked();
    void alertFrame();
    void on_hs_alpha_sliderReleased();
    void on_hs_thresholdline_sliderReleased();
    void on_hs_threshold_sliderReleased();
    void on_hs_cthreshold_sliderReleased();
    void on_hs_alpha_valueChanged(int value);
    void on_hs_thresholdline_valueChanged(int value);
    void on_hs_threshold_valueChanged(int value);
    void on_hs_zthreshold_sliderReleased();
    void on_hs_cthreshold_valueChanged(int value);
    void on_hs_zthreshold_valueChanged(int value);

    void on_button_ext_clicked();

    //void on_button_receive_clicked();
   // void readMyCom();
};

#endif // DLPY_MAIN_H
