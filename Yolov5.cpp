/**
 * @author FoPluto
*/

#include "Yolov5.h"

Yolov5::Yolov5(){
    this->m_xml_path = "./best.xml";
    this->m_bin_path = "./best.bin";
}

// create function to define some element, change the xml some time
Yolov5::Yolov5(std::string xml_path, std::string bin_path = 640, int input_weight, int input_height = 640){
    this->m_xml_path = xml_path;
    this->m_bin_path = bin_path;
    this->m_input_height = input_height;
    this->m_input_weight = input_weight;
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
		return  true;
	else
		return  false;
}