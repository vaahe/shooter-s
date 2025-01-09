#include "frameprocessorworker.h"


FrameProcessorWorker::FrameProcessorWorker(QObject *parent) :
    QThread(parent),
    m_cap(nullptr),
    m_stop(false),
    m_jsonManager(nullptr) {
    initializeJsonFile();
}


FrameProcessorWorker::~FrameProcessorWorker() {
    stopProcessing();
    wait();

    qDebug() << "Processor worker destroyed";
}


bool FrameProcessorWorker::initializeCamera() {
    if (m_cap == nullptr) {
        m_cap = new cv::VideoCapture("rtsp://admin:123456789m@192.168.1.64:554/Streaming/Channels/102");
        if (!m_cap->isOpened()) {
            qWarning() << "Failed to open the camera";
            return false;
        }
    }

    return true;
}


void FrameProcessorWorker::processFrames() {
    cv::Mat frame;
    (*m_cap) >> frame;
    cv::flip(frame, frame, 1);

    if (frame.empty()) {
        qWarning() << "Captured an empty frame.";
        return;
    }

    cv::Point calibrationPoint = m_globalsManager.getCalibrationPoint();
    int x = calibrationPoint.x + 220;
    int y = calibrationPoint.y + 140;

    if (!x || !y) {
        QMessageBox::warning(nullptr, "Calibration error", "Calibration point is not correct, try again!");
    }

    if (x <= 0 || y <= 0 || x > frame.cols || y > frame.rows) {
        QMessageBox::warning(nullptr, "Calibration error", "Calibration point is not correct, try again!");
    }

    cv::Mat undistortedFrame = undistortFrame(frame);
    auto [croppedFrame, resizedFrame] = cropAndResizeFrameByCenter(undistortedFrame, cv::Point(x, y));

    cv::Point maxLoc = findMaxLoc(resizedFrame);

    cv::Mat originalFrame = cropFrame(undistortedFrame, cv::Point(x, y));
    int whitePixelsCount = countWhitePixels(originalFrame);

    if (whitePixelsCount > 40) {
        SoundPlayer *soundPlayer = new SoundPlayer();
        soundPlayer->playSound("qrc:/sounds/sounds/gunshot.wav");
        processResult(maxLoc);

        emit shootingPointSent(maxLoc);
        m_cap->read(frame);

        delete soundPlayer;
        soundPlayer = nullptr;
    }

    emit frameProcessed(resizedFrame);

    // for (int i = 0; i < 3; ++i) {
    // SoundPlayer *m_player;
    // m_player->playSound("qrc:/sounds/sounds/gunshot.wav");

    //     processResult();
    //     QThread::msleep(2000);
    // }
}


cv::Point FrameProcessorWorker::findMaxLoc(const cv::Mat &frame) {
    cv::Mat grayFrame;
    if (frame.channels() > 1) {
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    } else {
        grayFrame = frame;
    }

    if (grayFrame.empty()) {
        qWarning() << "Gray frame is empty.";
        return cv::Point(0, 0);
    }

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(grayFrame, &minVal, &maxVal, &minLoc, &maxLoc);

    qDebug() << "Max Loc:" << "(" << maxLoc.x << "," << maxLoc.y << ")" << ", Max Value:" << maxVal;
    emit trajectoryPointSent(maxLoc);

    return maxLoc;
}


void FrameProcessorWorker::processResult(const cv::Point shootingPoint) {
    std::pair<int, int> trainingParams = m_globalsManager.getTrainingParams();
    const auto [distance, imitationDistance] = trainingParams;

    Result resultData;
    resultData.userId = "1";
    resultData.distance = distance;
    resultData.result = getPoint(shootingPoint.x, shootingPoint.y);
    resultData.imitationDistance = imitationDistance;
    resultData.date = QDateTime::currentDateTime().toString(Qt::ISODate);

    m_db->insertResult(resultData);
    emit newResultAvailable(resultData);

    qDebug() << "Result processed and emitted.";
}


QString FrameProcessorWorker::processPoint(const cv::Point shootingPoint) {
    m_jsonManager->findKey(shootingPoint);
    return "";
}

void FrameProcessorWorker::run() {
    if (!initializeCamera()) {
        return;
    }

    while (!isInterruptionRequested()) {
        processFrames();
    }
}


void FrameProcessorWorker::startProcessing() {
    if (!isRunning()) {
        start();
    }
}


