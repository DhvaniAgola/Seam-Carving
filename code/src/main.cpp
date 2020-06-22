//reference : Urvi Mali
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;
int intColumns;
int intRows;
int intNodes;
vector<int> anscestor;
vector<bool> visited;

 void showOutputImage(Mat &output_image,string output_img_name)
{
    for(int i=0;i<intRows;i++){
        for(int j=0;j<intColumns;j++){
            Vec3b pixel = output_image.at<Vec3b>(i, j);
            if(visited[i*intColumns+j]){
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
            }else{
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
            output_image.at<Vec3b>(i, j) = pixel;
        }
    }
    namedWindow( output_img_name, WINDOW_AUTOSIZE );
    imwrite( output_img_name, output_image);
    imshow( output_img_name, output_image );
}
bool canTraverse(vector<pair<int, double> > weights[]){
    
    queue<int> waitQ;
    int s=intNodes-2;
    int t=intNodes-1;
    waitQ.push(s);
    while (waitQ.size()!=0) {
        s = waitQ.front();
        waitQ.pop();
        for (int i = 0; i < weights[s].size(); i++) {
            if (!visited[weights[s][i].first] && weights[s][i].second >0) {
                visited[weights[s][i].first] = true;
                anscestor[weights[s][i].first] = s;
                waitQ.push(weights[s][i].first);
            }else if((weights[s][i].first == t)&&weights[s][i].second>0){
                visited[weights[s][i].first] = true;
                anscestor[weights[s][i].first] = s;
                return true;
            }
        }
    }
    return visited[t];
}
void generateRecidualGraph(vector<pair<int,double> > weights[],double minFlow)
{
    int s=intNodes-2;
    int t=intNodes-1;
    int firstAnscestor_t;
    while (true) {
        if (t == s)
            break;
        firstAnscestor_t = anscestor[t];
        for(int i = 0   ; i < weights[firstAnscestor_t].size(); i++){
            if (weights[firstAnscestor_t][i].first == t){
                weights[firstAnscestor_t][i].second-=minFlow;
                break;
            }
        }
        for(int i = 0   ; i < weights[t].size(); i++){
            if (weights[firstAnscestor_t][i].first == firstAnscestor_t){
                weights[t][i].second+=minFlow;
                break;
            }
        }
        t = firstAnscestor_t;
    }
}
double getMinWeightFromList(vector<pair<int,double> > weights[])
{
    double minFlow=LONG_MAX;
     int t = intNodes-1;
    int s=intNodes-2;
    int firstAnscestor_t;
    int nodeToBeTraveresedIndex;
    while (t!= s){
        firstAnscestor_t= anscestor[t];
        nodeToBeTraveresedIndex=-1;
        for (int i = 0; i < weights[firstAnscestor_t].size(); i++) {
            if (weights[firstAnscestor_t][i].first == t)
            {
                minFlow = min(minFlow,weights[firstAnscestor_t][i].second);
                break;
            }
        }
        t = firstAnscestor_t;
    }
    return minFlow;
}

void assignWeights(Mat& srcGray,vector<pair<int, double> > weights[]){
     Mat tempImage=srcGray;
    int intRowsTemp = tempImage.rows;
    int intColsTemp = tempImage.cols;
    
    int i,j;
    int aPoint;
    double intensity;
    double intensityThreshold=0.5;
    int mWeight=INT_MAX;
     double intensityAtPoint=0;
    for( i = 0; i < intRowsTemp; ++i)
    {
        for ( j = 0; j < intColsTemp; ++j)
        {
            aPoint = i*intColsTemp+j;
            intensityAtPoint=tempImage.at<uchar>(i,j);
            if (i-1>0) {
                 intensity = (intensityAtPoint - tempImage.at<uchar>(i-1, j));
                if(intensity<intensityThreshold)
                    weights[aPoint].push_back(make_pair((i-1)*intColsTemp+j,mWeight));
                else
                    weights[aPoint].push_back(make_pair((i-1)*intColsTemp+j,1));
                }
            if (i+1< intRowsTemp){
                intensity = (intensityAtPoint - tempImage.at<uchar>(i+1, j));
                if(intensity<intensityThreshold)
                    weights[aPoint].push_back(make_pair((i+1)*intColsTemp+j,mWeight));
                else
                    weights[aPoint].push_back(make_pair((i+1)*intColsTemp+j,1));
                }
            if (j+1<intColsTemp){
                intensity = (intensityAtPoint - tempImage.at<uchar>(i, j+1));
                if(intensity<intensityThreshold)
                    weights[aPoint].push_back(make_pair(i*intColsTemp+(j+1),mWeight));
                else
                    weights[aPoint].push_back(make_pair(i*intColsTemp+(j+1),1));
            }
            if (j-1>0){
                //not ideal but i have used
                intensity = (intensityAtPoint - tempImage.at<uchar>(i, j-1));
                if(intensity<intensityThreshold)
                    weights[aPoint].push_back(make_pair(i*intColsTemp+(j-1),mWeight));
                else
                     weights[aPoint].push_back(make_pair(i*intColsTemp+(j-1),1));}
        }
    }
}
void preTraverse(vector<pair<int, double> > weights[],Mat &out_image,string output_img_name){
    for(int i=0;i<intNodes;i++){
        visited.push_back(false);
        anscestor.push_back(-1);
    }
    while (canTraverse(weights)) {
        visited.clear();
        
        for (int i = 0; i < intNodes; i++)
            visited.push_back(false);
        double minFlow;
        minFlow=getMinWeightFromList(weights);
        generateRecidualGraph(weights,minFlow);
    }
    showOutputImage(out_image,output_img_name);
    
}
void processImage(Mat& in_image,Mat& output_image,vector<pair<int, double> > weights[],string output_img_name){
    Mat org_img,cvtColor_img,cvtColor_img_cloned,norm_img,scharr_col,scharr_row,scale_col,scale_row;
    org_img = in_image;
    GaussianBlur(org_img,org_img,Size(3,3),0,0,BORDER_DEFAULT);
    cvtColor(org_img,cvtColor_img,CV_BGR2GRAY);
    cvtColor_img_cloned = cvtColor_img.clone();
    normalize(cvtColor_img, norm_img, 0, 255,NORM_MINMAX,CV_8U);
    Scharr(cvtColor_img_cloned, scharr_col, CV_16S, 1, 0);
    Scharr(cvtColor_img_cloned, scharr_row, CV_16S, 0, 1);
    convertScaleAbs(scharr_col,scale_col);
    convertScaleAbs(scharr_row,scale_row);
    addWeighted( scale_col, 0.5, scale_row, 0.5, 0, cvtColor_img_cloned );
    assignWeights(norm_img,weights);
     preTraverse(weights,output_image,output_img_name);
}

int main(int c,char* args[])
{
    Mat input_img;
    ifstream in(args[1]);
    
    input_img = imread(args[1]);///Users/dhvaniagola/Desktop/config2.txt
//     input_img = imread("/Users/dhvaniagola/Desktop/simple.png");
    if(!input_img.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }
    
    if(input_img.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<input_img.depth()<<endl;
        return -1;
    }
    Mat output_img = input_img.clone();
    intColumns = input_img.cols;
    intRows = input_img.rows;
    intNodes = intColumns*intRows+2;
    vector<pair<int, double> > weights[intNodes];
//    cout<<"rows:"<<intRows<<"  cols:"<<intColumns<<"\n";
    int n;
    
    fstream config(args[2]);
//    fstream config("/Users/dhvaniagola/Desktop/config.txt");
    
        if(!config){
            cout<<"Could not load initial mask file!!!"<<endl;
            return -1;
        }
    string output_image_name=args[3];
//    string output_image_name="/Users/dhvaniagola/Desktop/output.png";
    config>>n;
    
      int scal_x,scal_y,B_Fbit;
    for(int i=0;i<n;++i){
        config>>scal_y>>scal_x>>B_Fbit;
        
        if(scal_y<0 || scal_y>=intColumns || scal_x<0 || scal_x>=intRows){
            cout<<"I valid pixel mask!"<<endl;
            return -1;
        }
        if (B_Fbit==1) {
            weights[intNodes-2].push_back(make_pair(scal_x*intColumns+scal_y, INT_MAX));
            weights[scal_x*intColumns+scal_y].push_back(make_pair(intNodes-2, -1));
        }else{
            weights[scal_x*intColumns+scal_y].push_back(make_pair(intNodes-1, INT_MAX));
            weights[intNodes-1].push_back(make_pair(scal_x*intColumns+scal_y, -1));
        }
    }
    processImage(input_img,output_img,weights,output_image_name);
    waitKey(0);
    return 0;
}
