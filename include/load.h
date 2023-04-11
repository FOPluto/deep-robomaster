#include "detect.h"

#ifndef Yolov5Loader
#define Yolov5Loader

class Yolov5Loader{
    private:
	cv::Mat m_src_image;
	cv::Mat m_src_copy_image;
	cv::Mat blob_image;
	
    int m_input_height;
	int m_input_width;
	int class_num;

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

	std::vector<DetectRect> res_rects;                           // res rects



    public:
    InferenceEngine::ExecutableNetwork& loadNetwork(std::string &xml_path, std::string &bin_path, int input_weight, int input_height);

    InferenceEngine::InferRequest& getRequest(InferenceEngine::ExecutableNetwork& network);

    Yolov5Loader();

    private:
    
    void clearWork();

}

#endif