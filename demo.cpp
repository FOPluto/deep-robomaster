/**
 * @author 可莉不知道哦
 * @brief 测试文件，后期可以改成测试类
*/
#include "Yolov5.h"

int main(){

    cv::VideoCapture video_capture(0);
    cv::Mat src_image = cv::imread("/home/robomaster/deep-robomaster/demo.png");

    cv::Mat frame;

    Yolov5* demo_yolov5_detector = new Yolov5("/home/robomaster/deep-robomaster/best/model/opt-0527-001.xml", "/home/robomaster/deep-robomaster/best/model/opt-0527-001.bin", 416, 416);

    // 使用GPU进行初始化的话会出现
    demo_yolov5_detector->init_yolov5_detector();

    // 获取开始时间戳
    auto start = std::chrono::system_clock::now();
    demo_yolov5_detector->detect_yolov5(src_image);
    // 获取结束时间戳
    auto end = std::chrono::system_clock::now();
    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "所有时间开销: " << duration.count() << "ms" << std::endl;

    cv::imshow("dst", src_image);
    cv::waitKey(0);
    while(video_capture.read(frame)){
        // 获取开始时间戳
        auto start = std::chrono::system_clock::now();
        demo_yolov5_detector->detect_yolov5(frame);
        // 获取结束时间戳
        auto end = std::chrono::system_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "all Time elapsed: " << duration.count() << "ms" << std::endl;
    }
    return 0;
}