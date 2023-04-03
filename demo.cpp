#include "Yolov5.h"

int main(){

    cv::VideoCapture video_capture("test.avi");

    cv::Mat frame;

    Yolov5* demo_yolov5_detector = new Yolov5("/home/robomaster/deep-robomaster/best/best.xml", "/home/robomaster/deep-robomaster/best/best.bin", 640, 640);

    // init
    demo_yolov5_detector->init_yolov5_detector();
    // use video to demo

    // demo_yolov5_detector->detect_yolov5(demo_mat);
    while(video_capture.read(frame)){
        demo_yolov5_detector->detect_yolov5(frame);
        demo_yolov5_detector->show_res();
    }
    return 0;
}