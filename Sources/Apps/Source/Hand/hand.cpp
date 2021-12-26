#include "hand.h"

QVideoFilterRunnable* HandDetectFilter::createFilterRunnable() {

    ortNet = new OrtNet();
    ortNet->Init("/workspace/Qt/qt-qml-ai/Sources/Assets/Hands/ssd-mb-handarmv2-300.onnx",
                    "/workspace/Qt/qt-qml-ai/Sources/Assets/Hands/class_names.txt"
    );

    return new HandDetectFilterRunnable(this);
}

QImage HandDetectFilter::detect(QImage &image) {

    mutex_ort.lock();

    ortNet->setInputTensor(image);
    ortNet->forward();

    mutex_ort.unlock();

    return ortNet->getProcessedFrame();
}


QVideoFrame HandDetectFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) {
    Q_UNUSED(flags);

    if (!input->isValid()) {
        qWarning("Invalid input format");
        return *input;
    }

    input->map(QAbstractVideoBuffer::ReadOnly);

    if(surfaceFormat.handleType() == QAbstractVideoBuffer::NoHandle) {
        QImage image = input->image();
        // cv::flip(frame, frame, 0);

        auto start = std::chrono::high_resolution_clock::now();

        // Input the image to the model and get the result
        QImage retImage(filter->detect(image));
        if (retImage.format() != QImage::Format_ARGB32) {
            retImage = retImage.convertToFormat(QImage::Format_ARGB32);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // qDebug() <<  "Total detection time : " << duration.count() << " ms";

        
        input->unmap();
        return QVideoFrame(retImage);
    }    

    input->unmap();
    return *input;
}
