#include "dlpy_frames.h"
#include "ui_dlpy_frames.h"



dlpy_frames::dlpy_frames(int state,string filename,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dlpy_frames)
{
    setGeometry(0, 50, 640, 480);
    ui->setupUi(this);
    Reference=filename;     //读取或保存的视频文件名
    if(state==2){       //摄像头模式
        flag=2;
        captRefrnc=cv::VideoCapture(0);
        captRefrnc.set(CV_CAP_PROP_FRAME_WIDTH,640);    //视频流的帧宽度
        captRefrnc.set(CV_CAP_PROP_FRAME_HEIGHT,480);   //视频流的帧高度
        captRefrnc.set(CV_CAP_PROP_FPS,30);     //帧率
        writer=VideoWriter(Reference, CV_FOURCC('P', 'I', 'M', '1'), 30.0, Size(640, 480));     //将摄像头获取的图像转换成avi格式视频
    }else if(state==1){     //读取视频模式
        flag=1;
        captRefrnc=cv::VideoCapture(Reference);     //打开视频文件
    }else{
        exit(-1);
    }

    //检测视频是否正常打开：成功打开时，isOpened返回true
    if (captRefrnc.isOpened())
    {
        captRefrnc.read(frameReference);
        this->setFixedSize(frameReference.cols+6,frameReference.rows+6);

        //设置初始化环境参数值
        zthreshold_value = 10;       //域化值
        threshold_value = 0;           //粒子筛选
        cthreshold_value = 60;       //边缘检测粗细度
        thresholdline_value = 42;   //线段长度筛选
        alpha_value = 100;              //对比度
        times=0;
        mask_image_l = Mat::zeros(frameReference.size(),CV_8UC1);
        mask_image_r = Mat::zeros(frameReference.size(),CV_8UC1);

        //右侧车道红线?
        mask_points[0][0]=Point((frameReference.cols)/2-1,(frameReference.rows)*13/20-1);
        mask_points[0][1]=Point((frameReference.cols)*50/120-1,(frameReference.rows)*13/20-1);
        mask_points[0][2]=Point(frameReference.cols*5/160-1,(frameReference.rows)*55/60-1);
        mask_points[0][3]=Point((frameReference.cols)*35/120-1,(frameReference.rows)*45/50-1);
        mask_points[0][4]=Point((frameReference.cols)/2-1,(frameReference.rows)*13/20-1);

        //左侧车道红线?
        mask_points[1][0]=Point((frameReference.cols)/2,(frameReference.rows)*13/20-1);
        mask_points[1][1]=Point((frameReference.cols)*70/120,(frameReference.rows)*13/20-1);
        mask_points[1][2]=Point(frameReference.cols*155/160,(frameReference.rows)*55/60-1);
        mask_points[1][3]=Point((frameReference.cols)*85/120,(frameReference.rows)*45/50-1);
        mask_points[1][4]=Point((frameReference.cols)/2,(frameReference.rows)*13/20-1);

        const Point* ppt1[1] = {mask_points[0]};
        const Point* ppt2[1] = {mask_points[1]};
        int npt[] = {5,5};

        /**
         * @brief fillPoly
         * 多边形被画到mask_image_l上
         * 多边形的顶点集为ppt1
         * 要绘制的多边形顶点数目为npt
         * 要绘制的多边形数量为1
         * 多边形颜色定义为Scalar(255)
         */
        fillPoly(mask_image_l,ppt1,npt,1,Scalar(255),8);    //用指定颜色填充指定闭合的多边形
        fillPoly(mask_image_r,ppt2,npt,1,Scalar(255),8);
        //scalar(B,G,R)

//上面这段代码是图形预处理工作(定义图类型的局部变量等),调用的都是opencv内置函数,具体每句代码的含义,请查阅opencv函数.


        this->qimage=new QImage;
        *qimage=mat2qimage(frameReference);
         timer = new QTimer(this);
         timer->setInterval(60);    //打开下一帧图像的时间间隔
         connect(timer,SIGNAL(timeout()),this,SLOT(nextFrame()));
         timer->start();
     }
}

dlpy_frames::~dlpy_frames()
{
    delete ui;
}

