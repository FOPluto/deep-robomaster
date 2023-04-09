#include "load.h"

Yolov5Loader::Yolov5Loader(){
    
}


InferenceEngine::InferRequest& Yolov5Loader::getRequest(InferenceEngine::ExecutableNetwork& network)