#include "utils.h"

Mat rgb;

//将mat格式图像转换为qimage格式图像
QImage mat2qimage(cv::Mat mat){
        rgb = mat.clone();
        QImage *img;
        if(mat.channels() == 3)    // RGB image
        {
            cvtColor(mat,rgb,CV_BGR2RGB);
            img =new QImage((unsigned char*)(rgb.data),  //(const unsigned char*)
                         rgb.cols,rgb.rows,
                         QImage::Format_RGB888);


        }
        else                     // gray image
        {
            img =new QImage((unsigned char*)(mat.data),
                         mat.cols,mat.rows,
                         QImage::Format_Indexed8);
        }
        return *img;
}

//图像预处理
Mat pre(Mat src,int alpha,int zthresholdv,int cthresholdv){
    Mat dst, temp;
    Rect rect;
    double brightValue = 0.00;  //灰度值

    cvtColor(src, dst, CV_RGB2GRAY);    //src转化为灰度图像dst

    //车道阈值选取
    dst.copyTo(temp);
    //rect = Rect(0,(dst.rows - 1)/2, dst.cols, (dst.rows - 1) / 2);      //截取下半部分图像，保证处理的是路面区域
    //temp = dst(rect);

    /*
    for (int x = 1; x < temp.rows - 1; x++)
    {
        for (int y = 1; y < temp.cols - 1; y++)
        {
            //计算每一点像素的累加和
            brightValue = brightValue + temp.at<uchar>(x,y);
        }
    }
    brightValue = brightValue /( temp.rows - 1) /( temp.cols - 1 );	//计算平均灰度值
   // zthresholdv = brightValue;  //将平均灰度值赋给阈值
    */

    IplImage *img;
    img = cvCreateImage(cvSize(temp.cols, temp.rows),8,1);

    //int thres  = 0;
    zthresholdv = otsu(img);
    cout<<"阈值：          "<<zthresholdv<<endl;
    /*
    //sobel算子
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    /// 创建 grad_x 和 grad_y 矩阵
      Mat grad_x, grad_y;
      Mat abs_grad_x, abs_grad_y;

      /// 求 X方向梯度
      //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
      Sobel( dst, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_x, abs_grad_x );

      /// 求Y方向梯度
      //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
      Sobel( dst, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_y, abs_grad_y );

      /// 合并梯度(近似)
      addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst );
    */

    //灰度图像dst进行阈值操作得到二值图像
    threshold(dst, dst, zthresholdv, 255,3);    //二值化
    /*convertTo()函数是源数组线性变换成目标数组，第二个参数为目标数组的类型*/
    dst.convertTo(dst, -1, (double)(alpha)/100, 0);
    equalizeHist(dst, dst);     //使灰度图像直方图均衡化
    GaussianBlur(dst,dst,Size(3,3),0,0);    //高斯滤波
    //medianBlur(dst, dst,3);

    Canny(dst, dst, cthresholdv, cthresholdv*3,3);  //边缘检测--检测轮廓
    return dst;
}

//返回图像中的直线集合
vector<Vec4i> detline(Mat src,int thresholdv,int thresholdline)
{
    vector<Vec4i> lines;

    //检测图像中的直线
    //输入图像只能是边缘二值图像
    HoughLinesP(src, lines, 1, CV_PI/180, thresholdv+1, thresholdline, 10 );    //输出检测到的图像端点(x0,y0,x1,y1)
    return lines;
}


//在图像上画线，并返回画线后图像
Mat drawlines(Mat src,vector<Vec4i> lines)
{
      Mat dst;
      src.copyTo(dst);
      for( size_t i = 0; i < lines.size(); i++ )
      {
            Vec4i l = lines[i];
            line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2, CV_AA);     //画线
            //dst--要画的线所在的图像，直线起点，直线终点，颜色(红色)，线条粗细，antialiased线条
      }
      return dst;
}