void dlpy_frames::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:    //改变语言
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//下一帧图像
void dlpy_frames::nextFrame()
{
    this->scene = new QGraphicsScene;
    captRefrnc.read(frameReference);
    if(flag==2)writer << frameReference;    //将摄像头拍摄的图片保存为视频文件
    img1= frameReference;
    img1.copyTo(img3);
    img2=pre(img1,alpha_value,zthreshold_value,cthreshold_value);   //对当前图像进行预处理
    img3_l = Mat::zeros(img1.size(),CV_8UC1);
    img3_r = Mat::zeros(img1.size(),CV_8UC1);
    img2.copyTo(img3_l,mask_image_l);
    img2.copyTo(img3_r,mask_image_r);
    vec1=detline(img3_l,threshold_value,thresholdline_value);   //threshold_value 符合线段的识别作用
    vec2=detline(img3_r,threshold_value,thresholdline_value);   //thresholdline_value 显示筛选最短线段的作用
    img3=drawmaxline(img1,vec1,1);
    img3=drawmaxline(img3,vec2,2);
    compareall();     //路面倾斜？？不是吧，道路偏移吧！
    //大概是通过判断绿色标线中线与车道中线位置来判断是否报警


    *qimage=mat2qimage(img3);

    scene->addPixmap(QPixmap::fromImage(*qimage));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->resize(qimage->width()+6, qimage->height()+6);
    ui->graphicsView->show();
}

//画最长线
Mat dlpy_frames::drawmaxline(Mat src,vector<Vec4i> lines,int whl)
{
    Mat dst;
    Point mp1,mp2;
    src.copyTo(dst);
    double max=0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        double c=sqrt((l[0]-l[2])*(l[0]-l[2]) + (l[1]-l[3])*(l[1]-l[3]));
        if(c>=max){
            mp1=Point(l[0], l[1]);mp2=Point(l[2], l[3]);max=c;
        }
    }
    if(max>0){
        if(whl==1){ //左红线
            line_l.x=mp1.x;
            line_l.y=mp1.y;
            line_l.k=getlineK(mp1.x,mp1.y,mp2.x,mp2.y); //斜率
            line_l.b=getlineB(mp1.x,mp1.y,mp2.x,mp2.y); //截矩
            line_l.x2=mp2.x;
            line_l.y2=mp2.y;
            line_l.ifcp=1;
            line(dst, Point(lineA_l.x, lineA_l.y), Point(240, getlineY(lineA_l,240)), Scalar(0,255,0), 2, CV_AA);       //A线定位--左侧绿线
            //line:画线 dst-要画线所在的图像，起点，终点， 颜色G=255, 粗细，
        }
        if(whl==2){ //右红线
            line_r.x=mp1.x;
            line_r.y=mp1.y;
            line_r.k=getlineK(mp1.x,mp1.y,mp2.x,mp2.y);
            line_r.b=getlineB(mp1.x,mp1.y,mp2.x,mp2.y);
            line_r.x2=mp2.x;
            line_r.y2=mp2.y;
            line_r.ifcp=1;
            line(dst, Point(lineA_r.x, lineA_r.y), Point(getlineX(lineA_r,420), 420), Scalar(0,255,0), 2, CV_AA);   //B线定位--右侧绿线
        }
        line(dst, mp1, mp2, Scalar(0,0,255), 2, CV_AA);
    }
    return dst;
}

//没用处？
/*
int dlpy_frames::compare2L(stdline a,stdline b,int alerter,int py){
    int rs=0;
    circle(img3,s_point,1,Scalar( 0, 255, 0 ),-1,8);
    double arc=atan((a.k-b.k)/(1+a.k*b.k))*180/PI;
    if(alerter==2){
    }
    if(arc<-2||arc>2){
        int tmpy=getlineY(b,s_point.x)+py;
        Point k_point=Point(s_point.x,tmpy);
        if(alerter==2){
            if(tmpy>=(s_point.y+8)){emit setalertsignal(4);rs=1;}
            else if(tmpy>=(s_point.y+3)){emit setalertsignal(2);rs=1;}
            else{}
            circle(img3,k_point,1,Scalar( 255, 0, 0 ),-1,8);
        }
        if(alerter==1){
            if(tmpy>=(s_point.y+8)){emit setalertsignal(3);rs=1;}
            else if(tmpy>=(s_point.y+3)){emit setalertsignal(1);rs=1;}
            else{}
            circle(img3,k_point,1,Scalar( 0, 0, 255 ),-1,8);
        }
    }
    return rs;
}
*/

