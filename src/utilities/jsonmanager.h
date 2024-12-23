#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QFile>
#include <QDebug>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <opencv2/core.hpp>

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

class JsonManager {
public:
    JsonManager(const QString jsonFilepath) : m_file(jsonFilepath), m_isFileReady(false) {}

    ~JsonManager() {
        if (m_file.isOpen()) {
            m_file.close();
        }
    }

    bool openFile() {
        if (m_file.isOpen()) {
            qWarning() << "File is already open.";
            return true;
        }

        if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file:" << m_file.errorString();
            return false;
        }

        m_isFileReady = true;
        qDebug() << "File opened successfully";

        m_jsonObject = readFromJson();
        if (!m_jsonObject.isEmpty()) {
            qDebug() << "Json read successfully";
        }

        return true;
    }

    QString findKey(const cv::Point shootingPoint) {
        qDebug() << "inside function";
        if (!m_isFileReady) {
            return "0";
        }

        if (m_jsonObject.isEmpty()) {
            return "0";
        }

        for (auto it = m_jsonObject.begin(); it != m_jsonObject.end(); ++it) {
            QString key = it.key();
            QJsonArray pointsArray = it.value().toArray();

            for (const QJsonValue& value : pointsArray) {
                QJsonObject pointObject = value.toObject();
                int x = pointObject["x"].toInt();
                int y = pointObject["y"].toInt();

                if (x == shootingPoint.x && y == shootingPoint.y) {
                    qDebug() << "key found:" << key;
                    return reformatKey(key);
                }
            }
        }

        return "0";
    }

    QString reformatKey(QString key) {
        static QRegularExpression regex("\\d+");
        QRegularExpressionMatchIterator it = regex.globalMatch(key);

        if (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            return match.captured(0);
        }

        return key;
    }

    QJsonObject readFromJson() {
        if (!m_isFileReady) {
            qWarning() << "File is not ready for reading.";
            return QJsonObject();
        }

        if (!m_file.isOpen()) {
            qWarning() << "File is unexpectedly closed.";
            return QJsonObject();
        }

        QByteArray jsonData = m_file.readAll();

        QJsonParseError parseError;
        m_document = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse JSON:" << parseError.errorString();
            return QJsonObject();
        }

        if (!m_document.isObject()) {
            qWarning() << "Invalid JSON format: Root element is not an object.";
            return QJsonObject();
        }

        return m_document.object();
    }

private:
    QFile m_file;
    bool m_isFileReady;
    QJsonObject m_jsonObject;
    QJsonDocument m_document;
};

#endif // JSONMANAGER_H
