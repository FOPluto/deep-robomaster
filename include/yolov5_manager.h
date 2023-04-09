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
    InferenceEngine::InferRequest* infer_request_buffer;
    int max_buffer;

    Yolov5Detector* detector;

    Yolov5Loader loader;


public:
    Yolov5Manager();
    ~Yolov5Manager();

private:
    void Product(cv::Mat src_);

    void Consume();
};


#endif