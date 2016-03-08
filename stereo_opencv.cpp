#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(){
	Mat img1, img2, g1, g2;
	Mat disp, disp8;

	img1 = imread("im7.png",0);
	img2 = imread("im8.png",0);
	

	StereoBM sbm;
	sbm.state->SADWindowSize = 9;
	sbm.state->numberOfDisparities = 112;
	sbm.state->preFilterSize = 5;
	sbm.state->preFilterCap = 61;
	sbm.state->minDisparity = -39;
	sbm.state->textureThreshold = 507;
	sbm.state->uniquenessRatio = 0;
	sbm.state->speckleWindowSize = 0;
	sbm.state->speckleRange = 8;
	sbm.state->disp12MaxDiff = 1;

	sbm(img1,img2, disp);
	normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);
	equalizeHist( disp8, disp );
	// namedWindow( "Display window", WINDOW_AUTOSIZE );
	imwrite("disp3.png", disp);
	printf("Success");

	return 0;






}