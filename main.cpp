
//  main.cpp
//  help
//
//  Created by Hitaansh Gaur on 1/30/20.
//  Copyright © 2020 Hitaansh Gaur. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

string filename = "";
cv::Mat ORIGINAL,BW,BLUR,EDGE,TEMP;
cv::Mat labels, cens;
vector<Vec3f> circles, temp;
vector<tuple<double, int>> groups;
vector<double> radii;
int distanceSlider = 1;
int firstThresholdSlider = 100;
int secondThresholdSlider = 50;
const int distanceSliderMax = 100;
const int firstThresholdMax = 2000;
const int secondThresholdMax = 1000;

// reads in the image
void readPPM() {
    ORIGINAL = cv::imread(filename, 1);
    
    // cv::imshow("original image: ", ORIGINAL);
    // cv::waitKey(2000);
    
    //cout << "file name entered: " << filename << endl;
}


// converts the image to black and white
void convertBW() {
    cv::cvtColor(ORIGINAL, BW, cv::COLOR_BGR2GRAY);
    
    //cv::imshow("black and white: ", BW);
    //cv::waitKey(2000);
}


// blurs the image (needed to remove noise)
void blur() {
    cv::GaussianBlur(BW, BLUR, cv::Size(45,45), 5);
    
    /* Different Blur and Filtering Techniques*/
    // cv::bilateralFilter(BW, BLUR, 25, 110, 110);
    // Mat element = cv::getStructuringElement(1, Size(11,11), Point(-1, -1));
    // cv::morphologyEx(BLUR, BLUR, cv::MORPH_GRADIENT, element);
    // cv::morphologyEx(BLUR, BLUR, cv::MORPH_OPEN, element);
    // cv::GaussianBlur(BLUR, BLUR, <#Size ksize#>, <#double sigmaX#>)
    
    //cv::imshow("blured: ", BLUR);
    //cv::waitKey(0);
}

// canny edge detection (don't need this)
void canny() {
    cv::Canny(BLUR, EDGE, 45, 45);
    
    cv::imshow("canny edge: ", EDGE);
    cv::waitKey(0);
}

// removes the circles drawn on the image
void clear() {
    circles.clear();
    ORIGINAL = cv::imread(filename, 1);
    cv::Canny(BLUR, EDGE, 45, 45);
    
}

// this the method that implements the trackbar
static void on_trackbar(int, void* ){
    
    clear();
    cout << "Threshold1: " << firstThresholdSlider << " Threshold2: " << secondThresholdSlider << endl;
    HoughCircles(BLUR, circles, HOUGH_GRADIENT, 5, 125, firstThresholdSlider, secondThresholdSlider,50,150);
    for (const auto& a: circles) {
        cv::circle(ORIGINAL, Point(a[0],a[1]),a[2], Scalar(255,0,0), 10);
    }
    cv::imshow("CircleDrawing", ORIGINAL);

}

// initializes the trackbars and calls the method
void houghCirclesFinder() {
    cv::namedWindow("CircleDrawing");
    //cv::createTrackbar("Minimum Distance", "CircleDrawing", &distanceSlider, distanceSliderMax, on_trackbar);
    cv::createTrackbar("Threshold1", "CircleDrawing", &firstThresholdSlider, firstThresholdMax, on_trackbar);
    cv::createTrackbar("Threshold2", "CircleDrawing", &secondThresholdSlider, secondThresholdMax, on_trackbar);
    on_trackbar(0, 0);
    waitKey(0);
}

// runs houghcircles with the appropriate parameters
void houghCircles() {
    clear();
    HoughCircles(BLUR, circles, HOUGH_GRADIENT, 5, 150, 43, 206, 50, 130);
    // 43, 206
    // 49, 130
}

// draws circles detected by hough circles
void drawCircles() {
    for (const auto& a: circles) {
        cv::circle(ORIGINAL, Point(a[0],a[1]),a[2], Scalar(255,0,0), 10);
    }
    cv::waitKey(0);
}

// prints the center coordinates for the circles
void printVector3() {
    cout << circles.size() << endl;
    for (const auto& a: circles) {
        cout << "X center coord: "<< a[0] << " Y center coord: " << a[1] << " Radius: " << a[2] << endl;
    }
}

// runs kmeans on the circle radii to seperate them into groups
void kMeansCluster() {
    Mat points(circles.size(), 1, CV_32F);
    for (int x = 0; x < circles.size(); x++) {
        points.at<float>(x) = circles[x][2];
    }
    
    // cv::KmeansFlags c = cv::KMEANS_RANDOM_CENTERS;
    cv::TermCriteria crit = cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 15, .5);
    cv::kmeans(points, 3, labels, crit, 3, cv::KMEANS_RANDOM_CENTERS, cens);
    //points.convertTo(temp, CV_8U);
    for(int x = 0; x < circles.size(); x++) {
        groups.push_back(make_tuple(circles[x][2], labels.at<int>(x)));
        //cout << "Radius: " << circles[x][2] << " " << "Label: " << labels.at<int>(x) << e ndl;
    }
}

// draws the coin groups on the image
void drawGroups() {
    // penny is blue (255, 0, 0)
    // nickle is green (0, 255, 0)
    // quarter is red (0, 0, 255)
    
    Mat output = ORIGINAL;
    // cv::cvtColor(BW, output, cv::COLOR_GRAY2BGR);
    for(int x = 0; x < circles.size(); x++) {
        int current_label = labels.at<int>(x);
        if(current_label == 0) {
            cv::circle(output, Point(circles[x][0], circles[x][1]), circles[x][2],Scalar(255,0,0),10);
        }
        else if(current_label == 1) {
            cv::circle(output, Point(circles[x][0], circles[x][1]), circles[x][2],Scalar(0,255,0),10);
        }
        else if(current_label == 2) {
            cv::circle(output, Point(circles[x][0], circles[x][1]), circles[x][2],Scalar(0,0,255),10);
        }
    }
    
    cv::imwrite("./bw.jpg", output);
    imshow("amount" , output);
    waitKey(0);
    
}

// calculates the dollar amount present in the picture
void dollarAmount() {
    double penny = 0.01;
    double nickle = 0.05;
    double quarter = 0.25;
    double finalamount = 0.00;
    
    for(int x = 0; x < circles.size(); x++) {
        int currgroup = labels.at<int>(x);
        if(currgroup == 0)
            finalamount += penny;
        else if(currgroup == 1)
            finalamount += nickle;
        else if(currgroup == 2)
            finalamount += quarter;
    }
    cout << "Money amount: " << finalamount << endl;
}

int main(int argc, char **argv)
{
    filename = argv[1];
    
    // read in the image
    readPPM();
    
    // convert it to black and white
    convertBW();
    
    // blur the image
    blur();
    
    // run canny edge detection on the image
    // canny();
    
    // help find paramteters for hough circles
    // houghcirclesfinder();
    
    // run hough circles
    houghCircles();
    
    // print coins radii
    // printVector3();
    
    // draw detected circles on image
    // drawcircles();
    
    // cluster coins according to radii
    kMeansCluster();
    
    // calcualte dollar amount present in picture
    dollarAmount();
    
    // draw the coins classification on the image
    drawGroups();

    return 0;
}
