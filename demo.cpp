// #include <pybind11/embed.h>
// #include <pybind11/stl.h>
// #include <iostream>
// #include <opencv2/opencv.hpp>

// namespace py = pybind11;

// int main() {
//     py::scoped_interpreter guard{};
//     cv::Mat frame;


//     cv::VideoCapture capture;

//     capture.open("/home/fo_pluto/demo_opencv/6000-2.mp4");

//     // 处理视频流
//     while (capture.read(frame)) {
//         // 在这里对视频帧进行处理，例如进行对象检测、人脸识别等
//         cv::imshow("video", frame);

//         // 加载yolov5模型
//         py::module sys = py::module::import("sys");
//         sys.attr("path").attr("append")("./yolov5");
//         py::module detect = py::module::import("detect");

//         // 进行对象检测
//         py::list results = detect.attr("detect")(frame);

//         // 处理结果
//         for (auto result : results) {
//             std::vector<float> res = result.cast<std::vector<float>>();
//             float x1 = res[0], y1 = res[1], x2 = res[2], y2 = res[3], score = res[4];
//             std::cout << "x1:" << x1 << ", y1:" << y1 << ", x2:" << x2 << ", y2:" << y2 << ", score:" << score << std::endl;
//         }

//         // 按下Esc键退出循环
//         if (cv::waitKey(1) == 27) {
//             break;
//         }

//     }
//     return 0;
// }

#include <opencv2/opencv.hpp>
#include <torch/script.h>
#include <iostream>
#include <memory>

int main() {
    // 加载模型
    auto module = torch::jit::load("model.pt");

    // 加载图片
    cv::Mat image = cv::imread("test.jpg");

    
    // 将图片转换为 Tensor，并且调整大小为 640x640
    torch::Tensor input_tensor = torch::from_blob(image.data, {1, image.rows, image.cols, image.channels()}, torch::kByte);
    input_tensor = input_tensor.permute({0, 3, 1, 2}); 
    input_tensor = torch::upsample_nearest2d(input_tensor, {640, 640});
    input_tensor = input_tensor.to(torch::kFloat);
    input_tensor = input_tensor.div(255);

    // 使用模型进行推理
    at::Tensor output_tensor = module.forward({input_tensor}).toTensor();

    // 处理输出结果
    // ...

    return 0;
}