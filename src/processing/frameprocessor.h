#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "src/database/databasemanager.h"
#include "src/processing/frameprocessorworker.h"
#include <QWidget>


class FrameProcessor : public QWidget
{
    Q_OBJECT

public:
    FrameProcessor(QWidget *parent = nullptr);
    ~FrameProcessor();

public slots:
    void startProcessing();
    void stopProcessing();

    void initializeImageFrame();
    void showImageFrame(const cv::Mat& frame);
    void drawTrajectoryPointsOnImageFrame();
    void drawShootingPointOnImageFrame();

    void initializeTrajectoryPoints(const cv::Point& trajectoryPoint);
    void initializeShootingPoints(const cv::Point& shootingPoint);

signals:
    void processingStarted(bool);
    void processingStopped(bool);
    void newResultAvailable(const Database::Result& result);
    void drawedImageSent(const cv::Mat& frame);

private:
    FrameProcessorWorker *m_processorWorker;
    std::vector<cv::Point> m_trackingPoints;
    std::vector<cv::Point> m_shootingPoints;
    cv::Mat m_imageFrame;
    cv::Scalar m_lineColor;
};

#endif // FRAMEPROCESSOR_H
