#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <iostream>
#include <opencv2/opencv.hpp>

namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};
    cv::Mat frame;


    cv::VideoCapture capture;

    capture.open(0);

    // 处理视频流
    while (capture.read(frame)) {
        // 在这里对视频帧进行处理，例如进行对象检测、人脸识别等
        cv::imshow("video", frame);

        // 加载yolov5模型
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("append")("./yolov5");
        py::module detect = py::module::import("detect");

        // 进行对象检测
        py::list results = detect.attr("detect")(frame);

        // 处理结果
        for (auto result : results) {
            std::vector<float> res = result.cast<std::vector<float>>();
            float x1 = res[0], y1 = res[1], x2 = res[2], y2 = res[3], score = res[4];
            std::cout << "x1:" << x1 << ", y1:" << y1 << ", x2:" << x2 << ", y2:" << y2 << ", score:" << score << std::endl;
        }

        // 按下Esc键退出循环
        if (cv::waitKey(1) == 27) {
            break;
        }

    }
    return 0;
}
