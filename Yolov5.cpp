/**
 * @author FoPluto
*/

#include "Yolov5.h"

#define DEBUG
#define WINDOW_NAME "res_show"
#define endl "\n"

std::vector<float> anchors = {
	10,13, 16,30, 33,23,
	30,61, 62,45, 59,119,
	116,90, 156,198, 373,326
};

int get_anchor_index(int scale_w, int scale_h) {
	if (scale_w == 20) {
		return 12;
	}
	if (scale_w == 40) {
		return 6;
	}
	if (scale_w == 80) {
		return 0;
	}
	return -1;
}

float get_stride(int scale_w, int scale_h) {
	if (scale_w == 20) {
		return 32.0;
	}
	if (scale_w == 40) {
		return 16.0;
	}
	if (scale_w == 80) {
		return 8.0;
	}
	return -1;
}

float sigmoid_function(float a)
{
	float b = 1. / (1. + exp(-a));
	return b;
}








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
    this->threshold = 0.6;
    this->class_num = 36;
    printf("---------------------done---------------------\n");
}




void Yolov5::read_network(){
    this->m_ie.SetConfig({{CONFIG_KEY(CPU_THREADS_NUM), "8"}}, "CPU");
    std::vector<std::string> availableDevice = this->m_ie.GetAvailableDevices();
    for(int i = 0;i < availableDevice.size();i++){
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
    
    this->m_executable_network = m_ie.LoadNetwork(network, "CPU");
}






bool Yolov5::is_allready()
{
	if(m_input_info.size()>0 && m_output_info.size()>0)
		return true;
	else
		return false;
}





void Yolov5::input2res(cv::Mat& src_){
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

    // cvtColor(src_, src_, cv::COLOR_BGR2RGB);

    if(scale_x < 1){
        cv::copyMakeBorder(src_, src_, 0, 0, 0, this->m_input_width - src_.cols, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    } else if(scale_y < 1){
        cv::copyMakeBorder(src_, src_, 0, this->m_input_height - src_.rows, 0, 0, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }

    Mat input_blob = cv::dnn::blobFromImage(src_, 1 / 255.0, Size(640, 640), Scalar(0, 0, 0), true, false);

    // 创建推理请求
    InferenceEngine::InferRequest infer_request = m_executable_network.CreateInferRequest();

    auto original_height = src_.rows;
    auto original_width = src_.cols;
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


    infer_request.Infer();


    // 获取输出 blob
    auto output_blob = infer_request.GetBlob("output");

    // 获取 blob 数据指针
    auto output_data = output_blob->buffer().as<InferenceEngine::PrecisionTrait<InferenceEngine::Precision::FP32>::value_type*>();

    // 获取输出 blob 大小和维度信息
    auto output_size = output_blob->size();
    auto output_dims = output_blob->getTensorDesc().getDims();

    // 获取检测到的框数量
    int num_detections = output_dims[1];

    // 处理输出结果
    // 遍历检测到的框
    int sum = 0;
    // std::vector<struct> 
    vector<int> confidence;
    vector<int> classes;

    for (int i = 0; i < num_detections; ++i) {
        // 获取框的属性
        float confidence = output_data[i * output_dims[2] + 4];
        if(confidence > 0.45) {
            
            float res[51];
            for(int j = 0;j < output_dims[2];j++){
                res[j] = output_data[j + i * output_dims[2]];
                // cout << output_data[j + i * 51] << " ";
            }
            #ifdef DEBUG
            cout << "confidence: " << confidence << endl;
            sum ++;
            #endif
        }
        // ...
    }

    #ifdef DEBUG
    cout << "num: " << sum << endl;
    draw_res();
    cv::imshow("src_image", src_);
    cv::waitKey(1);
    #endif
}

void Yolov5::draw_res(){
}


void Yolov5::detect_yolov5(cv::Mat src_){
    this->m_src_image = src_;
    this->m_src_image.copyTo(m_src_copy_image);
    this->input2res(m_src_copy_image);
}


void Yolov5::show_res(){
    imshow(WINDOW_NAME, this->m_src_copy_image);
}