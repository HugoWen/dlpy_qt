#include <iostream>
#include <string>
#include <fstream>
#include "weatherobj.h"

using namespace std;


void weatherobj::print()
{
    cout<<"zthreshold = "<<zthreshold<<endl;
    cout<<"cthreshold = "<<cthreshold<< endl;
    cout<<"threshold = "<<threshold<<endl;
    cout<<"thresholdline = "<<thresholdline<< endl;
    cout<<"trackbar_alpha = "<<trackbar_alpha<< endl;
}
weatherobj::weatherobj()
{
}
weatherobj::weatherobj(int z,int ct,int t,int l,int a)
{
    zthreshold=z;
    cthreshold=ct;
    threshold=t;
    thresholdline=l;
    trackbar_alpha=a;
}
int weatherobj::getz(){
    return zthreshold;
}
int weatherobj::getct(){
    return cthreshold;
}
int weatherobj::gett(){
    return threshold;
}
int weatherobj::getl(){
    return thresholdline;
}
int weatherobj::geta(){
    return trackbar_alpha;
}
