#include "load.h"




Yolov5Loader::Yolov5Loader(){
}


void Yolov5Loader::clearWork(){
    this->res_rects.clear();
}


InferenceEngine::ExecutableNetwork& Yolov5Loader::loadNetwork(std::string &xml_path, std::string &bin_path, std::string &device_name = "CPU", int input_weight = 640, int input_height = 640){
    this->m_ie.SetConfig({{InferenceEngine::PluginConfigParams::KEY_DYN_BATCH_ENABLED, "YES"}}, "GPU");

    std::vector<std::string> availableDevice = this->m_ie.GetAvailableDevices();
    for(size_t i = 0;i < availableDevice.size();i++){
        printf("avaliable device: %s\n", availableDevice[i].c_str());
    }
    // 加载 IR 模型
    InferenceEngine::CNNNetwork network = m_ie.ReadNetwork(this->m_xml_path, this->m_bin_path);


	// get info from network
	this->m_input_info = InferenceEngine::InputsDataMap(network.getInputsInfo());
	this->m_output_info = InferenceEngine::OutputsDataMap(network.getOutputsInfo());
	
    //获取输入并进行设置（第一种方式）
    auto item = m_input_info.begin();
    image_info_name = item->first;         //获取image_info输入的名字
    auto image_info_ptr = item->second;    //获取image_info输入的指针
    //配置input_tensor输入：U8,NCHW, 保持默认的禁止自动放缩输入图像和禁止自动转换颜色通道
    image_info_ptr->setPrecision(InferenceEngine::Precision::FP32); 
    image_info_ptr->setLayout(InferenceEngine::Layout::NCHW);
    image_info_ptr->getPreProcess().setColorFormat(InferenceEngine::ColorFormat::BGR);
	image_info_ptr->getPreProcess().setResizeAlgorithm(RESIZE_BILINEAR);
    //获取输出并进行设置(第二种方式)
	for (auto &output : m_output_info){
	    output.second->setPrecision(Precision::FP32);
	}
    
    // 指定GPU插件名称
    std::string device_name = "CPU";
    return m_ie.LoadNetwork(network, device_name);
}





InferenceEngine::InferRequest& Yolov5Loader::getRequest(InferenceEngine::ExecutableNetwork& network){
    this->clearWork();
	scale_x = (float)src_.cols / m_input_width;
	scale_y = (float)src_.rows / m_input_height;

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
    #ifdef RED_ENERMY 
    cvtColor(src_, src_, cv::COLOR_BGR2RGB);
    #endif

    if(scale_x < 1){
        cv::copyMakeBorder(src_, src_, 0, 0, 0, this->m_input_width - src_.cols, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    } else if(scale_y < 1){
        cv::copyMakeBorder(src_, src_, 0, this->m_input_height - src_.rows, 0, 0, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }

    Mat input_blob = cv::dnn::blobFromImage(src_, 1 / 255.0, Size(640, 640), Scalar(0, 0, 0), true, false);

    // 创建推理请求
    InferenceEngine::InferRequest infer_request = m_executable_network.CreateInferRequest();

    size_t original_height = src_.rows;
    size_t original_width = src_.cols;
    /* 获得模型的image_info输入的数据缓冲 */
    Blob::Ptr image_info_blob = infer_request.GetBlob(image_info_name);
    /** 向模型的image_info输入的数据缓冲填入数据: height, width, scale=1.0 **/
    InferenceEngine::LockedMemory<void> blobMapped = InferenceEngine::as<MemoryBlob>(image_info_blob)->wmap();
    auto data = blobMapped.as<float*>();

    size_t img_size = src_.cols * src_.rows;

    for(size_t row = 0;row < original_height;row ++){
        for(size_t col = 0;col < original_width;col ++){
            for(size_t ch = 0;ch < 3;ch ++){
                data[img_size * ch + row * original_width + col] = float(src_.at<Vec3b>(row,col)[ch]) / 255.0f;
            }
        }
    }
}