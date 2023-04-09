#include "detect.h"
#include "load.h"

#ifndef YOLOV5_MANAGER
#define YOLOV5_MANAGER

class yolov5_manager
{
private:
    /* data */
    int i, j;
    bool* is_space_buffer;
    InferenceEngine::InferRequest* infer_request_buffer;
    int max_buffer;


public:
    yolov5_manager();
    ~yolov5_manager();

private:
    void Product(cv::Mat src_);

    void Consume();
};


#endif