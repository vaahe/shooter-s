#ifndef CALIBRATIONRESULT_H
#define CALIBRATIONRESULT_H

#include <opencv2/core.hpp>
#include <QFile>
#include <QDebug>

class CalibrationResult {
public:
    CalibrationResult(cv::Mat cameraMatrix, cv::Mat distortionCoefficients) :
        m_cameraMatrix(cameraMatrix),
        m_distortionCoefficients(distortionCoefficients) {}

    static CalibrationResult fromCalibrationFile(const std::filesystem::path resourcePath) {
        QFile file(resourcePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "Error: Could not open the resource file for reading calibration data.";
            exit(-1);
        }

        QByteArray fileData = file.readAll();
        cv::FileStorage fs(fileData.toStdString(), cv::FileStorage::READ | cv::FileStorage::MEMORY);

        if (!fs.isOpened()) {
            qCritical() << "Error: Could not open the file storage for reading calibration data.";
            exit(-1);
        }

        cv::Mat cameraMatrix;
        cv::Mat distortionCoefficients;

        fs["CameraMatrix"] >> cameraMatrix;
        fs["DistortionCoefficients"] >> distortionCoefficients;

        if (cameraMatrix.empty() || distortionCoefficients.empty()) {
            qCritical() << "Error: Calibration data is missing or invalid.";
            exit(-1);
        }

        file.close();

        return CalibrationResult(cameraMatrix, distortionCoefficients);
    }

public:
    cv::Mat m_cameraMatrix;
    cv::Mat m_distortionCoefficients;
};

#endif // CALIBRATIONRESULT_H
