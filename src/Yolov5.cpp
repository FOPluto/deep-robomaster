/**
 * @author 可莉不知道哦 
*/
#include "Yolov5.h"

// 初始化函数
void Yolov5::Init(){
    VideoCapture temp_capture(0);
    this->src_capture = temp_capture;
    this->class_names = {                                        // 初始化类别名称列表
            "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "11", "12", "13", "14", "15", "16", "17",
            "18", "19", "20", "21", "22", "23", "24", "25",
            "26", "27", "28", "29", "30", "31", "32", "33",
            "34", "35", "36"
    };
    this->anchors = {                                            // 初始化anchor
        10,13, 16,30, 33,23,
        30,61, 62,45, 59,119,
        116,90, 156,198, 373,326
    };
}

void Yolov5::detect(cv::Mat frame){
    this->src_image = frame;
    this->src_image.copyTo(this->src_copy_image);
    
}

void inferAndOutput(cv::Mat &src_, InferenceEngine::InferRequest &infer_request, 
	InferenceEngine::InputsDataMap & input_info, InferenceEngine::OutputsDataMap &output_info, float sx, float sy){
    
}