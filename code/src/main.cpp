#include <opencv2/opencv.hpp>

#include "sc.h"
using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
   
        if(argc!=5){
            cout<<"Usage: ../sc input_image new_width new_height output_image"<<endl;
            return -1;
        }
    
    
    Mat in_image;
    in_image = imread(argv[1]);
    if(!in_image.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }
    
    if(in_image.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
        return -1;
    }
    
        int new_width = atoi(argv[2]);
        int new_height = atoi(argv[3]);
   
    Mat out_image;
    
    if(!seam_carving(in_image, new_width, new_height, out_image)){
        return -1;
    }
    
       imwrite( argv[4], out_image);
   
    namedWindow( "Output Image", WINDOW_AUTOSIZE );
    imshow( "Output Image", out_image );
    waitKey(0);
    return 0;
}
