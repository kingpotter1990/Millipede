#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

struct KEY_CAM{
    int f;                   // frame #
    float X;float Y;float Z; // camera position
    float x;float y;float z; // look at
    float a;float b;float c; // right
};

int main(){
    static const int N=3;
    KEY_CAM cam[N];
    cam[0]={1,
            1.2,2.2,3.2,
            1.5,2.5,3.5,
            1.0,0.0,0.0};
    cam[1]={4,
            2.2,3.2,4.2,
            2.5,3.5,4.5,
            2.0,0.0,0.0};
    cam[2]={100,
            3.2,4.2,5.2,
            3.5,4.5,5.5,
            1.0,0.0,0.0};

    for(int i=0;i<N-1;i++){
        double df=cam[i+1].f-cam[i].f;
        for(int f=cam[i].f;f<=cam[i+1].f;f++){
            double thisdf=f-cam[i].f;
            cout<<"Writing cam/CAM_"<<f<<".inc"<<std::endl;
            stringstream ss;
            ss<<f;
            string fn="cam/CAM_"+ss.str()+".inc";
            ofstream file;
            file.open(fn.c_str());
            file<<"camera{"<<endl
                <<" location "<<"<"<<cam[i].X+thisdf*(cam[i+1].X-cam[i].X)/df<<","<<cam[i].Y+thisdf*(cam[i+1].Y-cam[i].Y)/df<<","<<cam[i].Z+thisdf*(cam[i+1].Z-cam[i].Z)/df<<">"<<endl
                <<" look at  "<<"<"<<cam[i].x+thisdf*(cam[i+1].x-cam[i].x)/df<<","<<cam[i].y+thisdf*(cam[i+1].y-cam[i].y)/df<<","<<cam[i].z+thisdf*(cam[i+1].z-cam[i].z)/df<<">"<<endl
                <<" right    "<<"<"<<cam[i].a+thisdf*(cam[i+1].a-cam[i].a)/df<<","<<cam[i].b+thisdf*(cam[i+1].b-cam[i].b)/df<<","<<cam[i].c+thisdf*(cam[i+1].c-cam[i].c)/df<<">"<<endl
                <<"}"<<endl;
            file.close();}}
    
    return 0;
}
