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
        // 获取开始时间戳
        auto start = std::chrono::system_clock::now();

        demo_yolov5_detector->detect_yolov5(frame);
        demo_yolov5_detector->show_res();

        // 获取结束时间戳
        auto end = std::chrono::system_clock::now();

        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "all Time elapsed: " << duration.count() << "ms" << std::endl;
    }
    return 0;
}