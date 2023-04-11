#include "detect.h"
#include "load.h"

#ifndef YOLOV5_MANAGER
#define YOLOV5_MANAGER

struct request_struct
{
    InferenceEngine::InferRequest infer_request;         // 推理请求
    unsigned int idx;                                    // 用来标记该输入的idx
};


/**
 * @author 可莉不知道哦
 * @brief detect的时候使用的。
 * @brief 其中有框的详细信息，中心点的信息，方便查找，方便进行IOU筛选
*/
struct DetectRect{
	cv::Point min_point;
	cv::Point max_point;
	cv::Rect rect;
	std::vector<cv::Point> points;
	std::vector<std::pair<float, int>> classes;
	cv::Point cen_p;
	int class_id;
	double class_p;
};


class Yolov5Manager
{
private:
    /* data */
    InferenceEngine::ExecutableNetwork executable_network;           // 可以使用的神经网络
    queue<InferenceEngine::InferRequest> infer_request_buffer;       // 推理请求队列
    queue<float *> output_data_buffer;                               // 推理完成之后的原始数据
    int max_buffer;                                                  // 最大的buffer索引值，当buffer数组过大就锁住

    Yolov5Detector* detector;                                        // 推理器相关

    Yolov5Loader* loader;                                            // 加载器相关

	std::mutex mtx;                                                  // 进程锁
	std::condition_variable condition;                               // 好像是条件对象

    size_t m_input_width;                                            // 模型的输入图像的宽
    size_t m_input_height;                                           // 模型的输入图像的高
    int class_num;                                                   // 类别的数量

    static InferenceEngine::SizeVector output_dims;                  // 模型的输出维度（每一次唯一）

	std::vector<DetectRect> res_rects;                               // 结果容器

public:
    // 析构与构造
    Yolov5Manager();
    ~Yolov5Manager();

private:
    /*          线程函数            */
    //    输入Mat       ===>     获得的推理请求
    void Product(cv::Mat& src_);
    // 获取的推理请求    ===>     得到原始结果
    void Consume();
    //  得到原始结果     ===>     最终结果
    void Comsume_res();
    /*          内部函数           */
    // 初始化Yolov5Manager类，初始化一些参数
    void InitYolov5Manager(std::string &model_path, std::string &device_name, int input_weight, int input_height);
    // 获得推理请求函数
    void get_infer_request(cv::Mat& src_);
    // 获得推理结果函数
    float* get_infer_res(InferenceEngine::InferRequest& item_infer);
    // 获得最终结果函数（该函数需要根据调用进行修改返回值）
    cv::Point get_res_ans(float* item_infer_res);
    // 清理上一次工作（暂时没有用上，未实现）
    void clear_works();
    // 作图调试（如果DEBUG关了就没用）
    void draw_res(cv::Mat &src_);
};

#endif