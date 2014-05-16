#ifndef DLPY_FRAMES_H
#define DLPY_FRAMES_H

#include <QWidget>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string.h>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsScene>
#include "utils.h"

namespace Ui {
    class dlpy_frames;
}

class dlpy_frames : public QWidget {
    Q_OBJECT
public:
    dlpy_frames(int state,string filename,QWidget *parent = 0);
    ~dlpy_frames();
    stdline lineA_l,lineA_r,lineB,lineC,lineD,lineE;        //存左右A线的变量。绿线
signals:
   void setalertsignal(int alert);
public slots:
    void setparaslot(int z,int c,int t,int l,int a);
    void setlineslot(int l);
protected:
    void changeEvent(QEvent *e);
private slots:
   void nextFrame();
private:
    Ui::dlpy_frames *ui;
    cv::VideoCapture captRefrnc;    //读取视频类
    cv::Mat frameReference;     //创建mat，用于保存图像内容
    cv::VideoWriter writer;     //将图片序列保存成视频文件
    QGraphicsScene *scene;
    std::string Reference;
    QImage *qimage;
    QTimer *timer;
    stdline line_l,line_r,line_mid;         //识别出来的左右线。红线
    Mat img1,img2,img3,img3_l,img3_r,mask_image_l,mask_image_r,img_alert,img_alert_1,img_alert_2,img_temp;
    Rect roi_l,roi_r;
    vector<Vec4i> vec1,vec2;
    Point mask_points[2][4],s_point;
    static const double PI=3.1415926;
   // 阈值化（零）【zthreshold_value】：主要在opencv中的threshold函数使用，好像是去噪（具体请查opencv函数）
   // 对比度【alpha_value】：增加或降低对比度，为了突出图像用的。
   //粒子筛选【cthreshold_value】：也是起到突出图像的作用，用的opencv中的Canny函数（具体请查opencv函数）
   //边缘检测粗细度【threshold_value】：是在HoughLinesP函数中用到，起到符合线段的识别作用（具体请查opencv函数）
   //线段长度筛选【thresholdline_value】：也是在HoughLinesP函数中用到，起到显示筛选最短的线段的用（具体请查opencv函数）
    int zthreshold_value,threshold_value,cthreshold_value,thresholdline_value,alpha_value,times,flag;
    Mat drawmaxline(Mat src,vector<Vec4i> lines,int whl);

    //下面是处理路面倾斜的判断函数.
//    带有2L的函数是根据两线处理判断,1L是根据单线处理判断
    void compareall();
    void compare2L_mid(stdline linemid,Point pmid,int py);
    int compare2L(stdline a,stdline b,int alerter,int py);
    void compare1L(stdline a,stdline b,int alerter,int py);
};


#endif // DLPY_FRAMES_H
