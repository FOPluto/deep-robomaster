#include <opencv2/opencv.hpp>
#include <torch/script.h>
#include <iostream>
#include <memory>

int main() {
    // 加载模型
    auto module = torch::jit::load("model.pt");

    // 加载图片
    cv::Mat image = cv::imread("test.jpg");

    cv::VideoCapture videoCapture("6000-2.mp4");

    if(!videoCapture.isOpened()){
        std::cout << "video is not opened" << std::endl;
        return -1;
    }

    cv::Mat frame;

    while(videoCapture.read(frame)){
        cv::Mat src_image;
        frame.copyTo(src_image);

        cv::imshow("src_image", frame);

        // 转换为输入的Tensor类型的数据,并且调整顺序
        torch::Tensor input_tensor = torch::from_blob(src_image.data, {1, src_image.rows, src_image.cols, src_image.channels()}, torch::kByte);
        input_tensor = input_tensor.permute({0, 3, 1, 2});
        input_tensor = input_tensor.to(torch::kFloat);
        input_tensor = input_tensor.div(255);
        // 使用模型进行推理
        auto output_tensor = module.forward({input_tensor});
        // 并将结果转换成为tensor格式
        std::cout << output_tensor << std::endl;
        auto output = output_tensor.toTensorList();
        std::vector<at::Tensor> output_tensors(output.begin(), output.end());
        std::cout << "num_of_output_level: " << output_tensors.size() << std::endl;
        cvWaitKey(1);
    }

    return 0;
}