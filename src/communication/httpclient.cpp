#include "httpclient.h"
#include <QTimer>

BasicAuthClient::BasicAuthClient(QObject *parent) :
    QObject(parent),
    m_manager(new QNetworkAccessManager(this)),
    m_username("admin"),
    m_password("123456789m") {}

void BasicAuthClient::response() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Request failed:" << reply->errorString();
        } else {
            QByteArray response = reply->readAll();
            qDebug() << "Response:" << response;
        }

        reply->deleteLater();
    }
}

void BasicAuthClient::put(const QString& url, const QByteArray& xmlData) {
    QUrl requestUrl(m_cameraIp + url);
    QNetworkRequest request(requestUrl);

    QString authHeader = createBasicAuthHeader();
    request.setRawHeader("Authorization", authHeader.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");

    QNetworkReply* authenticatedReply = m_manager->put(request, xmlData);
    connect(authenticatedReply, &QNetworkReply::finished, this, &BasicAuthClient::response);
}

QString BasicAuthClient::createBasicAuthHeader() {
    QByteArray auth = m_username.toUtf8() + ":" + m_password.toUtf8();
    QByteArray authBase64 = auth.toBase64();

    qDebug() << authBase64 << auth;

    return "Basic " + authBase64;
}

void BasicAuthClient::setCameraDaytime(const QString &daytime) {
    QByteArray xmlData = QString(R"(
            <IrcutFilter>
                <IrcutFilterType>%1</IrcutFilterType>
                <nightToDayFilterLevel>4</nightToDayFilterLevel>
                <nightToDayFilterTime>5</nightToDayFilterTime>
            </IrcutFilter>
        )").arg(daytime).toUtf8();

    put("/ISAPI/Image/channels/1/ircutFilter/", xmlData);
}

void BasicAuthClient::setCameraExposure(const QString &exposition) {
    QByteArray expositionXmlData = QString(R"(
        <Shutter><ShutterLevel>%1</ShutterLevel></Shutter>
        )").arg(exposition).toUtf8();

    put("/ISAPI/Image/channels/1/shutter/", expositionXmlData);
}

void BasicAuthClient::setCameraColorSettings(const QString &brightness, const QString &contrast, const QString &saturation) {
    QByteArray colorXmlData = QString(R"(
        <Color>
            <brightnessLevel>%1</brightnessLevel>
            <contrastLevel>%2</contrastLevel>
            <saturationLevel>%3</saturationLevel>
        </Color>
    )").arg(brightness, contrast, saturation).toUtf8();

    put("/ISAPI/Image/channels/1/color/", colorXmlData);
}


void BasicAuthClient::setCameraSharpness(const QString &sharpness) {
    QByteArray sharpnessXmlData = QString(R"(
        <Sharpness>
            <SharpnessLevel>%1</SharpnessLevel>
        </Sharpness>
    )").arg(sharpness).toUtf8();

    put("/ISAPI/Image/channels/1/sharpness/", sharpnessXmlData);
}

void BasicAuthClient::applyProcessingSettings() {
    setCameraDaytime("night");

    QTimer::singleShot(500, [this]() {
        setCameraColorSettings("0", "70", "0");
    });


    QTimer::singleShot(1000, [this]() {
        setCameraExposure("1/750");
    });
}
