#ifndef GLOBALSMANAGER_H
#define GLOBALSMANAGER_H

#include <QObject>
#include <opencv2/core/types.hpp>
#include <QDebug>

class GlobalsManager : public QObject {
    Q_OBJECT

public:
    static GlobalsManager& getInstance() {
        static GlobalsManager instance;
        return instance;
    }

    bool getMuteState() const {
        return m_isMuted;
    }

    void setMuteState(bool newState) {
        m_isMuted = newState;
    }

    cv::Point getCalibrationPoint() const {
        return m_calibrationPoint;
    }

    void setCalibrationPoint(const cv::Point& newCalbirationPoint) {
        m_calibrationPoint = newCalbirationPoint;
    }

    std::pair<int, int> getTrainingParams() const {
        return m_trainingParams;
    }

    void setTrainingParams(const std::pair<int, int>& newTrainingParams) {
        m_trainingParams = newTrainingParams;
    }

    cv::Size getTargetImageSize() const {
        return m_targetImageSize;
    }

    void setTargetImageSize(const cv::Size& newTargetImageSize) {
        m_targetImageSize = newTargetImageSize;
    }

    const QString getUserId() const {
        return m_userId;
    }

    void setUserId(const QString newUserId) {
        m_userId = newUserId;
    }

    GlobalsManager(const GlobalsManager&) = delete;
    GlobalsManager& operator=(const GlobalsManager&) = delete;

private:
    GlobalsManager() {}
    bool m_isMuted = false;
    cv::Point m_calibrationPoint;
    std::pair<int, int> m_trainingParams = {3, 10};
    cv::Size m_targetImageSize;
    QString m_userId;
};

#endif // GLOBALSMANAGER_H
