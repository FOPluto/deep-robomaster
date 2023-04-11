/**
 * @author 可莉不知道哦
*/

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cstring>
#include <opencv2/dnn.hpp>
#include <inference_engine.hpp>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <map>
#include <queue>
#include <condition_variable>
#include <thread>

#ifndef YOLOV5_DETECTOR
#define YOLOV5_DETECTOR

class Yolov5Detector(){
    private:
	cv::Mat m_src_image;
	cv::Mat m_src_copy_image;
	cv::Mat blob_image;
	int m_input_height;
	int m_input_width;
	std::string image_info_name;
	std::string m_xml_path;                                      // xml path
	std::string m_bin_path;                                      // bin path
	
	InferenceEngine::Core m_ie;
	InferenceEngine::InputsDataMap m_input_info;                 // input information
	InferenceEngine::OutputsDataMap m_output_info;               // output information
	InferenceEngine::ExecutableNetwork m_executable_network;     // trained model

	float scale_x;
	float scale_y;

	float threshold;

	int class_num;

    public:

    Yolov5Detector();
}

#endif