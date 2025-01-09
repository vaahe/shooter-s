#ifndef FRAMEPROCESSORWORKER_H
#define FRAMEPROCESSORWORKER_H

#include "src/utilities/soundplayer.h"
#include "src/utilities/jsonmanager.h"
#include "src/utilities/globalsmanager.h"
#include "src/database/databasemanager.h"
#include "src/utilities/calibrationresult.h"

#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QMessageBox>
#include <QJsonDocument>
#include <QFile>
#include <QMutexLocker>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include <tuple>


class FrameProcessorWorker : public QThread
{
    Q_OBJECT

public:
    FrameProcessorWorker(QObject *parent = nullptr);
    ~FrameProcessorWorker();

public slots:
    void startProcessing();
    void stopProcessing();

    void processFrames();
    void processResult(const cv::Point shootingPoint);
    QString processPoint(const cv::Point shootingPoint);

    void initializeJsonFile();
    bool initializeCamera();

    QString getPoint(int x, int y);

    int countWhitePixels(const cv::Mat& frame);
    cv::Point findMaxLoc(const cv::Mat& frame);
    int tresholdFrame(const cv::Mat& frame);
    cv::Mat undistortFrame(const cv::Mat& frame);
    std::pair<cv::Mat, cv::Mat> cropAndResizeFrameByCenter(const cv::Mat& frame, cv::Point centerPoint);
    cv::Mat cropFrame(const cv::Mat &frame, cv::Point cropPoint);

signals:
    void processingStarted();
    void processingFinished();
    void newResultAvailable(const Database::Result& result);
    void frameProcessed(const cv::Mat& frame);
    void trajectoryPointSent(const cv::Point& trajectoryPoint);
    void shootingPointSent(const cv::Point& shootingPoint);

protected:
    void run() override;

private:
    QMutex m_mutex;
    cv::VideoCapture *m_cap;
    bool m_stop;

    JsonManager *m_jsonManager;
    DatabaseManager *m_db = DatabaseManager::getInstance();
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
};

#endif // FRAMEPROCESSORWORKER_H
