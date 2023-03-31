#include "Yolov5.h"

int main(){
    cv::VideoCapture video_capture("input.mp4");

    cv::Mat frame;

    Yolov5* demo_yolov5_detector = new Yolov5();

    // use video to demo
    while(video_capture.read(frame)){
        
    }
}