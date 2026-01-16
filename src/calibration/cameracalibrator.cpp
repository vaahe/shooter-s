#include "cameracalibrator.h"


CameraCalibrator::CameraCalibrator(QWidget *parent) : QWidget(parent), m_calibratorWorker(nullptr) {}


CameraCalibrator::~CameraCalibrator() {
    cv::destroyAllWindows();
}


void CameraCalibrator::startCalibration() {
    if (m_calibratorWorker == nullptr) {
        m_calibratorWorker = new CameraCalibratorWorker(this);
        connect(m_calibratorWorker, &CameraCalibratorWorker::frameProcessed, this, &CameraCalibrator::showFrame);
        connect(m_calibratorWorker, &CameraCalibratorWorker::shotTaken, this, &CameraCalibrator::stopCalibration);
    }

    if (!m_calibratorWorker->isRunning()) {
        m_calibratorWorker->startCalibration();
        emit calibrationStarted(true);
        qDebug() << "Calibration started";
    }
}


void CameraCalibrator::stopCalibration() {
    if (m_calibratorWorker && m_calibratorWorker->isRunning()) {
        m_calibratorWorker->requestInterruption();
        m_calibratorWorker->wait();

        emit calibrationStopped(true);
    }

    delete m_calibratorWorker;
    m_calibratorWorker = nullptr;
}


void CameraCalibrator::showFrame(const cv::Mat& frame) {
    cv::imshow("Frame", frame);
}
