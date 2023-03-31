/**
 * @anchor 可莉不知道哦
*/

#include <torch/script>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cstring>
#include <vector>

class Yolov5{
	// 隐藏变量
	private:
	cv::Mat src_image;                         // 输入图像
	cv::Mat src_copy_image;                    // 输入图像的拷贝
	cv::Mat dst_image;                         // 调试图像
	std::vector<std::string> class_names;      // 类别名的列表
	cv::VideoCapture src_capture;              // 输入视频流
	std::vector<float> anchors;

	// 对外接口
	public:
	
	Yolov5();

	void Init();                                    // 初始化函数
	void yolov5_detect(cv::Mat src_);                 // 识别函数，用来识别装甲板

	// 内部接口
	private:
	// 图像预处理
	void inferAndOutput(cv::Mat &src_, InferenceEngine::InferRequest &infer_request, 
		InferenceEngine::InputsDataMap & input_info, InferenceEngine::OutputsDataMap &output_info, float sx, float sy);
	
	void yolov5_model_init(std::string xml_path, std::string bin_path);
	
};