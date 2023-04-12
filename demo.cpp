/**
 * @author 可莉不知道哦
 * @brief 测试文件，后期可以改成测试类
*/
#include "Yolov5.h"

int main(){

    cv::VideoCapture video_capture("/home/robomaster/deep-robomaster/test.avi");

    cv::Mat frame;

    Yolov5* demo_yolov5_detector = new Yolov5("/home/robomaster/deep-robomaster/best/model/opt-0527-001.xml", "/home/robomaster/deep-robomaster/model/opt-0527-001.bin", 416, 416);

    // init
    demo_yolov5_detector->init_yolov5_detector();
    
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