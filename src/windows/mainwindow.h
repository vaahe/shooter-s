#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/communication/httpclient.h"
#include "src/utilities/globalsmanager.h"
#include "src/utilities/languagemanager.h"
#include "src/processing/frameprocessor.h"
#include "src/calibration/cameracalibrator.h"
#include "src/utilities/lightintensitymanager.h"
#include "src/widgets/resultswidget/resultswidget.h"
#include "src/widgets/newtrainingmodal/newtrainingmodal.h"

#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QMainWindow>
#include <QActionGroup>

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void setLanguageMenu();
    void setLightIntensityMenu();

public slots:
    void stopCalibrating();
    void startCalibrating();

    void stopProcessing();
    void startProcessing();

    void startNewTraining();
    void openResultsWidget();

    void setupResultsTable();
    void fillResultsTable(const Database::Result& result);
    void clearResultsTable();

    void toggleSound();
    void updateTargetImageSize();
    void setSelectedParamsLabel(const std::pair<int, int>& selectedParams);

    void resetBackgroundImage();
    void updateBackgroundImage(const cv::Mat& frame);

signals:
    void loggedOut();

private:
    Ui::MainWindow *ui;
    CameraCalibrator *m_cameraCalibrator;
    FrameProcessor *m_frameProcessor;
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
    BasicAuthClient m_authClient;
};
#endif // MAINWINDOW_H
