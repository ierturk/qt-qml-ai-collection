#pragma once
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cassert>
#include "onnxruntime_cxx_api.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QImage>

#include <string>
#include <fstream>

struct NetIOs {
  cv::Mat frame;
  Ort::Value input_tensor = Ort::Value(nullptr);
  std::vector<Ort::Value> output_tensor = std::vector<Ort::Value>();
};

class OrtNet {

public:
	OrtNet();
	~OrtNet();


    void Init(const char* model_path, const char* class_path);

    void setInputTensor(QImage& image);
    void forward();
    QImage getProcessedFrame();

    void postprocess(cv::Mat& frame);

private:
	// Ort Environment
    Ort::Env env = Ort::Env(nullptr);
	Ort::Session session = Ort::Session(nullptr);
	Ort::SessionOptions session_options;
    Ort::AllocatorWithDefaultOptions allocator;
    Ort::MemoryInfo allocator_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);


    // Model
	// Inputs
	std::vector<const char*> input_node_names = std::vector<const char*>();
	std::vector<size_t> input_node_sizes = std::vector<size_t>();
	std::vector<std::vector<int64_t>> input_node_dims = std::vector<std::vector<int64_t>>();

	// Outputs
	std::vector<const char*>output_node_names = std::vector<const char*>();
	std::vector<size_t> output_node_sizes = std::vector<size_t>();
	std::vector<std::vector<int64_t>> output_node_dims = std::vector<std::vector<int64_t>>();


    // Parameters for SSD MobileNet (fixed)
    const float scale = 0.0078125f;
    const cv::Scalar mean = cv::Scalar(128.0f, 128.0f, 128.0f);
    const cv::Size sz = cv::Size(300, 300);
    const bool swapRB = true;

    float confThreshold = 0.85f;
    float nmsThreshold = 0.40f;
    std::vector<std::string> classes;

    NetIOs* netIOs = new NetIOs();

    void drawPred(cv::Mat& frame, int classId, float conf, int left, int top, int right, int bottom);
};
