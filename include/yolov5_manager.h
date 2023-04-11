#include "detect.h"
#include "load.h"

#ifndef YOLOV5_MANAGER
#define YOLOV5_MANAGER

struct request_struct
{
    InferenceEngine::InferRequest infer_request;
    unsigned int idx;
};


class Yolov5Manager
{
private:
    /* data */
    InferenceEngine::ExecutableNetwork executable_network;
    queue<InferenceEngine::InferRequest> infer_request_buffer;
    priority_queue<>
    int max_buffer;

    Yolov5Detector* detector;

    Yolov5Loader* loader;

	std::mutex mtx;
	std::condition_variable condition;

public:
    Yolov5Manager();
    ~Yolov5Manager();

private:
    void Product(cv::Mat& src_);

    void Consume();

    void InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight, int input_height);

    void get_infer_request(cv::Mat& src_);
};

#endif