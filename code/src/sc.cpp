//Reference : Urvi Mali

#include "sc.h"
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <chrono>
using namespace cv;
using namespace std;
double reducetime=0;
double imageprepare=0;
int multiplicative=255;
int dd=CV_64F;
int dx=1;
int dy=0;
vector<int> selectedPixels;
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
    return true;
}


bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){
    
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;
    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
    }
    
    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}
Mat getLoc(Mat& out_image)
{
    double least, highest;
    minMaxLoc(out_image, &least, &highest);
    
    out_image = out_image * (multiplicative/highest);
    out_image.convertTo(out_image, CV_8U);
    return out_image;
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& iimage, int new_width, int new_height, Mat& oimage){
    
    Mat in_image = iimage.clone();
    Mat out_image = iimage.clone();
     int intTempW,intTempH;
    int minIndex=0;
    int intleastEleIndex=0;
    int intleastEleValue=0;
    if(in_image.cols>new_width){
        intTempW = out_image.cols;
        intTempH = out_image.rows;
        vector<vector<int> > intensMatx(intTempH+1,vector<int>(intTempW+1,0));
        vector<vector<int> > keepTrack(intTempH+1,vector<int>(intTempW+1,0));
        selectedPixels.clear();
        for(int i=0;i<out_image.rows;i++)
            selectedPixels.push_back(1);
        minIndex=selectedPixels[0];
    while( in_image.cols!=new_width){
        Mat matBW;
            cvtColor(in_image, matBW, CV_BGR2GRAY);
            Mat matScharr_x, matScharr_y;
            
            Scharr(matBW, matScharr_x, dd, dx, dy);
            Scharr(matBW, matScharr_y, dd, dy, dx);
            magnitude(matScharr_x,matScharr_y, out_image);
            out_image=getLoc(out_image);
            intTempW = out_image.cols;
            intTempH = out_image.rows;
            minIndex=selectedPixels[0];
            intleastEleIndex=max(intTempH,intTempW);
            bool flag=true;
            for(int i = 0; i < intTempH; i++){
                if(selectedPixels[i]<minIndex)
                    minIndex=selectedPixels[i];
                for(int j = minIndex-1; j < intTempW; j++){
                    if(j>=0){
                    intensMatx[i][j] = out_image.at<uchar>(i,j);
                    if(i!=0){
                        if (j == 0) {
                            if(intensMatx[i-1][j]<= intensMatx[i-1][j+1])
                            {
                                intensMatx[i][j]+=intensMatx[i-1][j];
                                keepTrack[i][j]=0;
                            }
                            else{
                                intensMatx[i][j]+=intensMatx[i-1][j+1];
                                keepTrack[i][j]=1;
                            }
                        }
                        else if (j == intTempW-1){
                            if(intensMatx[i-1][j]<= intensMatx[i-1][j-1])
                            {
                                intensMatx[i][j]+=intensMatx[i-1][j];
                                keepTrack[i][j]=0;
                            }
                            else{
                                intensMatx[i][j]+=intensMatx[i-1][j-1];
                                keepTrack[i][j]=-1;
                            }
                        }else{
                            if(intensMatx[i-1][j]<=intensMatx[i-1][j+1])
                            {
                                if(intensMatx[i-1][j]<=intensMatx[i-1][j-1])
                                {
                                    intensMatx[i][j]+=intensMatx[i-1][j];
                                    keepTrack[i][j]=0;
                                }
                                else{
                                    intensMatx[i][j]+=intensMatx[i-1][j-1];
                                    keepTrack[i][j]=-1;
                                }
                            }
                            else{
                                if(intensMatx[i-1][j-1]<=intensMatx[i-1][j+1])
                                {
                                    intensMatx[i][j]+=intensMatx[i-1][j-1];
                                    keepTrack[i][j]=-1;
                                }
                                else{
                                    intensMatx[i][j]+=intensMatx[i-1][j+1];
                                    keepTrack[i][j]=1;
                                }
                            }
                            
                        }
                        if(i==intTempH-1)
                        {
//                            cout<<"\n minIndex : "<<minIndex<<"  j : "<<j<<"  flag : "<<flag;
                            if(flag)
                            {
                                intleastEleIndex=j;
                                intleastEleValue=intensMatx[i][j];
                                flag=false;
                            }
                            else
                            {
                                if(intensMatx[i][j]<intleastEleValue){
                                    intleastEleIndex=j;
                                    intleastEleValue=intensMatx[i][j];
                                }
                            }
                        }
                    }
                    }
                }
                intensMatx[i].pop_back();
                keepTrack[i].pop_back();
            }
        
            if(minIndex==0)
                minIndex=1;
            int temp=min_element(intensMatx[intTempH-1].begin(), intensMatx[intTempH-1].begin()+minIndex-1)-intensMatx[intTempH-1].begin();
            if(intensMatx[intTempH-1][temp]<intleastEleValue)
            {
                intleastEleIndex=temp;
            }
            selectedPixels.clear();
            selectedPixels.push_back(intleastEleIndex);
            for(int i=intTempH-1;i>0;i--)
            {
                if(keepTrack[i][intleastEleIndex]==0)
                    intleastEleIndex=intleastEleIndex;
                else if(keepTrack[i][intleastEleIndex]==1)
                    intleastEleIndex+=1;
                else if(keepTrack[i][intleastEleIndex]==-1)
                    intleastEleIndex-=1;
                selectedPixels.push_back(intleastEleIndex);
            }
            out_image = Mat(intTempH, intTempW-1, CV_8UC3);
            Vec3b pixel;
            for(int i=0;i<intTempH;++i){
                for(int j=0;j<intTempW-1;++j){
                    if (j >= selectedPixels.at(intTempH-i-1))
                        pixel = in_image.at<Vec3b>(i, j+1);
                    else
                        pixel = in_image.at<Vec3b>(i, j);
                    out_image.at<Vec3b>(i,j) = pixel;
                }
            }
            in_image = out_image.clone();
        }
    }
    if(in_image.rows>new_height)
    {
        intTempW = out_image.cols;
        intTempH = out_image.rows;
        vector<vector<int> > intensMatx(intTempW+1,vector<int>(intTempH+1,0));
        vector<vector<int> > keepTrack(intTempW+1,vector<int>(intTempH+1,0));
        selectedPixels.clear();
        for(int i=0;i<out_image.cols;i++)
            selectedPixels.push_back(1);
        minIndex=selectedPixels[0];
    while(in_image.rows!=new_height)
    {
         rotate(in_image, in_image, ROTATE_90_CLOCKWISE);
        Mat matBW;
        cvtColor(in_image, matBW, CV_BGR2GRAY);
        Mat matScharr_x, matScharr_y;
        
        Scharr(matBW, matScharr_x, dd, dx, dy);
        Scharr(matBW, matScharr_y, dd, dy, dx);
        magnitude(matScharr_x,matScharr_y, out_image);
        out_image=getLoc(out_image);
        
        intTempW = out_image.cols;
        intTempH = out_image.rows;
        minIndex=selectedPixels[0];
        intleastEleIndex=max(intTempH,intTempW);
        bool flag=true;
        for(int i = 0; i < intTempH; i++){
            if(selectedPixels[i]<minIndex)
                minIndex=selectedPixels[i];
            for(int j = minIndex-1; j < intTempW; j++){
                if(j>=0){
                    intensMatx[i][j] = out_image.at<uchar>(i,j);
                    if(i!=0){
                        if (j == 0) {
                            if(intensMatx[i-1][j]<= intensMatx[i-1][j+1])
                            {
                                intensMatx[i][j]+=intensMatx[i-1][j];
                                keepTrack[i][j]=0;
                            }
                            else{
                                intensMatx[i][j]+=intensMatx[i-1][j+1];
                                keepTrack[i][j]=1;
                            }
                        }
                        else if (j == intTempW-1){
                            if(intensMatx[i-1][j]<= intensMatx[i-1][j-1])
                            {
                                intensMatx[i][j]+=intensMatx[i-1][j];
                                keepTrack[i][j]=0;
                            }
                            else{
                                intensMatx[i][j]+=intensMatx[i-1][j-1];
                                keepTrack[i][j]=-1;
                            }
                        }else{
                            if(intensMatx[i-1][j]<=intensMatx[i-1][j+1])
                            {
                                if(intensMatx[i-1][j]<=intensMatx[i-1][j-1])
                                {
                                    intensMatx[i][j]+=intensMatx[i-1][j];
                                    keepTrack[i][j]=0;
                                }
                                else{
                                    intensMatx[i][j]+=intensMatx[i-1][j-1];
                                    keepTrack[i][j]=-1;
                                }
                            }
                            else{
                                if(intensMatx[i-1][j-1]<=intensMatx[i-1][j+1])
                                {
                                    intensMatx[i][j]+=intensMatx[i-1][j-1];
                                    keepTrack[i][j]=-1;
                                }
                                else{
                                    intensMatx[i][j]+=intensMatx[i-1][j+1];
                                    keepTrack[i][j]=1;
                                }
                            }
                            
                        }
                        if(i==intTempH-1)
                        {
                            if(flag)
                            {
                                intleastEleIndex=j;
                                intleastEleValue=intensMatx[i][j];
                                flag=false;
                            }
                            else
                            {
                                if(intensMatx[i][j]<intleastEleValue){
                                    intleastEleIndex=j;
                                    intleastEleValue=intensMatx[i][j];
                                    
                                }
                            }
                        }
                    }
                }
            }
            intensMatx[i].pop_back();
            keepTrack[i].pop_back();
        }
        
        if(minIndex==0)
            minIndex=1;
        int temp=min_element(intensMatx[intTempH-1].begin(), intensMatx[intTempH-1].begin()+minIndex-1)-intensMatx[intTempH-1].begin();
        if(intensMatx[intTempH-1][temp]<intleastEleValue)
        {
            intleastEleIndex=temp;
        }
        selectedPixels.clear();
        selectedPixels.push_back(intleastEleIndex);
        for(int i=intTempH-1;i>0;i--)
        {
            if(keepTrack[i][intleastEleIndex]==0)
                intleastEleIndex=intleastEleIndex;
            else if(keepTrack[i][intleastEleIndex]==1)
                intleastEleIndex+=1;
            else if(keepTrack[i][intleastEleIndex]==-1)
                intleastEleIndex-=1;
            selectedPixels.push_back(intleastEleIndex);
        }
        out_image = Mat(intTempH, intTempW-1, CV_8UC3);
        Vec3b pixel;
        for(int i=0;i<intTempH;++i){
            for(int j=0;j<intTempW-1;++j){
                if (j >= selectedPixels.at(intTempH-i-1))
                    pixel = in_image.at<Vec3b>(i, j+1);
                else
                    pixel = in_image.at<Vec3b>(i, j);
                out_image.at<Vec3b>(i,j) = pixel;
            }
        }
        rotate(out_image, out_image, ROTATE_90_COUNTERCLOCKWISE);
        in_image = out_image.clone();
        
    }//while
    }
    oimage = out_image.clone();
    return true;
}

bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){
    return true;
}
