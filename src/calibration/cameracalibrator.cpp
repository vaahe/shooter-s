#include "cameracalibrator.h"

CameraCalibrator::CameraCalibrator(QWidget *parent) : QWidget(parent), m_calibratorWorker(nullptr) {
    qDebug() << "Camera calibrator created";
}

CameraCalibrator::~CameraCalibrator() {

    qDebug() << "Camera calibrator destroyed";
}

void CameraCalibrator::startCalibration() {
    if (m_calibratorWorker == nullptr) {
        m_calibratorWorker = new CameraCalibratorWorker(this);
        connect(m_calibratorWorker, &CameraCalibratorWorker::frameProcessed, this, &CameraCalibrator::showFrame);
    }

    if (!m_calibratorWorker->isRunning()) {
        m_calibratorWorker->startCalibration();
        emit calibrationStarted(true);
        qDebug() << "Calibration started";
    }

    qDebug() << "Worker running status after start: " << m_calibratorWorker->isRunning();
}

void CameraCalibrator::stopCalibration() {
    qDebug() << "Worker running status before stop: " << m_calibratorWorker->isRunning();

    if (m_calibratorWorker && m_calibratorWorker->isRunning()) {
        m_calibratorWorker->requestInterruption();
        m_calibratorWorker->wait();

        emit calibrationStopped(true);
        qDebug() << "Calibration stopped";
    }

    delete m_calibratorWorker;
    m_calibratorWorker = nullptr;
}

void CameraCalibrator::showFrame(const cv::Mat& frame) {
    cv::imshow("Frame", frame);
}
