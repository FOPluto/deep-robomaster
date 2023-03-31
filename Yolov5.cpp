/**
 * @author FoPluto
*/

#include "Yolov5.h"

#define DEBUG

#define WINDOW_NAME "res_show"

Yolov5::Yolov5(){
    this->m_xml_path = "./best.xml";
    this->m_bin_path = "./best.bin";
}

// create function to define some element, change the xml some time
Yolov5::Yolov5(std::string xml_path, std::string bin_path, int input_width, int input_height){
    this->m_xml_path = xml_path;
    this->m_bin_path = bin_path;
    this->m_input_height = input_height;
    this->m_input_width = input_width;
}

void Yolov5::init_yolov5_detector(){
    printf("--------------start read network--------------\n");
    this->read_network();
    printf("---------------------done---------------------\n");
}

void Yolov5::read_network(){
    std::vector<std::string> availableDevice = this->m_ie.GetAvailableDevices();
    for(int i = 0;i < availableDevice.size();i++){
        printf("avaliable device: %s\n", availableDevice[i].c_str());
    }
	// get network from best and bin path
	auto network = m_ie.ReadNetwork(this->m_xml_path, this->m_bin_path);
	// get info from network
	this->m_input_info = InferenceEngine::InputsDataMap(network.getInputsInfo());
	this->m_output_info = InferenceEngine::OutputsDataMap(network.getOutputsInfo());
	// in
	for (auto &item : m_input_info) {
		auto input_data = item.second;
		input_data->setPrecision(Precision::FP32);
		input_data->setLayout(Layout::NCHW);
		input_data->getPreProcess().setResizeAlgorithm(RESIZE_BILINEAR);
		input_data->getPreProcess().setColorFormat(ColorFormat::RGB);
	}
	// out
	for (auto &item : m_output_info) 
	{
		auto output_data = item.second;
		output_data->setPrecision(Precision::FP32);
	}
    // load network
	this->m_executable_network = m_ie.LoadNetwork(network, "CPU");
	printf("network info: ");
	printf("i=%ld,out=%ld\n",m_input_info.size(),m_output_info.size());
    
}






bool Yolov5::is_allready()
{
	if(m_input_info.size()>0 && m_output_info.size()>0)
		return true;
	else
		return false;
}





void Yolov5::input2res(cv::Mat& src_){
	auto infer_request = m_executable_network.CreateInferRequest();
	float scale_x = (float)src_.cols / m_input_width;
	float scale_y = (float)src_.rows / m_input_height;

    if(scale_x > 1 || scale_y > 1){
        float max_scale = std::max(scale_x, scale_y);
        int res_width = src_.cols / max_scale;
        int res_height = src_.rows / max_scale;
        cv::resize(src_, src_, cv::Size(res_width, res_height));
    }
    
	scale_x = (float)src_.cols / m_input_width;
	scale_y = (float)src_.rows / m_input_height;

    if(scale_x < 1 && scale_y < 1){
        if(scale_x > scale_y){
            float max_scale = scale_x;
            cv::resize(src_, src_, cv::Size(src_.cols / max_scale, src_.rows / max_scale));
        } else {
            float max_scale = scale_y;
            cv::resize(src_, src_, cv::Size(src_.cols / max_scale, src_.rows / max_scale));
        }
    }

    if(scale_x < 1){
        cv::copyMakeBorder(src_, src_, 0, 0, 0, this->m_input_width - src_.cols, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    } else if(scale_y < 1){
        cv::copyMakeBorder(src_, src_, 0, this->m_input_height - src_.rows, 0, 0, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }

    #ifdef DEBUG
    cv::imshow("src_size", src_);
    cv::waitKey(0);
    #endif



}



void Yolov5::detect_yolov5(cv::Mat src_){
    this->m_src_image = src_;
    this->m_src_image.copyTo(m_src_copy_image);
	auto infer_request = m_executable_network.CreateInferRequest();
    this->input2res(m_src_image);
}


void Yolov5::show_res(){
    imshow(WINDOW_NAME, this->m_src_copy_image);
}