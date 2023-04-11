#include "detect.h"
#include "load.h"

#ifndef YOLOV5_MANAGER
#define YOLOV5_MANAGER

class Yolov5Manager
{
private:
    /* data */
    int i, j;
    bool* is_space_buffer;
    InferenceEngine::ExecutableNetwork executable_network;
    InferenceEngine::InferRequest* infer_request_buffer;
    int max_buffer;

    Yolov5Detector* detector;

    Yolov5Loader* loader;

public:
    Yolov5Manager();
    ~Yolov5Manager();

private:
    void Product(cv::Mat src_);

    void Consume();

    void InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight, int input_height);
};

#endif