void FrameProcessorWorker::stopProcessing() {
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


std::pair<cv::Mat, cv::Mat> FrameProcessorWorker::cropAndResizeFrameByCenter(const cv::Mat& frame, cv::Point centerPoint) {
    std::pair<int, int> trainingParams = m_globalsManager.getTrainingParams();
    const auto [distance, imitationDistance] = trainingParams;

    cv::Size cropSize(0, 0);

    switch (distance) {
    case 3:
        cropSize = cv::Size(34, 34);
        break;
    case 4:
        cropSize = cv::Size(23, 23);
        break;
    case 5:
        cropSize = cv::Size(20, 20);
        break;
    case 6:
        cropSize = cv::Size(12, 12);
        break;
    default:
        qWarning() << "Unsupported distance, returning original frame.";
        return std::make_pair(frame, frame);
    }

    int centerX = centerPoint.x;
    int centerY = centerPoint.y;

    int startX = std::max(0, centerX - cropSize.width / 2);
    int startY = std::max(0, centerY - cropSize.height / 2);

    int adjustedWidth = std::min(cropSize.width, frame.cols - startX);
    int adjustedHeigth = std::min(cropSize.height, frame.rows - startY);

    cv::Rect cropRegion(startX, startY, adjustedWidth, adjustedHeigth);
    cv::Mat croppedFrame = frame(cropRegion).clone();

    cv::Mat resizedFrame;
    cv::Size resizeCredentials = m_globalsManager.getTargetImageSize();
    cv::resize(croppedFrame, resizedFrame, resizeCredentials);

    return std::make_pair(croppedFrame, resizedFrame);
}


cv::Mat FrameProcessorWorker::undistortFrame(const cv::Mat &frame) {
    CalibrationResult calibrationResult = CalibrationResult::fromCalibrationFile(":/data/data/calibration_result.xml");
    cv::Mat undistortedFrame;
    cv::undistort(frame, undistortedFrame, calibrationResult.m_cameraMatrix, calibrationResult.m_distortionCoefficients);

    return undistortedFrame;
}


int FrameProcessorWorker::countWhitePixels(const cv::Mat& frame) {
    int whitePixelsCount = 0;

    for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
            int pixelValue = frame.at<uchar>(y, x);

            if (pixelValue > 200) {
                whitePixelsCount++;
            }
        }
    }

    return whitePixelsCount;
}


void FrameProcessorWorker::initializeJsonFile() {
    std::pair<int, int> trainingParams = m_globalsManager.getTrainingParams();
    const int imitationDistance = trainingParams.second;
    const QString jsonFilePath = QString(":data/data/pointsMatrix_%1.json").arg(imitationDistance);

    m_jsonManager = new JsonManager(jsonFilePath);

    if (!m_jsonManager->openFile()) {
        qWarning() << "Failed to initialize JSON file.";
    }
}


QString FrameProcessorWorker::getPoint(int x, int y) {
    std::pair<int, int> trainingParams = m_globalsManager.getTrainingParams();
    const int imitationDistance = trainingParams.second;

    QString filePath = QString(":/data/data/pointsMatrix_%1.json").arg(imitationDistance);
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open JSON file:" << file.errorString();
        return "";
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing json" << jsonError.errorString();
        return "";
    }

    if (!jsonDoc.isObject()) {
        qDebug() << "JSON document is not an object";
        return "";
    }

    QJsonObject jsonObj = jsonDoc.object();
    QStringList keys = jsonObj.keys();

    static const QRegularExpression numberRegex("\\d+");

    for (const QString& key : keys) {
        if (jsonObj[key].isArray()) {
            QJsonArray pointArray = jsonObj[key].toArray();

            for (const auto& point : pointArray) {
                if (point.isObject()) {
                    QJsonObject obj = point.toObject();

                    if (obj.contains("x") && obj.contains("y")) {
                        int jsonX = obj["x"].toInt();
                        int jsonY = obj["y"].toInt();

                        if (jsonX == x && jsonY == y) {
                            QString pointNumber = numberRegex.match(key).captured(0);
                            return pointNumber;
                        }
                    }
                }
            }
        }
    }

    return "";
}


int FrameProcessorWorker::tresholdFrame(const cv::Mat &frame) {
    static int previousPixelCount = 0;

    if (frame.empty()) {
        qWarning() << "Captured an empty frame.";
        return 0;
    }

    cv::Mat grayFrame;
    if (frame.channels() > 1) {
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    } else {
        grayFrame = frame;
    }

    cv::Mat irMask;
    double irThreshold = 253; // Adjust based on IR light intensity
    cv::threshold(grayFrame, irMask, irThreshold, 255, cv::THRESH_BINARY);

    // Count the number of white pixels in the IR mask
    int irPixelCount = cv::countNonZero(irMask);

    // Emit or process IR detection result
    if (irPixelCount > 1000) { // Adjust threshold for pixel area
        qDebug() << "IR light detected. Pixel area:" << irPixelCount;
    }

    int currentPixelCount = cv::countNonZero(irMask);

    // Check if the bright region size increased significantly
    if (currentPixelCount > previousPixelCount * 1.5 && currentPixelCount > 4000) {
        qDebug() << "Shot detected. IR light pixel area increased.";
        return currentPixelCount;
    } else {
        return 0;
    }
}


cv::Mat FrameProcessorWorker::cropFrame(const cv::Mat &frame, cv::Point cropPoint) {
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

    cv::Size cropSize(200, 200);

    int startX = std::max(cropPoint.x - cropSize.width / 2, 0);
    int startY = std::max(cropPoint.y - cropSize.height / 2, 0);

    int cropWidth = std::min(cropSize.width, grayFrame.cols - startX);
    int cropHeight = std::min(cropSize.height, grayFrame.rows - startY);

    cv::Rect croppedRegion(startX, startY, cropWidth, cropHeight);
    cv::Mat croppedFrame = grayFrame(croppedRegion);

    return croppedFrame;
}