//比较一条车道线情况
void dlpy_frames::compare1L(stdline a,stdline b,int alerter,int py){
    double arc=atan((a.k-b.k)/(1+a.k*b.k))*180/PI;  //arctan?
    if(arc<-2||arc>2){
        int tmpy=getlineY(b,s_point.x)+py;      //py可能是控制误差？
        Point k_point=Point(s_point.x,tmpy);
        circle(img3,s_point,1,Scalar( 0, 255, 0 ),-1,8);
        circle(img3,k_point,1,Scalar( 255, 0, 0 ),-1,8);
        if(alerter==1){
            if(tmpy<=(s_point.y-8)){emit setalertsignal(4);times=0;}
            else if(tmpy<=(s_point.y-3)&&times>2){emit setalertsignal(2);times=0;}
            else if(tmpy>=(s_point.y+8)){emit setalertsignal(3);times=0;}
            else if(tmpy>=(s_point.y+3)&&times>2){emit setalertsignal(1);times=0;}
            else{}
        }
        if(alerter==2){
            if(tmpy<=(s_point.y-8)){emit setalertsignal(3);times=0;}
            else if(tmpy<=(s_point.y-3)&&times>2){emit setalertsignal(1);times=0;}
            else if(tmpy>=(s_point.y+8)){emit setalertsignal(4);times=0;}
            else if(tmpy>=(s_point.y+3)&&times>2){emit setalertsignal(2);times=0;}
            else{}
        }
    }
    times++;    //times的作用？
}

//比较两条左右红线的中间线和两条绿线的交点
void dlpy_frames::compare2L_mid(stdline linemid,Point pmid,int py){
    int tmpx=getlineX(linemid,pmid.y);  //计算与绿线交点y坐标相同的红线中线上的x坐标

    //通过比对同一y坐标下x坐标的距离，来判断报警程度
    if(tmpx<pmid.x){    //红线中线x<绿线交点x       说明道路红线中线跑到车的左边=====>车压到了右线
        if((tmpx+20)<pmid.x){emit setalertsignal(4);times=0;}   //
        else if((tmpx+5)<pmid.x&&times>2){emit setalertsignal(2);times=0;}
        else{}
    }
    if(tmpx>pmid.x){    //压左线
        if((tmpx-20)>pmid.x){emit setalertsignal(3);times=0;}
        else if((tmpx-5)>pmid.x&&times>2){emit setalertsignal(1);times=0;}
        else{}
        times++;
    }
    circle(img3,s_point,1,Scalar( 0, 255, 0 ),-1,8);   //画出绿线交点
    circle(img3,Point(tmpx,pmid.y),1,Scalar( 255, 0, 0 ),-1,8);     //画出红线中线同一y坐标点
}

//比较绿线和红线
void dlpy_frames::compareall(){
    if(line_l.ifcp==1&&line_r.ifcp==1){     //左右红线都存在
        line_mid=getmidline(line_l,line_r);     //获取左右标线的中间线
        Point c1=Point(line_mid.x,line_mid.y);  //中间线起点
        Point c2=Point(line_mid.x2,line_mid.y2);    //中间线终点
        Point cl1=getmidpoint(lineA_l.x,lineA_l.y,getlineX(lineA_r,lineA_l.y),lineA_l.y);   //计算右侧绿线的X坐标，然后算出与左侧起点的中点
        Point cl2=getmidpoint(lineA_l.x2,lineA_l.y2,getlineX(lineA_r,lineA_l.y2),lineA_l.y2);
        line(img3, c1, c2, Scalar(255,0,0), 2, CV_AA);      //画出红线中间线
        line(img3, cl1, cl2, Scalar(0,255,0), 2, CV_AA);    //画出绿线中间线
        compare2L_mid(line_mid,s_point,0);      //比较左右标线的中间线和两条绿线的交点
    }
    else if(line_l.ifcp==1&&line_r.ifcp==0){    //只存在左红线
        compare1L(lineA_l,line_l,1,3);  //比较左侧绿线、左侧红线、py=3      //py是啥意思？
    }
    else if(line_l.ifcp==0&&line_r.ifcp==1){    //只存在右红线
        compare1L(lineA_r,line_r,2,3);  //比较右侧绿线、右侧红线、py=3
    }
    else{}
    line_l.ifcp=0;      //为何要重新赋0?
    line_r.ifcp=0;
}

//设置环境参数
void dlpy_frames::setparaslot(int z,int c,int t,int l,int a){
    zthreshold_value=z;
    cthreshold_value = c;
    threshold_value = t;
    thresholdline_value = l;
    alpha_value =a;
}

//将左右绿线直线各个参数设置为红线参数
//返回左右两条绿线的交点x、y坐标
void dlpy_frames::setlineslot(int l){
    if(l==1)        //l=1 A线定位
    {
        lineA_l.x=line_l.x;
        lineA_l.y=line_l.y;
        lineA_l.x2=line_l.x2;
        lineA_l.y2=line_l.y2;
        lineA_l.k=line_l.k;
        lineA_l.b=line_l.b;
        lineA_r.x=line_r.x;
        lineA_r.y=line_r.y;
        lineA_r.x2=line_r.x2;
        lineA_r.y2=line_r.y2;
        lineA_r.k=line_r.k;
        lineA_r.b=line_r.b;
        s_point=getlinespoint(lineA_l,lineA_r);
    }
}

