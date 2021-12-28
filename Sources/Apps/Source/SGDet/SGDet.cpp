#include "SGDet.h"

QVideoFilterRunnable* SGDetFilter::createFilterRunnable() {

    ortNet = OrtNet(modelPath, isGPU, input_image_size);
    classNames = OrtUtils::loadNames(classNamesPath);
    return new SGDetFilterRunnable(this);
}

QImage SGDetFilter::detect(QImage &image) {

    mutex_ort.lock();

    cv::Mat frame  = cv::Mat(
            image.height(),
            image.width(),
            CV_8UC4,
            image.bits(),
            image.bytesPerLine())
            .clone();

    std::vector<Detection> result = ortNet.detect(frame, confThreshold, iouThreshold);
    OrtUtils::visualizeDetection(frame, result, classNames);

    QImage out =  QImage(
            frame.data,
            frame.cols,
            frame.rows,
            frame.step,
            QImage::Format_ARGB32).copy();

    mutex_ort.unlock();

    return out;
}


QVideoFrame SGDetFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) {
    Q_UNUSED(flags);

    if (!input->isValid()) {
        qWarning("Invalid input format");
        return *input;
    }

    input->map(QAbstractVideoBuffer::ReadOnly);
#if 1
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
#endif
    input->unmap();
    return *input;
}
