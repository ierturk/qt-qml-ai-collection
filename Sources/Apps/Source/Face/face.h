#ifndef QCVDETECTFILTER_H
#define QCVDETECTFILTER_H

#include <QAbstractVideoFilter>
#include <QDebug>
#include <QTemporaryFile>
#include <QMutex>
#include "opencv2/opencv.hpp"

class QCvDetectFilter : public QAbstractVideoFilter
{
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable();

signals:
    void objectDetected(float x, float y, float w, float h);

public slots:

};

class QCvDetectFilterRunnable : public QVideoFilterRunnable
{
public:
    QCvDetectFilterRunnable(QCvDetectFilter *creator) {filter = creator;}
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

private:
    QCvDetectFilter *filter;
};

#endif // QCVDETECTFILTER_H
