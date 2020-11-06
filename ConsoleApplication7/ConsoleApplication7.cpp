// Start skeleton workshop lesweek 6: OpenCV workshop

#define STRICT

#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <opencv2/videoio.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <tchar.h>


using namespace cv;
using namespace std;



const int max_value_H = 360 / 2;
const int max_value = 255;
const String window_capture_name = "Video";
const String window_detection_name = "Mask";
int low_H = 0, low_S = 22, low_V = 109;
int high_H = 28, high_S = 158, high_V = 210;



static void on_low_H_thresh_trackbar(int, void*)
{
    low_H = min(high_H - 1, low_H);
    setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void*)
{
    high_H = max(high_H, low_H + 1);
    setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void*)
{
    low_S = min(high_S - 1, low_S);
    setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void*)
{
    high_S = max(high_S, low_S + 1);
    setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void*)
{
    low_V = min(high_V - 1, low_V);
    setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void*)
{
    high_V = max(high_V, low_V + 1);
    setTrackbarPos("High V", window_detection_name, high_V);
}

int geheugen[];



#define USE_CAMERA


int main()
{
#ifdef USE_CAMERA
    VideoCapture cap;
    int deviceId = 2;
    int apiId = cv::CAP_ANY;
    cap.open(deviceId + apiId);



    if (!cap.isOpened()) {
        cerr << "Can't open camera" << endl;
        return -1;
    }
#else



#endif



    namedWindow(window_detection_name);
    namedWindow(window_capture_name);



    // Trackbars to set thresholds for HSV values
    createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);



    while (true) {



        Mat src;



#ifdef USE_CAMERA
        cap.read(src);
        if (src.empty()) {
            cerr << "Error! blank frame" << endl;
        }
#else
        src = imread("oranje_pingpongbal.jpg", IMREAD_COLOR);
#endif  USE_CAMERA



        Mat blurred;
        GaussianBlur(src, blurred, Size(3, 3), 0);



        Mat hsv;
        cvtColor(blurred, hsv, COLOR_BGR2HSV);

       

        Mat mask;
       // inRange(hsv, Scalar(0, 22, 109), Scalar(28, 158, 211), mask);
        inRange(hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);
        erode(mask, mask, 2);
        dilate(mask, mask, 2);




        // Vind aaneengesloten punten ....
        vector<vector<Point>> contours;
        findContours(mask.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);



        // New plaatje
        Mat contourImage = Mat::zeros(src.size(), CV_8UC3);



        // Center of Gravity
        
        vector<Moments> mu(contours.size());
        for (int idx = 0; idx < contours.size(); idx++) {
            mu[idx] = moments(contours[idx], false);
        }



        for (int idx = 0; idx < mu.size(); idx++) {
            Point center(mu[idx].m10 / mu[idx].m00, mu[idx].m01 / mu[idx].m00);
            circle(contourImage, center, 2, Scalar(255, 0, 0), FILLED);
        }
        


        // Filteren
        std::sort(
            contours.begin(),
            contours.end(),
            [](vector<Point>& a, vector<Point>& b) {return a.size() < b.size(); }
        );

        if (contours.size() > 0) {


            Moments m = moments(contours[contours.size() - 1], false);
            Point center(m.m10 / m.m00, m.m01 / m.m00);
            circle(src, center, 10, Scalar(0, 255, 255), FILLED);

         
            cout << center << endl;



            imshow("Source", src);
            imshow(window_detection_name, mask);
            imshow(window_capture_name, hsv);
            imshow("Contour", contourImage);



            if (waitKey(5) >= 0)
                break;
        }
    }



}