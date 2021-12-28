#ifndef SGDET_H
#define SGDET_H

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

class SGDetFilter : public QAbstractVideoFilter {
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable();

    QImage detect(QImage &image);

signals:
    void objectDetected(float x, float y, float w, float h);

public slots:

private:

    // Detection threshold
    float confThreshold = 0.4;
    float iouThreshold = 0.45;

    // Model Info 
    bool isGPU = false;
    const std::string classNamesPath = "/workspace/Qt/qt-qml-ai/Sources/Assets/YoloV5/coco.names";
    const std::string modelPath = "/workspace/Qt/qt-qml-ai/Sources/Assets/YoloV5/yolov5n.onnx";  
    const cv::Size input_image_size = cv::Size(640, 640);

    // Store the list of classe name
    std::vector<std::string> classNames;

    // DNN model
    OrtNet ortNet {nullptr};

    // Mutex
    QMutex mutex_ort;

};

class SGDetFilterRunnable : public QVideoFilterRunnable {
public:
    SGDetFilterRunnable(SGDetFilter *creator) {filter = creator;}
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

private:
    SGDetFilter *filter;
};

#endif // SGDET_H
