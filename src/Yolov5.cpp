/**
 * @author FoPluto
*/

#include "Yolov5.h"

// #define DEBUG
#define WINDOW_NAME "res_show"

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
    this->m_executable_network = m_ie.LoadNetwork(network, device_name);
}






bool Yolov5::is_allready()
{
	if(m_input_info.size()>0 && m_output_info.size()>0)
		return true;
	else
		return false;
}


void Yolov5::clear_work(){
    this->res_rects.clear();
}

void image_pre_processing(cv::Mat& src_){

}

void Yolov5::infer2res(cv::Mat& src_){
    #ifdef DEBUG
    // 获取开始时间戳
    auto start = std::chrono::system_clock::now();
    #endif // DEBUG

    this->clear_work();

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

    infer_request.Infer();

    #ifdef DEBUG

    // 获取结束时间戳
    auto end = std::chrono::system_clock::now();

    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Infer Time elapsed: " << duration.count() << "ms" << std::endl;

    #endif // DEBUG

    // 获取输出 blob
    auto output_blob = infer_request.GetBlob("output");

    // 获取 blob 数据指针
    auto output_data = output_blob->buffer().as<InferenceEngine::PrecisionTrait<InferenceEngine::Precision::FP32>::value_type*>();

    // 获取输出 blob 大小和维度信息
    // auto output_size = output_blob->size();
    auto output_dims = output_blob->getTensorDesc().getDims();

    // 获取检测到的框数量
    int num_detections = output_dims[1];

    // 处理输出结果
    // 遍历检测到的框
    int sum = 0;
    int dims = output_dims[2]; // weidu
    int num_of_classes = dims - 15; // get num of classes

    std::vector<DetectRect> rects;

    std::vector<DetectRect> final_rects;

    for (int i = 0; i < num_detections; ++i) {
        // 获取框的属性
        float confidence = output_data[i * dims + 4];
        if(confidence > 0.40) {
            DetectRect temp_rect;
            for(int j = 0;j < 5;j ++){
                cv::Point temp_p = cv::Point(output_data[5 + i * dims + j * 2], output_data[6 + i * dims + j * 2]);
                temp_rect.points.push_back(temp_p);
            }
            // classes
            for(int j = 0;j < num_of_classes;j++){
                std::pair<float, int> temp_item = {output_data[15 + j + i * dims], j};
                temp_rect.classes.push_back(temp_item);
            }
            // sort get max
            std::sort(temp_rect.classes.begin(), temp_rect.classes.end(), [](const std::pair<float, int>& a, const std::pair<float, int>& b){
                return a.first > b.first;
            });
            temp_rect.class_id = temp_rect.classes[0].second;
            temp_rect.class_p = temp_rect.classes[0].first * confidence;
            
            if(temp_rect.class_p < 0.62) continue;

            // x, y, w, h, cof, cls
            cv::Point point = cv::Point(output_data[i * dims], output_data[i * dims + 1]);
            temp_rect.cen_p = point;

            int rect_width = output_data[i *dims + 2];
            int rect_height = output_data[i * dims + 3];
            temp_rect.min_point = cv::Point(temp_rect.cen_p.x - rect_width / 2, temp_rect.cen_p.y - rect_height / 2);
            temp_rect.min_point = cv::Point(temp_rect.cen_p.x + rect_width / 2, temp_rect.cen_p.y + rect_height / 2);
            temp_rect.rect = cv::Rect(temp_rect.min_point, temp_rect.max_point);

            #ifdef DEBUG
            cout << "confidence: " << confidence << endl;
            // circle(src_, temp_rect.cen_p, 4, cv::Scalar(255, 0, 0), 4);
            sum ++;
            #endif

            // add to rects
            rects.push_back(temp_rect);
        }
        // ...
    }
    // sort
    if(rects.size()){
        std::sort(rects.begin(), rects.end(), [](const DetectRect& d1, const DetectRect& d2){
            if(d1.class_p != d2.class_p) return d1.class_p > d2.class_p;
        });
        // IOU
        res_rects.push_back(rects[0]); // push the best to the res_vector
        bool flag = true;
        for(int i = 1;i < rects.size();i++){
            for(auto item_max_rect : res_rects){
                cv::Rect max_p_rect = item_max_rect.rect; // max p rect
                cv::Rect item_p_rect = rects[i].rect; // item p rect

                int x1 = std::max(max_p_rect.x, item_p_rect.x);
                int y1 = std::max(max_p_rect.y, item_p_rect.y);
                int x2 = std::min(max_p_rect.x + max_p_rect.width, item_p_rect.x + item_p_rect.width);
                int y2 = std::min(max_p_rect.y + max_p_rect.height, item_p_rect.y + item_p_rect.height);

                int intersectionArea = std::max(0, x2 - x1) * std::max(0, y2 - y1);

                int or_space = max_p_rect.area() + item_p_rect.area() - intersectionArea;

                float IOU_with_the_max = (float)intersectionArea / or_space;

                if(IOU_with_the_max >= 0.90) {
                    flag = false;
                    break;
                }
            }
            if(flag){
                res_rects.push_back(rects[i]);
            }
        }
    }

    #ifdef DEBUG
    this->draw_res(src_);
    cout << "num: " << sum << endl;
    cv::imshow("src_image", src_);
    cv::waitKey(1);
    #endif
}

void Yolov5::draw_res(cv::Mat &src_){
    // iterate the res rect
    for(auto item_res_rect : res_rects){
        cv::Point p01 = item_res_rect.points[0]; // zuo shang
        cv::Point p02 = item_res_rect.points[1]; // zuo xia
        cv::Point p03 = item_res_rect.points[2]; // you xia
        cv::Point p04 = item_res_rect.points[3]; // you shang

        #ifdef DEBUG
        cv::circle(src_, p01, 2, cv::Scalar(0, 0, 255), 2);
        cv::circle(src_, p02, 2, cv::Scalar(0, 0, 255), 2);
        cv::circle(src_, p03, 2, cv::Scalar(0, 0, 255), 2);
        cv::circle(src_, p04, 2, cv::Scalar(0, 0, 255), 2);
        cv::line(src_, p01, p02, cv::Scalar(0, 255, 0));
        cv::line(src_, p01, p03, cv::Scalar(0, 255, 0));
        cv::line(src_, p01, p04, cv::Scalar(0, 255, 0));
        cv::line(src_, p02, p03, cv::Scalar(0, 255, 0));
        cv::line(src_, p02, p04, cv::Scalar(0, 255, 0));
        cv::line(src_, p03, p04, cv::Scalar(0, 255, 0));
        #endif        
    }
}


void Yolov5::detect_yolov5(cv::Mat src_){
    this->m_src_image = src_;
    // this->m_src_image.copyTo(m_src_copy_image);
    // this->infer2res(m_src_copy_image);
    // 优化改成
    this->infer2res(m_src_image);
}


void Yolov5::show_res(){
    imshow(WINDOW_NAME, this->m_src_copy_image);
}