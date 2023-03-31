#include "Yolov5.h"

int main(){
    cv::VideoCapture video_capture("input.mp4");

    cv::Mat frame;

    Yolov5* demo_yolov5_detector = new Yolov5("/home/robomaster/deep-robomaster/best/best.xml", "/home/robomaster/deep-robomaster/best/best.bin");

    // init
    demo_yolov5_detector->init_yolov5_detector();
    // use video to demo
    while(video_capture.read(frame)){
        
    }
}