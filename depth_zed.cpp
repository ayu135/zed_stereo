#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>

//opencv includes
#include <opencv2/opencv.hpp>

//ZED Includes
#include <zed/Camera.hpp>





//main  function

int main(int argc, char **argv) {

    
    sl::zed::SENSING_MODE dm_type = sl::zed::RAW;
    sl::zed::Camera* zed;

    
    zed = new sl::zed::Camera(sl::zed::HD1080);
    

    int width = zed->getImageSize().width;
    int height = zed->getImageSize().height;

    sl::zed::ERRCODE err = zed->init(sl::zed::MODE::PERFORMANCE, 0, true);

    // ERRCODE display
    std::cout << sl::zed::errcode2str(err) << std::endl;


    // Quit if an error occurred
    if (err != sl::zed::SUCCESS) {
        delete zed;
        return 1;
    }

    char key = ' ';
    int ViewID = 2;
    int count = 0;
    int ConfidenceIdx = 100;

    bool DisplayDisp = true;
    bool displayConfidenceMap = false;

    cv::Mat disp(height, width, CV_8UC4);
    cv::Mat anaplyph(height, width, CV_8UC4);
    cv::Mat confidencemap(height, width, CV_8UC4);

    cv::Size DisplaySize(720, 404);
    cv::Mat dispDisplay(DisplaySize, CV_8UC4);
    cv::Mat anaplyphDisplay(DisplaySize, CV_8UC4);
    cv::Mat confidencemapDisplay(DisplaySize, CV_8UC4);

    /* Init mouse callback */
    sl::zed::Mat depth;
    zed->grab(dm_type);
    depth = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH); // Get the pointer
    // Set the structure
    mouseStruct._image = cv::Size(width, height);
    mouseStruct._resize = DisplaySize;
    mouseStruct.data = (float*) depth.data;
    mouseStruct.step = depth.step;
    mouseStruct.name = "DEPTH";
    /***/

    //create Opencv Windows
    cv::namedWindow(mouseStruct.name, cv::WINDOW_AUTOSIZE);
    
    cv::namedWindow("VIEW", cv::WINDOW_AUTOSIZE);

    std::cout<< "Press 'q' to exit"<<std::endl;


    //loop until 'q' is pressed
    while (key != 'q') {
        // DisparityMap filtering
        //zed->setDispReliability(reliabilityIdx); !!function name has been change in Release 0.8 --see ChangeLog
        zed->setConfidenceThreshold(ConfidenceIdx);

        // Get frames and launch the computation
        bool res = zed->grab(dm_type);

        depth = zed->retrieveMeasure(sl::zed::MEASURE::DEPTH); // Get the pointer

       

        /***************  DISPLAY:  ***************/
        // Normalize the DISPARITY / DEPTH map in order to use the full color range of grey level image
        if (DisplayDisp)
            slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DISPARITY)).copyTo(disp);
        else
            slMat2cvMat(zed->normalizeMeasure(sl::zed::MEASURE::DEPTH)).copyTo(disp);


        // To get the depth at a given position, click on the DISPARITY / DEPTH map image
        cv::resize(disp, dispDisplay, DisplaySize);
        

        

        //Even if Left and Right images are still available through getView() function, it's better since v0.8.1 to use retrieveImage for cpu readback because GPU->CPU is done async during depth estimation.
        // Therefore :
        // -- if disparity estimation is enabled in grab function, retrieveImage will take no time because GPU->CPU copy has already been done during disp estimation
        // -- if disparity estimation is not enabled, GPU->CPU copy is done in retrieveImage fct, and this function will take the time of copy.
        if (ViewID == sl::zed::STEREO_LEFT || ViewID == sl::zed::STEREO_RIGHT)
            slMat2cvMat(zed->retrieveImage(static_cast<sl::zed::SIDE> (ViewID))).copyTo(anaplyph);
        else
            slMat2cvMat(zed->getView(static_cast<sl::zed::VIEW_MODE> (ViewID))).copyTo(anaplyph);

        cv::resize(anaplyph, anaplyphDisplay, DisplaySize);
        imshow("VIEW", anaplyphDisplay);

        key = cv::waitKey(5);

        // Keyboard shortcuts
        
        ConfidenceIdx = ConfidenceIdx < 1 ? 1 : ConfidenceIdx;
        ConfidenceIdx = ConfidenceIdx > 100 ? 100 : ConfidenceIdx;
    }

    delete zed;
    return 0;
}