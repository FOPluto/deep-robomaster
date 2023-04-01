/**
 * @anchor FoPluto
*/

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cstring>
#include <opencv2/dnn.hpp>
#include <inference_engine.hpp>

// #define DEBUG

using namespace std;
using namespace cv;
using namespace cv::dnn;
using namespace InferenceEngine;

// yolov5_detector
class Yolov5{
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
	
	InferenceEngine::InferRequest infer_request;

	float scale_x;
	float scale_y;

	float threshold;

	int class_num;

	vector<Rect> boxes;
	vector<int> classIds;
	vector<float> confidences;

	public:

	Yolov5();                           // 
	Yolov5(std::string xml_path, std::string bin_path, int input_weight, int input_height);

	void init_yolov5_detector();        // init detector class

	void detect_yolov5(cv::Mat src_);   // detect function

	void show_res();

	private:

	void input2res(cv::Mat& src_);   // Mat yuchuli

	void read_network();                // read network to class

	bool is_allready();                 // judge if is allready

	void draw_res();
};