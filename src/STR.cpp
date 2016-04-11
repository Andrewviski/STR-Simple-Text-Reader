#include <opencv2/opencv.hpp>
#include <stdio.h>
using namespace std;
using namespace cv;

void help() {
    puts("--help    display this message");
    puts("--pic     specify a picture as an input");
}
int size=20;
Mat frame;
std::vector<cv::Rect> detectLetters(cv::Mat img) {
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(size, size));
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly(contours.size());
    for (int i = 0; i < contours.size(); i++)
        if (contours[i].size() > 100) {
            cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
            cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
            if (appRect.width > appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}

void on_trackbar(int, void*) {
    Mat cpy; 
    frame.copyTo(cpy);
    std::vector<cv::Rect> letterBBoxes = detectLetters(cpy);
    for (int i = 0; i < letterBBoxes.size(); i++)
        cv::rectangle(cpy, letterBBoxes[i], cv::Scalar(0, 255, 0), 3, 8, 0);
    imshow("Main display", cpy);
}

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "help") == 0)
        help(), exit(0);
    if (argc > 1) {
        frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
        namedWindow("Main display", CV_WINDOW_OPENGL);
        createTrackbar("Structering Size", "Main display", &size, 100, on_trackbar);
        on_trackbar(size, 0);
        waitKey(0);
    } else {
        puts("Reading camera stream...");
        VideoCapture cap(0);
        if (!cap.isOpened())
            puts("Error reading from video stream");
        while (1) {
            cap >> frame; // get a new frame from camera
            std::vector<cv::Rect> letterBBoxes = detectLetters(frame);
            for (int i = 0; i < letterBBoxes.size(); i++)
                cv::rectangle(frame, letterBBoxes[i], cv::Scalar(0, 255, 0), 3, 8, 0);
            imshow("Main display", frame);
            if (waitKey(30) >= 0)
                break;
        }
    }
}