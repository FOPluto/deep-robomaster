/**
 * @author 可莉不知道哦
 * @brief 本文件主要为yolov5多线程的调用，i为生产者线程的索引，j为消费者线程的索引
 * @bug 尚未写完代码，bug应该不少
 * 
*/
#include "yolov5_manager.h"

Yolov5Manager::Yolov5Manager()
{
    
}

Yolov5Manager::~Yolov5Manager(){
    
}




/**
 * @return 
 * @details 
 * @author 可莉不知道哦
 * @brief 
*/
InferenceEngine::InferRequest Yolov5Manager::get_infer_request(cv::Mat& src_){
    this->clear_work();
    // 第一步先将图片resize成为一个值，模型输入为[1, 3, 416, 416]的tensor张量
    float scale_x = (float)src_.cols / m_input_width;
    float scale_y = (float)src_.rows / m_input_height;
    // resize即可
    if(scale_x > 1 || scale_y > 1){
        float max_scale = std::max(scale_x, scale_y);
        int res_width = src_.cols / max_scale;
        int res_height = src_.rows / max_scale;
        cv::resize(src_, src_, cv::Size(res_width, res_height));
    }
    // 获取scale用于resize
    scale_x = (float)src_.cols / m_input_width;
    scale_y = (float)src_.rows / m_input_height;
    // 第一次resize
    if(scale_x < 1 && scale_y < 1){
        if(scale_x > scale_y){
            float max_scale = scale_x;
            cv::resize(src_, src_, cv::Size(src_.cols / max_scale, src_.rows / max_scale));
        } else {
            float max_scale = scale_y;
            cv::resize(src_, src_, cv::Size(src_.cols / max_scale, src_.rows / max_scale));
        }
    }
    // 如果红色的是敌人就把颜色空间反过来，红色变成蓝色，因为这个模型对于蓝色的颜色检测好一些
    #ifdef RED_ENERMY 
    cvtColor(src_, src_, cv::COLOR_BGR2RGB);
    #endif
    // 第二次resize
    if(scale_x < 1){
        cv::copyMakeBorder(src_, src_, 0, 0, 0, this->m_input_width - src_.cols, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    } else if(scale_y < 1){
        cv::copyMakeBorder(src_, src_, 0, this->m_input_height - src_.rows, 0, 0, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }
    // 创建推理请求
    InferenceEngine::InferRequest infer_request = m_executable_network.CreateInferRequest();
    // 处理完成的高和宽
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
    return infer_request;
}



/**
 * @name get_infer_res
 * @return InferenceEngine::SizeVector（当前推理的结果output_data）
 * @brief 传入当前推理的推理请求，会先进行推理Infer，然后进行处理，获取到一个结果数组的开始位置output_data
 * @author 可莉不知道哦
*/
InferenceEngine::SizeVector Yolov5Manager::get_infer_res(InferenceEngine::InferRequest& item_infer){
    // 推理api
    infer_request.Infer();
    // 获取输出 blob。得到的结果为[1, , , 51]的张量，这个是模型固定好了的
    auto output_blob = infer_request.GetBlob("output");
    // 获取 blob 数据指针
    float* output_data = output_blob->buffer().as<InferenceEngine::PrecisionTrait<InferenceEngine::Precision::FP32>::value_type*>();
    // 获取输出 blob 大小和维度信息（该值固定）
    // auto output_size = output_blob->size();
    this->output_dims = output_blob->getTensorDesc().getDims();
    // 返回指针
    return output_data;
}





cv::Point Yolov5Manager::get_res_ans(float* item_infer_res){
    // 获取检测到的框数量
    int num_detections = output_dims[1];
    // 处理输出结果
    // 遍历检测到的框
    int sum = 0;
    int dims = output_dims[2]; // 维度
    int num_of_classes = dims - 15; // 获得类别的数量

    std::vector<DetectRect> rects;
    std::vector<DetectRect> final_rects;
    std::vector<DetectRect> res_rects;

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
        // 根据图像之间的交并比IOU进行非极大值抑制
        res_rects.push_back(rects[0]); // 将最好的结果先推到结果中
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
                int or_area = max_p_rect.area() + item_p_rect.area() - intersectionArea;

                float IOU_with_the_max = (float)intersectionArea / or_area;

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


// 初始化代码
void Yolov5Manager::InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight = 416, int input_height = 416){
    // 加载模型
    if(model_path[model_path.length() - 1] == '/')
        this->executable_network = this->loader->loadNetwork(model_path + "best.xml", model_path + "best.bin", device_name, input_weight, input_height);
    else
        this->executable_network = this->loader->loadNetwork(model_path + "/best.xml", model_path + "/best.bin", device_name, input_weight, input_height);
    // 初始化参数，后期改为xml读取
    this->m_input_height = 416; // 模型需要的图片大小，如果是原本没有转换过的模型
    this->m_input_width = 416; 
    this->max_buffer = 1000; // 最大的buffer队列大小
}
/**
 * @name get_inference_request_from_external_source
 * @details 这里可能调用摄像头的封装类，本函数只有Product函数中调用了
 * @author 可莉不知道哦
 * @brief 摄像头相关。
*/
cv::Mat Yolov5Manager::get_inference_request_from_external_source() {
    cv::Mat src;
    if (cap.read(src)) {
        return src;
    } else {
        // 处理读取失败的情况
        return cv::Mat();
    }
}
/**
 * @name Product
 * @details 生产者线程，生产出来的推理请求存到buffer数组中，外部调用，名字得改
 * @author 可莉不知道哦
 * @brief 输入Mat       ===>     获得的推理请求
*/
void Yolov5Manager::Product(){
    while(1){
        // 从外部获取推理请求
        cv::Mat src_ = get_inference_request_from_external_source();
        // 如果buffer中东西过多，也需要锁住不处理了
        std::unique_lock<std::mutex> lock(mtx);
        condition.wait(lock, []{ return is_space_buffer.size() > 1e4; });
        // 调用多线程x`主要函数
        this->infer_request_buffer.push(this->get_infer_request(src_));
    }
}
/**
 * @name Consume
 * @return void（只是将结果存到队列中）
 * @details 消费者线程，将消费buffer数组中的推理请求类，将推理请求的类push到类中的infer_request_buffer中
 * @author 可莉不知道哦
 * @brief 获取的推理请求    ===>     得到原始结果
*/
void Yolov5Manager::Consume(){
    while(1){
        // 如果queue中没东西，不能进行下一步
        std::unique_lock<std::mutex> lock(mtx);
        condition.wait(lock, []{ return !infer_request_buffer.empty(); });
        // 再次确保一下size
        if(infer_request_buffer.size()){
            // 将infer取出来推理
            auto item = infer_request_buffer.front();
            infer_request_buffer.pop();
            // 推理到结果，并且存到队列中
            // 如果这里过大，需不需要锁住？
            this->output_data_buffer.push(this->get_infer_res());
        }
    }
}
/**
 * @name Consume_res
 * @details 第二级消费者线程
 * @author 可莉不知道哦
 * @brief 得到原始结果     ===>     最终结果
*/
void Yolov5Manager::Consume_res(){
    while(1){
        // 如果queue中没东西，不能进行下一步
        std::unique_lock<std::mutex> lock(mtx);
        condition.wait(lock, []{ return !output_data_buffer.empty(); });
        // 再次确保一下size
        if(output_data_buffer.size()){
            // 将data取出来进行后续处理
            InferenceEngine::SizeVector item = output_data_buffer.front();
            output_data_buffer.pop();
            // 如果这里过大，需不需要锁住？
            output_point.push(this->get_res_ans(item));
        }
    }
}
/**
 * @name draw_res
 * @details 画图函数
 * @author 可莉不知道哦
 * @brief 画图相关，如果将#define DEBUG解开，就会进入DEBUG模式，自动将代码添加进来实现画图
*/
void Yolov5Manager::draw_res(cv::Mat &src_){
    // iterate the res rect
    for(auto item_res_rect : res_rects){
        cv::Point p01 = item_res_rect.points[0]; // zuo shang
        cv::Point p02 = item_res_rect.points[1]; // zuo xia
        cv::Point p03 = item_res_rect.points[2]; // you xia
        cv::Point p04 = item_res_rect.points[3]; // you shang

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
    }
}
