#include "yolov5_manager.h"

Yolov5Manager::Yolov5Manager(/* args */)
{
    
}

Yolov5Manager::~Yolov5Manager(){
    
}

// 消费者线程，将消费buffer数组中的推理请求类
void Yolov5Manager::Comsume(){
    
}

//
void Yolov5Manager::get_infer_request(cv::Mat& src_){
    this->clear_work();
    // 第一步先将图片resize成为一个值
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
    // 大小
    size_t img_size = src_.cols * src_.rows;

    // 将数据打包成为一个一维数组data
    for(size_t row = 0;row < original_height;row ++){
        for(size_t col = 0;col < original_width;col ++){
            for(size_t ch = 0;ch < 3;ch ++){
                data[img_size * ch + row * original_width + col] = float(src_.at<Vec3b>(row,col)[ch]) / 255.0f;
            }
        }
    }
    // 存储request到buffer
    this.buffer[i ++] = infer_request;
    i = i >= 500 ? 0 : i; // 重置为0
}

// 生产者线程，生产出来的推理请求存到buffer数组中
void Yolov5Manager::Product(cv::Mat& src_){
    this->get_infer_request(src_);
}

// 初始化代码
void Yolov5Manager::InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight = 640, int input_height = 640){
    if(model_path[model_path.length() - 1] == '/')
        this->executable_network = this->loader->loadNetwork(model_path + "best.xml", model_path + "best.bin", device_name, input_weight, input_height);
    else
        this->executable_network = this->loader->loadNetwork(model_path + "/best.xml", model_path + "/best.bin", device_name, input_weight, input_height);
}