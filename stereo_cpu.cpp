#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#define WIN_SIZE 9
#define MAX_SHIFT 63

using namespace cv;

Mat getDisparity(Mat h_inLeft, Mat h_inRight){
  // set board width to half SAD window
  int i,j;
  int imageHeight = h_inLeft.rows;
  int imageWidth =h_inLeft.cols;
  int marginWidth = WIN_SIZE / 2;
  int marginHeight = WIN_SIZE / 2;
  // set height of padded image
  int paddedHeight = imageHeight + marginWidth * 2;
  // set width of padded image
  int paddedWidth = imageWidth + marginWidth * 2;
  // memory size (Byte) of the padded image
  int mem_size_paddedRight = sizeof(unsigned char) * paddedHeight * paddedWidth;
  int mem_size_paddedLeft = mem_size_paddedRight;
  // malloc for padded left image
  Mat* paddedLeft = (unsigned char*) malloc(mem_size_paddedLeft);
  // malloc for padded right image
  Mat* paddedRight = (unsigned char*) malloc(mem_size_paddedRight);

  // initial the padded image and shifted image to '0'
  memset(paddedLeft, 0, mem_size_paddedLeft);
  memset(paddedRight, 0, mem_size_paddedRight);

  // copy the image to the padded image
  for(i = 0; i < imageHeight; i++){
    for(j = 0; j < imageWidth; j++){
      paddedLeft[(marginHeight+i)*paddedWidth + marginWidth + j] =	\
	h_inLeft[i*imageWidth + j];
      paddedRight[(marginHeight+i)*paddedWidth + marginWidth + j] =	\
	h_inRight[i*imageWidth + j];
    }
  }

  // malloc for shifted right image
  Mat shiftedRight;
  // malloc for SSD image, use integer (4 Byte) to avoid over flow
  int mem_size_ssd = sizeof(unsigned int) * paddedHeight * paddedWidth;
  unsigned int* ssd = (unsigned int*) malloc(mem_size_ssd);
  // malloc for window over SSD
  int mem_size_winssd = sizeof(unsigned int) * imageHeight * imageWidth;
  unsigned int* winssd = (unsigned int*) malloc(mem_size_winssd);
  // minimun of window over SSD
  int mem_size_minssd = sizeof(unsigned int) * imageHeight * imageWidth;
  unsigned int* minssd = (unsigned int*) malloc(mem_size_minssd);
  // disparity map
  int mem_size_disparity = sizeof(unsigned char) * imageHeight * imageWidth;
  Mat disparity;
  // the shift value of the right image
  int shift;

  memset(shiftedRight, 0, mem_size_paddedRight);
  // initialize SSD to xFFFFFFFF
  memset(ssd, 255, mem_size_ssd);
  // initialize window over SSD to xFFFFFFFF
  memset(winssd, 255, mem_size_winssd);
  // initialize min window SSD to xFFFFFFFF
  memset(minssd, 255, mem_size_minssd);
  // initialize the disparity map to 0
  memset(disparity, 0, mem_size_disparity);

  for (shift = 0; shift < MAX_SHIFT; shift++){

    /**************************************************
     * For each shift value, perform:
     * 1. Shift the right image
     * 2. Wondow SSD/SAD between left and shifted right image
     * 3. Find the shift value that leads to minimun SSD 
     *************************************************/

    // make a shifted right image
    for(i = 0; i < paddedHeight; i++){
      for(j = 0; j < paddedWidth - shift - marginWidth; j++){
	shiftedRight[i*paddedWidth + shift + j] =	\
	  paddedRight[i*paddedWidth + j];
      }
    }

    // SSD/SAD
    for(i = 0; i < paddedHeight; i++){
      for(j = 0; j < paddedWidth; j++){
	int index = i * paddedWidth+j;
	int diff = paddedLeft[index] - shiftedRight[index];
	
	ssd[index] = (unsigned int)abs(diff);
	
      }
    }

    // accumulate SSD over a window
    for(i = 0; i < imageHeight; i++){
      for(j = 0; j < imageWidth; j++){
	// indexwinssd: index to the center of the window over ssd
	int indexwinssd = i * imageWidth+j;
	// accu is used to accumulate the sum over the window
	unsigned int accu = 0;
	// loop over the window
	for(int ii = -1*marginHeight; ii < marginHeight + 1; ii++){
	  for(int jj = -1*marginWidth; jj < marginWidth + 1; jj++){
	    // indexssd: index to the ssd position for accumulating the window
	    int indexssd = (i + marginHeight + ii)*paddedWidth + j + marginWidth + jj;
	    accu = accu + ssd[indexssd];
	  }//loop over window width
	}//loop over window height
	winssd[indexwinssd] = accu;
      }// loop over imageWidth
    }// loop over imageHeight

    // update the minimun of window over SSD and corresponding shift value
    for(i = 0; i < imageHeight; i++){
      for(j = 0; j < imageWidth; j++){
	int index = i * imageWidth + j;
	if(winssd[index] < minssd[index]){
	  minssd[index] = winssd[index];
	  disparity[index] = (unsigned char)shift;
	}
      }
    }
      
  }
  return disparity;
}

int main(){
	Mat disparity,h_inLeft,h_inRight;
    h_inLeft = imread( "/cones/im2.png", 0 );
    h_inRight = imread("/cones/im6.png",0);
    disparity = getDisparity(h_inLeft,h_inRight);
    namedWindow("Disparity",WINDOW_AUTOSIZE);
    imshow("Disparity",disparity);


}