#include "OrtNet.h"
#include <QtCore/QFile>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QDebug>

OrtNet::OrtNet() {}
OrtNet::~OrtNet() {}

void OrtNet::Init(const char* model_path, const char* class_path) {

    env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "OrtEnv");
    session_options.SetIntraOpNumThreads(1);

    // Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0));
    // Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_Tensorrt(session_options, 0));

    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

    session = Ort::Session(env, model_path, session_options);

    Ort::IoBinding io_binding{session};

	// print number of model input nodes
    int num_input_nodes = session.GetInputCount();
	input_node_names = std::vector<const char*>(num_input_nodes);
    qDebug() << "Number of inputs = " << num_input_nodes;

	// iterate over all input nodes
    for (int i = 0; i < num_input_nodes; i++) {
		// print input node names
		char* input_name = session.GetInputName(i, allocator);
        qDebug() << "Input " << i << " name " << input_name;
		input_node_names[i] = input_name;

		// print input node types
		Ort::TypeInfo type_info = session.GetInputTypeInfo(i);
		auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
		input_node_sizes.push_back(tensor_info.GetElementCount());

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        qDebug() << "Input " << i << " type " << type;

		// print input shapes/dims
		input_node_dims.push_back(tensor_info.GetShape());
        qDebug() << "Input " << i << " size " << input_node_dims[i].size();
        // for (size_t j = 0; j < input_node_dims[i].size(); j++)
        //	printf("Input %d : dim %d=%jd\n", i, j, input_node_dims[i][j]);
	}

	// print number of model output nodes
    int num_output_nodes = session.GetOutputCount();
	output_node_names = std::vector<const char*>(num_output_nodes);
    qDebug() << "Number of outputs = " << num_output_nodes;

	// iterate over all input nodes
	for (int i = 0; i < num_output_nodes; i++) {
		// print output node names
		char* output_name = session.GetOutputName(i, allocator);
        qDebug() << "Output " << i << " name " << output_name;
		output_node_names[i] = output_name;

		// print output node types
		Ort::TypeInfo type_info = session.GetOutputTypeInfo(i);
		auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
		output_node_sizes.push_back(tensor_info.GetElementCount());

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        qDebug() << "Output " << i << " type " <<  type;

		// print output shapes/dims
		output_node_dims.push_back(tensor_info.GetShape());
        qDebug() << "Output " << i << " size " << output_node_dims[i].size();
        // for (size_t j = 0; j < output_node_dims[i].size(); j++)
        //	printf("Output %d : dim %d=%jd\n", i, j, output_node_dims[i][j]);
	}

    // Open and read class file
    std::ifstream ifs(class_path);
    if(!ifs.is_open()) {
        qDebug() << "Class File not found.";
    }

    std::string line;
    while(std::getline(ifs, line)) {
        classes.push_back(line);
    }
}


void OrtNet::setInputTensor(QImage& image)
{
    cv::Mat cv_blob;

    netIOs->frame =
            cv::Mat(
            image.height(),
            image.width(),
            CV_8UC4,
            image.bits(),
            image.bytesPerLine())
            .clone();

    // cv::flip(netIOs->frame, netIOs->frame, 1);

    cv_blob = cv::dnn::blobFromImage(
            netIOs->frame,
            scale,
            sz,
            mean,
            swapRB,
            false,
            CV_32F);

    netIOs->input_tensor = Ort::Value::CreateTensor<float>(
        allocator_info,
        cv_blob.ptr<float>(),
        input_node_sizes[0],
        input_node_dims[0].data(),
        input_node_dims[0].size());
    assert(netIOs->input_tensor.IsTensor());
}

void OrtNet::forward()
{
    auto start = std::chrono::high_resolution_clock::now();

    netIOs->output_tensor = session.Run(
        Ort::RunOptions{ nullptr },
        input_node_names.data(),
        &(netIOs->input_tensor),
        input_node_names.size(),
        output_node_names.data(),
        output_node_names.size());

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // qDebug() <<  "Inference time : " << duration.count() << " ms";

    // scores = output_tensor[0].GetTensorMutableData<float>();
    // boxes = output_tensor[1].GetTensorMutableData<float>();
    // outs = std::make_pair(scores, boxes);
}

QImage OrtNet::getProcessedFrame() {

    static cv::Mat frame;

    frame = netIOs->frame;

    // cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    postprocess(frame);

    return QImage(
            frame.data,
            frame.cols,
            frame.rows,
            frame.step,
            QImage::Format_ARGB32).copy();
}

void OrtNet::postprocess(cv::Mat& frame)
{
    std::vector<int> p_classIds;
    std::vector<float> p_confidences;
    std::vector<cv::Rect> p_boxes;
    float* scores;
    float* boxes;

    scores = netIOs->output_tensor[0].GetTensorMutableData<float>();
    boxes = netIOs->output_tensor[1].GetTensorMutableData<float>();


    // CV_Assert(scores[0] > 0);

    for (size_t i = 0; i < 3000; i++) {
            // qDebug() << " confidence "
            //     << " - 0 " << scores[4 * i + 0]
            //     << " - 1 " << scores[4 * i + 1]
            //     << " - 2 " << scores[4 * i + 2]
            //     << " - 3 " << scores[4 * i + 3];
        for (size_t j = 1; j < 4; j++) {
            float confidence = scores[4 * i + j];                
            if (confidence > confThreshold)
            {
                int left = (int)(boxes[4 * i] * frame.cols);
                int top = (int)(boxes[4 * i + 1] * frame.rows);
                int right = (int)(boxes[4 * i + 2] * frame.cols);
                int bottom = (int)(boxes[4 * i + 3] * frame.rows);
                int width = right - left + 1;
                int height = bottom - top + 1;

                p_classIds.emplace_back((int)j);;
                p_confidences.emplace_back((float)confidence);
                p_boxes.emplace_back(cv::Rect(left, top, width, height));
            }
        }
    }

    std::vector<int> p_indices;
    cv::dnn::NMSBoxes(p_boxes, p_confidences, confThreshold, nmsThreshold, p_indices);
    for (size_t i = 0; i < p_indices.size(); ++i)
    {
        int idx = p_indices[i];
        cv::Rect box = p_boxes[idx];
        drawPred(frame, p_classIds[idx], p_confidences[idx], box.x, box.y,
            box.x + box.width, box.y + box.height);
    }
}

void OrtNet::drawPred(cv::Mat& frame, int classId, float conf, int left, int top, int right, int bottom)
{
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

    std::string label = cv::format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ": " + label;
    }

    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 1.2, 1.2, &baseLine);
    top = cv::max(top, labelSize.height);

    cv::rectangle(frame,
                  cv::Point(left, top - labelSize.height),
                  cv::Point(left + labelSize.width, top + baseLine),
                  cv::Scalar::all(255),
                  cv::FILLED);

    cv::putText(frame,
                label,
                cv::Point(left, top),
                cv::FONT_HERSHEY_SIMPLEX,
                1.0,
                cv::Scalar());
}
