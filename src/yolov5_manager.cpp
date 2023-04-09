#include "yolov5_manager.h"

Yolov5Manager::Yolov5Manager(/* args */)
{
    
}

Yolov5Manager::~Yolov5Manager(){
    
}

void Yolov5Manager::Comsume(){
    
}


void Yolov5Manager::Product(cv::Mat& src_){
    
}

void Yolov5Manager::InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight = 640, int input_height = 640){
    if(model_path[model_path.length() - 1] == '/')
        this->executable_network = this->loader->loadNetwork(model_path + "best.xml", model_path + "best.bin", device_name, input_weight, input_height);
    else
        this->executable_network = this->loader->loadNetwork(model_path + "/best.xml", model_path + "/best.bin", device_name, input_weight, input_height);
}