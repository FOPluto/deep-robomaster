/**
 * @anchor FoPluto
*/

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cstring>
#include <opencv2/dnn.hpp>
#include <inference_engine.hpp>

// yolov5_detector
class Yolov5{
	private:

	cv::Mat src_image;
	cv::Mat src_copy_image;
	double input_height;
	double input_weight;
	std::string xml_path;               // xml path
	std::string bin_path;               // bin path


	public:

	Yolov5();

	void init_yolov5_detector();        // init detector class
};