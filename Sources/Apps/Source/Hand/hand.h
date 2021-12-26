#ifndef HANDDETECTFILTER_H
#define HANDDETECTFILTER_H

#include <QAbstractVideoFilter>
#include <QDebug>
#include <QTemporaryFile>
#include "QMutex"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core/utils/filesystem.hpp>

#include "OrtNet.h"

class HandDetectFilter : public QAbstractVideoFilter {
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable();

    QImage detect(QImage &image);
    
    std::vector<cv::Scalar> getColors() {
        return colors;
    }


signals:
    void objectDetected(float x, float y, float w, float h);

public slots:

private:

    // Detection threshold
    float conf_threshold = 0.5;
    float nms_threshold = 0.5;

    // SSD MobileNet Model files 
    const std::string class_file = "/workspace/Qt/qt-qml-ai/Sources/Assets/Hands/class_names.txt";
    const std::string model_file = "/workspace/Qt/qt-qml-ai/Sources/Assets/Hands/ssd-mb-handarmv2-300.onnx";
    
    // Parameters for SSD MobileNet (fixed)
    const float scale = 0.007843137f;
    const cv::Scalar mean = cv::Scalar(127.5f, 127.5f, 127.5f);
    const cv::Size sz = cv::Size(300, 300);
    const bool swapRB = false;

    // Store the list of classe name
    std::vector<std::string> classes;
    std::vector<cv::Scalar> colors;

    // DNN model
    OrtNet* ortNet;

    // Mutex
    QMutex mutex_ort;

};

class HandDetectFilterRunnable : public QVideoFilterRunnable {
public:
    HandDetectFilterRunnable(HandDetectFilter *creator) {filter = creator;}
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

private:
    HandDetectFilter *filter;
};

#endif // HANDDETECTFILTER_H
