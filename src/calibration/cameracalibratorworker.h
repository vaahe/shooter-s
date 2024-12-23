#ifndef CAMERACALIBRATORWORKER_H
#define CAMERACALIBRATORWORKER_H

#include "src/utilities/globalsmanager.h"
#include "src/utilities/calibrationresult.h"

#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QMutexLocker>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

class CameraCalibratorWorker : public QThread
{
    Q_OBJECT

public:
    CameraCalibratorWorker(QObject *parent = nullptr);
    ~CameraCalibratorWorker();

public slots:
    void startCalibration();
    void stopCalibration();

    int countWhitePixels(const cv::Mat& frame);
    cv::Mat cropFrame(const cv::Mat& frame);
    cv::Mat findMinMaxLoc(const cv::Mat& frame);
    cv::Mat resizeFrame(const cv::Mat& frame, cv::Size size);
    cv::Mat undistortFrame(const cv::Mat& frame);

    bool initializeCamera();

signals:
    void calibrationStarted();
    void calibrationFinished();
    void frameProcessed(const cv::Mat& frame);

protected:
    void run() override;

private:
    QMutex m_mutex;
    cv::VideoCapture *m_cap;
    bool m_stop;
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
};

#endif // CAMERACALIBRATORWORKER_H
