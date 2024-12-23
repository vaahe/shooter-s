#include "cameracalibratorworker.h"

CameraCalibratorWorker::CameraCalibratorWorker(QObject *parent) : QThread(parent), m_cap(nullptr), m_stop(false) {
    qDebug() << "Calibrator worker created";
}

CameraCalibratorWorker::~CameraCalibratorWorker() {
    stopCalibration();
    wait();

    qDebug() << "Calibrator worker destroyed";
}

bool CameraCalibratorWorker::initializeCamera() {
    if (m_cap == nullptr) {
        m_cap = new cv::VideoCapture("rtsp://admin:123456789m@192.168.1.64:554/Streaming/Channels/102");
        if (!m_cap->isOpened()) {
            qWarning() << "Failed to open the camera";
            return false;
        }
    }

    return true;
}


void CameraCalibratorWorker::run() {
    if (!initializeCamera()) {
        return;
    }

    while (!isInterruptionRequested()) {
        cv::Mat frame;
        (*m_cap) >> frame;

        if (frame.empty()) {
            break;
        }

        try {
            cv::flip(frame, frame, 1);
            cv::Mat undistortedFrame = undistortFrame(frame);
            cv::Mat croppedFrame = cropFrame(undistortedFrame);
            cv::Mat blackFrame = findMinMaxLoc(croppedFrame);

            countWhitePixels(croppedFrame);

            cv::Mat combinedFrame;
            cv::hconcat(blackFrame, croppedFrame, combinedFrame);

            emit frameProcessed(combinedFrame);
        } catch (const cv::Exception& e) {
            qDebug() << QString("OpenCV error: %1").arg(e.what());
            stopCalibration();
        }
    }
}

void CameraCalibratorWorker::startCalibration() {
    if (!isRunning()) {
        start();
    }
}

void CameraCalibratorWorker::stopCalibration() {
    QMutexLocker locker(&m_mutex);

    if (m_stop) {
        return;
    }

    m_stop = true;

    if (m_cap && m_cap->isOpened()) {
        m_cap->release();
    }

    delete m_cap;
    m_cap = nullptr;

    locker.unlock();
}

cv::Mat CameraCalibratorWorker::cropFrame(const cv::Mat &frame) {
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

    cv::Size cropSize(200, 200);
    int centerX = frame.cols / 2;
    int centerY = frame.rows / 2;

    int startX = std::max(centerX - cropSize.width / 2, 0);
    int startY = std::max(centerY - cropSize.height / 2, 0);

    int cropWidth = std::min(cropSize.width, grayFrame.cols - startX);
    int cropHeight = std::min(cropSize.height, grayFrame.rows - startY);

    cv::Rect croppedRegion(startX, startY, cropWidth, cropHeight);
    cv::Mat croppedFrame = grayFrame(croppedRegion);

    return croppedFrame;
}

cv::Mat CameraCalibratorWorker::resizeFrame(const cv::Mat &frame, cv::Size size) {
    cv::Mat resizedFrame;

    cv::resize(frame, resizedFrame, size);
    return resizedFrame;
}

int CameraCalibratorWorker::countWhitePixels(const cv::Mat& frame) {
    int whitePixelsCount = 0;

    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            int pixelValue = frame.at<uchar>(y, x);

            if (pixelValue > 200) {
                whitePixelsCount++;
            }
        }
    }

    // qDebug() << "white pixels count:" << whitePixelsCount;

    if (whitePixelsCount > 40) {
        qDebug() << "krakoc";
    }

    return whitePixelsCount;
}

cv::Mat CameraCalibratorWorker::findMinMaxLoc(const cv::Mat &frame) {
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;

    cv::minMaxLoc(frame, &minVal, &maxVal, &minLoc, &maxLoc);
    m_globalsManager.setCalibrationPoint(maxLoc);

    cv::Mat blackFrame(frame.rows, frame.cols, CV_8UC1, cv::Scalar(0));
    cv::circle(blackFrame, maxLoc, 5, cv::Scalar(255), -1);

    qDebug() << "Max Loc:" << "(" << maxLoc.x << "," << maxLoc.y << ")" << ", Max Value:" << maxVal;
    return blackFrame;
}

cv::Mat CameraCalibratorWorker::undistortFrame(const cv::Mat &frame) {
    CalibrationResult calibrationResult = CalibrationResult::fromCalibrationFile(":/data/data/calibration_result.xml");
    cv::Mat undistortedFrame;
    cv::undistort(frame, undistortedFrame, calibrationResult.m_cameraMatrix, calibrationResult.m_distortionCoefficients);

    return undistortedFrame;
}
