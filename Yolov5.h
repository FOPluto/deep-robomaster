#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cuda_provider_factory.h>   // 提供cuda加速
#include <onnxruntime_cxx_api.h>	 // C或c++的api

// 命名空间
using namespace std;
using namespace cv;
using namespace Ort;


// 自定义配置结构
struct Configuration
{
	public: 
	float confThreshold; // Confidence threshold
	float nmsThreshold;  // Non-maximum suppression threshold
	float objThreshold;  //Object Confidence threshold
	string modelpath;
};

// 定义BoxInfo类型
typedef struct BoxInfo
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score;
	int label;
} BoxInfo;


class YOLOv5
{
public:
	YOLOv5(Configuration config);
	void detect(Mat& frame);
private:
	float confThreshold;
	float nmsThreshold;
	float objThreshold;
	int inpWidth;
	int inpHeight;
	int nout;
	int num_proposal;
	int num_classes;
	string classes[80] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus",
							"train", "truck", "boat", "traffic light", "fire hydrant",
							"stop sign", "parking meter", "bench", "bird", "cat", "dog",
							"horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
							"backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
							"skis", "snowboard", "sports ball", "kite", "baseball bat",
							"baseball glove", "skateboard", "surfboard", "tennis racket",
							"bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
							"banana", "apple", "sandwich", "orange", "broccoli", "carrot",
							"hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant",
							"bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse",
							"remote", "keyboard", "cell phone", "microwave", "oven", "toaster",
							"sink", "refrigerator", "book", "clock", "vase", "scissors",
							"teddy bear", "hair drier", "toothbrush"};
	const bool keep_ratio = true;
	vector<float> input_image_;		                                          // 输入图片
	void normalize_(Mat img);		                                          // 归一化函数
	void nms(vector<BoxInfo>& input_boxes);  
	Mat resize_image(Mat srcimg, int *newh, int *neww, int *top, int *left);  // 预处理

	Env env = Env(ORT_LOGGING_LEVEL_ERROR, "yolov5-6.1");                     // 初始化环境，
	Session *ort_session = nullptr;                                           // 初始化Session指针选项
	SessionOptions sessionOptions = SessionOptions();                         //初始化Session对象
	//SessionOptions sessionOptions;
	vector<char*> input_names;                                                // 定义一个字符指针vector
	vector<char*> output_names;                                               // 定义一个字符指针vector
	vector<vector<int64_t>> input_node_dims;                                  // >=1 outputs  ，二维vector
	vector<vector<int64_t>> output_node_dims;                                 // >=1 outputs
};