#include "frameprocessor.h"


FrameProcessor::FrameProcessor(QWidget *parent) :
    QWidget(parent),
    m_processorWorker(new FrameProcessorWorker(this)),
    m_trackingPoints(std::vector<cv::Point>()),
    m_imageFrame(cv::Mat()),
    m_lineColor(cv::Scalar(0, 255, 0))
{
    qDebug() << "Frame processor created";
    initializeImageFrame();

    connect(m_processorWorker, &FrameProcessorWorker::frameProcessed, this, &FrameProcessor::showImageFrame);
    connect(m_processorWorker, &FrameProcessorWorker::newResultAvailable, this, &FrameProcessor::newResultAvailable);
    connect(m_processorWorker, &FrameProcessorWorker::shootingPointSent, this, &FrameProcessor::initializeShootingPoints);
    connect(m_processorWorker, &FrameProcessorWorker::trajectoryPointSent, this, &FrameProcessor::initializeTrajectoryPoints);
}


FrameProcessor::~FrameProcessor() {

    qDebug() << "Frame processor destroyed";
}


void FrameProcessor::startProcessing() {
    if (!m_processorWorker->isRunning()) {
        m_processorWorker->startProcessing();
        emit processingStarted(true);
        qDebug() << "Processing started";
    }

    qDebug() << "Processing Worker running status after start: " << m_processorWorker->isRunning();
}


void FrameProcessor::stopProcessing() {
    qDebug() << "Processing Worker running status before stop: " << m_processorWorker->isRunning();

    if (m_processorWorker && m_processorWorker->isRunning()) {
        m_processorWorker->requestInterruption();
        m_processorWorker->wait();

        emit processingStopped(true);
        qDebug() << "Processing stopped";
    }

    delete m_processorWorker;
    m_processorWorker = nullptr;
}


void FrameProcessor::showImageFrame(const cv::Mat& imageFrame) {
    cv::imshow("imageFrame processor", imageFrame);
}


void FrameProcessor::initializeTrajectoryPoints(const cv::Point& trajectoryPoint) {
    m_trackingPoints.push_back(trajectoryPoint);

    drawTrajectoryPointsOnImageFrame();
}


void FrameProcessor::initializeShootingPoints(const cv::Point& shootingPoint) {
    m_shootingPoints.push_back(shootingPoint);

    drawShootingPointOnImageFrame();
    m_lineColor = cv::Scalar(0, 0, 255);
}


void FrameProcessor::drawTrajectoryPointsOnImageFrame() {
    if (m_imageFrame.empty()) {
        qWarning() << "Image frame is empty, cannot draw trajectory points.";
        return;
    }

    for (size_t i = 1; i < m_trackingPoints.size(); ++i) {
        cv::line(m_imageFrame, m_trackingPoints[i - 1], m_trackingPoints[i], m_lineColor, 1, cv::LINE_AA);
    }

    while (m_trackingPoints.size() > 2) {
        std::vector<cv::Point>::iterator it = m_trackingPoints.begin();
        m_trackingPoints.erase(it);
    }

    QMetaObject::invokeMethod(this, [this]() {
        emit drawedImageSent(m_imageFrame);
    });
}


void FrameProcessor::drawShootingPointOnImageFrame() {
    if (m_imageFrame.empty()) {
        qWarning() << "Image frame is empty, cannot draw shooting point.";
        return;
    }

    if (m_shootingPoints.size() > 0) {
        std::vector<cv::Point>::iterator it = m_shootingPoints.begin();

        cv::circle(m_imageFrame, *it, 7, cv::Scalar(255, 0, 0), -1);
        m_shootingPoints.erase(it);
    }

    QMetaObject::invokeMethod(this, [this]() {
        emit drawedImageSent(m_imageFrame);
    });
}


void FrameProcessor::initializeImageFrame() {
    QString filePath = QString(":/images/images/background_10.png");

    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error reading resource file";
    }

    QByteArray imageData = file.readAll();
    m_imageFrame = cv::imdecode(cv::Mat(1, imageData.size(), CV_8UC1, (void*)imageData.data()), cv::IMREAD_COLOR);

    if (m_imageFrame.empty()) {
        return;
    }
}
