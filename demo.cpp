#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

using namespace std;
using namespace cv;
using namespace Ort;

int main() {
    // Load ONNX model
    Env::Default().SetLogSeverityLevel(1);
    SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
    session_options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);
    Ort::Session session{GetApi(), "../yolov5.onnx", session_options};

    // Get input and output names
    vector<const char*> input_names = {"images"};
    vector<const char*> output_names = {"output"};

    // Create input tensor
    Ort::AllocatorWithDefaultOptions allocator;
    Ort::Value input_tensor{nullptr};
    vector<int64_t> input_shape = {1, 3, 640, 640};
    input_tensor = Ort::Value::CreateTensor<float>(allocator, input_shape.data(), input_shape.size());

    // Create output tensor
    Ort::Value output_tensor{nullptr};
    vector<int64_t> output_shape = {1, 25200, 85};
    output_tensor = Ort::Value::CreateTensor<float>(allocator, output_shape.data(), output_shape.size());

    // Open video file
    VideoCapture cap("../input.mp4");
    if (!cap.isOpened()) {
        cerr << "Failed to open video file!" << endl;
        return -1;
    }

    // Create window to display video
    namedWindow("YOLOv5", WINDOW_NORMAL);

    // Process video frames
    Mat frame;
    while (cap.read(frame)) {
        // Resize frame to input shape
        Mat resized_frame;
        resize(frame, resized_frame, Size(640, 640));

        // Convert frame to tensor
        float* input_data = input_tensor.GetTensorMutableData<float>();
        for (int c = 0; c < 3; c++) {
            for (int h = 0; h < 640; h++) {
                for (int w = 0; w < 640; w++) {
                    input_data[c * 640 * 640 + h * 640 + w] = resized_frame.at<Vec3b>(h, w)[c] / 255.0f;
                }
            }
        }

        // Run inference
        Ort::RunOptions run_options;
        session.Run(run_options, input_names.data(), &input_tensor, 1, output_names.data(), 1, &output_tensor);

        // Get output tensor data
        const float* output_data = output_tensor.GetTensorData<float>();

        // Draw bounding boxes on frame
        for (int i = 0; i < 25200; i++) {
            int class_id = -1;
            float confidence = 0.5;
            for (int j = 0; j < 80; j++) {
                float prob = output_data[i * 85 + j + 5];
                if (prob > confidence) {
                    class_id = j;
                    confidence = prob;
                }
            }
            if (class_id >= 0) {
                int x1 = output_data[i * 85 + 0] * frame.cols;
                int y1 = output_data[i * 85 + 1] * frame.rows;
                int x2 = output_data[i * 85 + 2] * frame.cols;
                int y2 = output_data[i * 85 + 3] * frame.rows;
                rectangle(frame, Point(x1, y1), Point(x2, y2), Scalar(0, 0, 255), 2);
                putText(frame, to_string(class_id), Point(x1, y1 - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 2);
            }
        }

        // Display frame
        imshow("YOLOv5", frame);
        waitKey(1);
    }

    // Release resources
    destroyAllWindows();
    cap.release();

    return 0;
}