//是不是算错了？
//根据一条直线以及另一条直线上一点y坐标计算x坐标
int getlineX(stdline line,int Y){
    //double tmp=(((double)line.x*line.k-Y+line.y)/line.k);
    double tmp = (Y - line.y + (double)line.x * line.k) / line.k;       //改正
    return (int)(tmp+0.5);
}

//计算y坐标
int getlineY(stdline line,int X){
    //double tmp=(-line.k*(X-line.x)+line.y);
    double tmp = line.y - line.k * (line.x - X);        //改正
    return (int)(tmp+0.5);
}

//返回两点斜率
double getlineK(double x1,double y1,double x2,double y2){
    //return -(y1-y2)/(x1-x2);    //有负号？
    return (y1-y2)/(x1-x2);
}

//返回y轴常量
double getlineB(double x1,double y1,double x2,double y2){
    return (x1*y2-x2*y1)/(x1-x2);
}


//返回两点中间点x、y坐标
Point getmidpoint(double x1,double y1,double x2,double y2){
    double tempx=(x1+x2)/2;
    double tempy=(y1+y2)/2;
    return Point((int)tempx, (int)tempy);
}

//返回两条直线交点的x、y坐标
Point getlinespoint(stdline line1,stdline line2){
    double tempy=(line1.b*line2.k-line1.k*line2.b)/(line2.k-line1.k);
    double tempx=(line1.b-tempy)/line1.k;
    return Point((int)tempx, (int)tempy);
}

//返回两线中间线
stdline getmidline(stdline linel,stdline liner){
    stdline line_mid_temp;
    Point c1=getmidpoint(linel.x,linel.y,getlineX(liner,linel.y),linel.y);      //返回左右两条直线的相同y坐标点的中间点
    Point c2=getmidpoint(liner.x,liner.y,getlineX(linel,liner.y),liner.y);
    line_mid_temp.x=c1.x;
    line_mid_temp.y=c1.y;
    line_mid_temp.x2=c2.x;
    line_mid_temp.y2=c2.y;
    line_mid_temp.k=getlineK(c1.x,c1.y,c2.x,c2.y);
    line_mid_temp.b=getlineB(c1.x,c1.y,c2.x,c2.y);
    return line_mid_temp;
}


int otsu(const IplImage *src_image) //大津法求阈值
{
    double sum = 0.0;
    double w0 = 0.0;
    double w1 = 0.0;
    double u0_temp = 0.0;
    double u1_temp = 0.0;
    double u0 = 0.0;
    double u1 = 0.0;
    double delta_temp = 0.0;
    double delta_max = 0.0;

    //src_image灰度级
    int pixel_count[256]={0};
    float pixel_pro[256]={0};
    int threshold = 0;
    uchar* data = (uchar*)src_image->imageData;
    //统计每个灰度级中像素的个数
    for(int i = 0; i < src_image->height; i++)
    {
        for(int j = 0;j < src_image->width;j++)
        {
            pixel_count[(int)data[i * src_image->width + j]]++;
            sum += (int)data[i * src_image->width + j];
        }
    }
    cout<<"平均灰度："<<sum / ( src_image->height * src_image->width )<<endl;
    //计算每个灰度级的像素数目占整幅图像的比例
    for(int i = 0; i < 256; i++)
    {
    pixel_pro[i] = (float)pixel_count[i] / ( src_image->height * src_image->width );
    }
    //遍历灰度级[0,255],寻找合适的threshold
    for(int i = 0; i < 256; i++)
    {
        w0 = w1 = u0_temp = u1_temp = u0 = u1 = delta_temp = 0;
        for(int j = 0; j < 256; j++)
        {
            if(j <= i)   //背景部分
            {
                w0 += pixel_pro[j];
                u0_temp += j * pixel_pro[j];
            }
            else   //前景部分
            {
                w1 += pixel_pro[j];
                u1_temp += j * pixel_pro[j];
            }
        }
        u0 = u0_temp / w0;
        u1 = u1_temp / w1;
        delta_temp = (float)(w0 *w1* pow((u0 - u1), 2)) ;
        if(delta_temp > delta_max)
        {
            delta_max = delta_temp;
            threshold = i;
        }
    }
    return threshold;
}

