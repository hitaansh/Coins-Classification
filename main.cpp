
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

std::string filename = "";
cv::Mat ORIGINAL,BW,BLUR,EDGE,TEMP;
cv::Mat labels, cens;
vector<Vec3f> circles, temp;
vector<tuple<double, int>> groups;
vector<double> radii;
char midist[50];
int center;
int distanceslider = 1;
int threshold1slider = 100;
int threshold2slider = 50;
const int distanceslidermax = 100;
const int threshold1max = 2000;
const int threshold2max = 1000;


void readPPM() {
    //std::cout << "Enter ppm file: ";
    // std::cin >> filename;
    ORIGINAL = cv::imread(filename, 1);
    // cv::imshow("original image: ", ORIGINAL);
    //cv::waitKey(2000);
    std::cout << std::endl;
    //cout << "file name entered: " << filename << endl;
}

void convertBW() {
    cv::cvtColor(ORIGINAL, BW, cv::COLOR_BGR2GRAY);
    //cv::imshow("black and white: ", BW);
    //cv::waitKey(2000);
}

void blur() {
    //cv::bilateralFilter(BW, BLUR, 25, 110, 110);
    cv::GaussianBlur(BW, BLUR, cv::Size(45,45), 5);
    
    
    //
    //Mat element = cv::getStructuringElement(1, Size(11,11), Point(-1, -1));
    //cv::morphologyEx(BLUR, BLUR, cv::MORPH_GRADIENT, element);
    //cv::morphologyEx(BLUR, BLUR, cv::MORPH_OPEN, element);
    // cv::GaussianBlur(BLUR, BLUR, <#Size ksize#>, <#double sigmaX#>)
    // for(int x =0; x)
    
    //cv::imshow("blured: ", BLUR);
    //cv::waitKey(0);
}

void canny() {
    cv::Canny(BLUR, EDGE, 45, 45);
    // cv::Canny(BW, TEMP, 45, 45);
    cv::imshow("canny edge: ", EDGE);
    //cv::waitKey(0);
}

void clear() {
    circles.clear();
    ORIGINAL = cv::imread(filename, 1);
    cv::Canny(BLUR, EDGE, 45, 45);
    
}
static void on_trackbar(int, void* ){
    //cout << "at on trackbar" << endl;
//    cv::imshow("CircleDrawing", EDGE);
//    int rand1 = rand() * 100 + 1;
//    if (rand1 < 75) {
//        cout << "";
//    }
//    else {
        //cout << "gonna do hough" << endl;
    clear();
    // 43,165
    cout << "Threshold1: " << threshold1slider << " Threshold2: " << threshold2slider << endl;
        HoughCircles(BLUR, circles, HOUGH_GRADIENT, 5, 125, threshold1slider, threshold2slider,50,150);
        //cout << "finished hough" << endl;
        for (const auto& a: circles) {
            cv::circle(ORIGINAL, Point(a[0],a[1]),a[2], Scalar(255,0,0), 10);
        }
        cv::imshow("CircleDrawing", ORIGINAL);
        
   // }
}

void houghcirclesfinder() {
    cv::namedWindow("CircleDrawing");
    //cv::createTrackbar("Minimum Distance", "CircleDrawing", &distanceslider, distanceslidermax, on_trackbar);
    cv::createTrackbar("Threshold1", "CircleDrawing", &threshold1slider, threshold1max, on_trackbar);
    cv::createTrackbar("Threshold2", "CircleDrawing", &threshold2slider, threshold2max, on_trackbar);
    on_trackbar(0, 0);
    waitKey(0);
    //HoughCircles(BW, circles, HOUGH_GRADIENT, 5, BW.rows/10, 45, 45, 10, 100);
    //cout << "created trackbars: " << endl;
    //cv::imshow("CircleDrawing", EDGE);
    //cout << "hello after" << endl;
    //print(circles);
}

void houghcircles() {
    clear();
    HoughCircles(BLUR, circles, HOUGH_GRADIENT, 5, 150, 43, 206, 50, 130);
    // 43, 206
    // 49, 130
}


void drawcircles() {
    //if(circles.size() <= 70) {
      //  cout << "circle size: " << circles.size() << endl;
    for (const auto& a: circles) {
        cv::circle(ORIGINAL, Point(a[0],a[1]),a[2], Scalar(255,0,0), 10);
    }
    //cout << "drawn circles" << endl;
    // cv::imshow("Circles drawn", ORIGINAL);
    cv::waitKey(10000);
 //   }
//    else {
//        cout << "too many circles " << circles.size() << endl;
//    }
}

void printVector3() {
    //cout << "hello" << endl;
    cout << circles.size() << endl;
    for (const auto& a: circles) {
        cout << "X center coord: "<< a[0] << " Y center coord: " << a[1] << " Radius: " << a[2] << endl;
    }
}

void kmeanscluster() {
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

void drawgroups() {
    // penny is blue (255, 0, 0)
    // nickle is green (0, 255, 0)
    // quarter is red (0, 0, 255)
    Mat output;
    cv::cvtColor(BW, output, cv::COLOR_GRAY2BGR);
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
    //imshow("amount" , output);
    //waitKey(0);
    
}

void dollaramount() {
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
    //cout << "at main" << endl;
    filename = argv[1];
    //cout << filename << endl;
    readPPM();
    convertBW();
    blur();
    // canny();
    //houghcirclesfinder();
    houghcircles();
    // printVector3();
    //drawcircles();
    kmeanscluster();
    dollaramount();
    drawgroups();

    return 0;
    
}
