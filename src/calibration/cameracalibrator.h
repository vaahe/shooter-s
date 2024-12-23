#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H

#include "src/calibration/cameracalibratorworker.h"

#include <QWidget>

class CameraCalibrator : public QWidget
{
    Q_OBJECT

public:
    CameraCalibrator(QWidget *parent = nullptr);
    ~CameraCalibrator();

public slots:
    void startCalibration();
    void stopCalibration();

    void showFrame(const cv::Mat& frame);

signals:
    void calibrationStarted(bool);
    void calibrationStopped(bool);

private:
    CameraCalibratorWorker *m_calibratorWorker;
};

#endif // CAMERACALIBRATOR_H
