#include "detect.h"

#ifndef Yolov5Loader
#define Yolov5Loader

class Yolov5Loader{
    private:

    public:
    InferenceEngine::ExecutableNetwork& loadNetwork(std::string &xml_path, std::string &bin_path, int input_weight, int input_height);

    InferenceEngine::InferRequest& getRequest(`::ExecutableNetwork& network);

    Yolov5Loader();

}

#endif