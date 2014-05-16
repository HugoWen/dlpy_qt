#ifndef UTILS_H
#define UTILS_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "qimage.h"
#include "qpixmap.h"
#include <iostream>

using namespace cv;
using namespace std;

//下面是线的类型定义,以及线的图像处理函数(求线的x,y横纵坐标的,只在线的判断中使用的,和参数调节无任何关系)
typedef struct _stdline
{
    int x;
    int y;
    int x2;
    int y2;
    double k;
    double b;
    int ifcp;
}stdline;

QImage mat2qimage(Mat mat);
Mat pre(Mat src,int alpha,int zthresholdv,int cthresholdv);
vector<Vec4i> detline(Mat src,int thresholdv,int thresholdline);
Mat drawlines(Mat src,vector<Vec4i> lines);
int getlineX(stdline line,int Y);
int getlineY(stdline line,int X);
double getlineK(double x1,double y1,double x2,double y2);
double getlineB(double x1,double y1,double x2,double y2);
Point getmidpoint(double x1,double y1,double x2,double y2);
Point getlinespoint(stdline line1,stdline line2);
stdline getmidline(stdline linel,stdline liner);

int otsu(const IplImage *src_image);     //大津法求阈值

#endif // UTILS_H
