#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_cameraCalibrator(nullptr),
    m_frameProcessor(nullptr)
{
    ui->setupUi(this);
    setLanguageMenu();
    setupResultsTable();
    setLightIntensityMenu();
    updateTargetImageSize();

    connect(ui->startCalibrationBtn, &QPushButton::clicked, this, &MainWindow::startCalibrating);
    connect(ui->stopCalibrationBtn, &QPushButton::clicked, this, &MainWindow::stopCalibrating);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &MainWindow::loggedOut);

    connect(ui->startTrainingBtn, &QPushButton::clicked, this, &MainWindow::startProcessing);
    connect(ui->stopTrainingBtn, &QPushButton::clicked, this, &MainWindow::stopProcessing);

    connect(ui->startNewTrainingBtn, &QPushButton::clicked, this, &MainWindow::startNewTraining);
    connect(ui->resultsBtn, &QPushButton::clicked, this, &MainWindow::openResultsWidget);

    connect(ui->soundBtn, &QPushButton::clicked, this, &MainWindow::toggleSound);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::startCalibrating() {
    // if (!checkDevice()) {
    //     QMessageBox::warning(nullptr, tr("Flash key is not found"), tr("Insert the correct flash"), QMessageBox::Ok);
    //     return;
    // }

    if (m_cameraCalibrator == nullptr) {
        m_cameraCalibrator = new CameraCalibrator(this);
    }

    connect(m_cameraCalibrator, &CameraCalibrator::calibrationStarted, ui->startCalibrationBtn, &QPushButton::setDisabled);
    connect(m_cameraCalibrator, &CameraCalibrator::calibrationStarted, ui->stopCalibrationBtn, &QPushButton::setEnabled);
    connect(m_cameraCalibrator, &CameraCalibrator::calibrationStopped, ui->startCalibrationBtn, &QPushButton::setEnabled);
    connect(m_cameraCalibrator, &CameraCalibrator::calibrationStopped, ui->stopCalibrationBtn, &QPushButton::setDisabled);

    m_authClient.applyProcessingSettings();

    QTimer::singleShot(1000, [this]() {
        m_cameraCalibrator->startCalibration();
    });
}


void MainWindow::stopCalibrating() {
    if (m_cameraCalibrator != nullptr) {
        m_cameraCalibrator->stopCalibration();
    }

    delete m_cameraCalibrator;
    m_cameraCalibrator = nullptr;
}


void MainWindow::startProcessing() {
    // if (!checkDevice()) {
    //     QMessageBox::warning(nullptr, "Flash key is not found", "Insert the correct flash", QMessageBox::Ok);
    //     return;
    // }

    if (m_frameProcessor == nullptr) {
        m_frameProcessor = new FrameProcessor(this);
    }

    connect(m_frameProcessor, &FrameProcessor::drawedImageSent, this, &MainWindow::updateBackgroundImage);
    connect(m_frameProcessor, &FrameProcessor::newResultAvailable, this, &MainWindow::fillResultsTable, Qt::UniqueConnection);
    connect(m_frameProcessor, &FrameProcessor::processingStarted, this, &MainWindow::clearResultsTable, Qt::UniqueConnection);

    connect(m_frameProcessor, &FrameProcessor::processingStarted, ui->startTrainingBtn, &QPushButton::setDisabled);
    connect(m_frameProcessor, &FrameProcessor::processingStarted, ui->stopTrainingBtn, &QPushButton::setEnabled);
    connect(m_frameProcessor, &FrameProcessor::processingStopped, ui->startTrainingBtn, &QPushButton::setEnabled);
    connect(m_frameProcessor, &FrameProcessor::processingStopped, ui->stopTrainingBtn, &QPushButton::setDisabled);

    m_frameProcessor->startProcessing();
    resetBackgroundImage();
}


void MainWindow::stopProcessing() {
    if (m_frameProcessor != nullptr) {
        m_frameProcessor->stopProcessing();
    }

    delete m_frameProcessor;
    m_frameProcessor = nullptr;
}


void MainWindow::setLanguageMenu() {
    QMap<QString, QString> languages;
    languages.insert("English", QCoreApplication::applicationDirPath() + "/translations/shooter_s_en_US.qm");
    languages.insert("Armenian", QCoreApplication::applicationDirPath() + "/translations/shooter_s_hy_AM.qm");

    QMenu *languageMenu = ui->menuAdditional->addMenu(tr("Language"));
    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->setExclusive(true);

    for (auto it = languages.begin(); it != languages.end(); ++it) {
        QAction *languageAction = new QAction(tr(it.key().toUtf8().constData()), this);
        languageAction->setCheckable(true);

        if (it.key() == "English") {
            languageAction->setChecked(true);
        }

        languageGroup->addAction(languageAction);
        languageMenu->addAction(languageAction);

        connect(languageAction, &QAction::triggered, this, [this, languageFilePath = it.value()]() {
            switchLanguage(languageFilePath);
            emit languageChanged();
        });
    }
}

void MainWindow::switchLanguage(const QString& languageFilePath) {
    static QTranslator translator;

    qApp->removeTranslator(&translator);

    if (translator.load(languageFilePath)) {
        qApp->installTranslator(&translator);
    } else {
        qDebug() << "Failed to load translation file:" << languageFilePath;
    }

    ui->retranslateUi(this);
}



void MainWindow::setLightIntensityMenu() {
    QActionGroup *lightIntensityActionGroup = new QActionGroup(ui->lightIntensityMenu);
    lightIntensityActionGroup->setExclusive(true);

    const QList<QAction *> actions = ui->lightIntensityMenu->actions();
    for (int i = 0; i < actions.size() - 1; ++i) {
        QAction *action = actions[i];
        lightIntensityActionGroup->addAction(action);

        connect(action, &QAction::triggered, this, [this, action]() {
            m_globalsManager.setLightIntensity(action->toolTip().toInt());
            qDebug() << "23232";
        });
    }

    const QAction *manualInputAction = ui->lightIntensityMenu->actions().back();
    connect(manualInputAction, &QAction::triggered, this, &MainWindow::openManualIntensityWidget);
}


void MainWindow::startNewTraining() {
    NewTrainingModal *modal = new NewTrainingModal();
    modal->openModal();

    QWidget *overlay = new QWidget(this);
    overlay->setGeometry(0, 0, width(), height());
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
    overlay->show();

    connect(modal, &BasicModal::modalClosed, overlay, [overlay]() {overlay->close();});
    connect(modal, &NewTrainingModal::paramsSelected, this, &MainWindow::setSelectedParamsLabel);
    connect(modal, &NewTrainingModal::paramsSelected, &m_globalsManager, &GlobalsManager::setTrainingParams);
    connect(modal, &NewTrainingModal::paramsSelected, this, &MainWindow::resetBackgroundImage);
}


void MainWindow::openResultsWidget() {
    ResultsWidget *resultsWidget = new ResultsWidget();
    resultsWidget->show();

    connect(this, &MainWindow::languageChanged, resultsWidget, &ResultsWidget::retranslateUI);
}


void MainWindow::setupResultsTable() {
    QStringList headers = {"Result", "Date"};
    ui->resultsTable->setColumnCount(headers.size());
    ui->resultsTable->setHorizontalHeaderLabels(headers);
    ui->resultsTable->horizontalHeader()->setStretchLastSection(true);
    ui->resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
}


void MainWindow::fillResultsTable(const Database::Result& resultData) {
    int newRow = ui->resultsTable->rowCount();
    ui->resultsTable->insertRow(newRow);

    QTableWidgetItem *resultItem = new QTableWidgetItem(resultData.result);
    const QStringList formattedDate = resultData.date.split('T');
    QTableWidgetItem *dateItem = new QTableWidgetItem(formattedDate.join(' '));

    ui->resultsTable->setItem(newRow, 0, resultItem);
    ui->resultsTable->setItem(newRow, 1, dateItem);
}


void MainWindow::clearResultsTable() {
    if (ui->resultsTable->rowCount() > 0) {
        ui->resultsTable->setRowCount(0);
    }
}


void MainWindow::toggleSound() {
    QIcon unmuteIcon(":/images/images/unmute.png");
    QIcon muteIcon(":/images/images/mute.png");

    bool isMuted = m_globalsManager.getMuteState();

    if (isMuted) {
        ui->soundBtn->setIcon(unmuteIcon);
    } else {
        ui->soundBtn->setIcon(muteIcon);
    }

    m_globalsManager.setMuteState(!isMuted);
}


void MainWindow::setSelectedParamsLabel(const std::pair<int, int>& selectedParams) {
    int selectedDistance = selectedParams.first;
    int selectedImitationDistance = selectedParams.second;

    const QString selectedValues = "Distance: " + QString::number(selectedDistance) + "\nImit. distance: " + QString::number(selectedImitationDistance);
    ui->selectedParamsLabel->setText(selectedValues);
}

void MainWindow::updateTargetImageSize() {
    ui->targetImageLabel->adjustSize();

    QPixmap pixmap = ui->targetImageLabel->pixmap();
    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaled(QSize(660, 660), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->targetImageLabel->setPixmap(scaledPixmap);

        QSize pixmapSize = ui->targetImageLabel->pixmap().size();
        cv::Size cvTargetImageSize(pixmapSize.width(), pixmapSize.height());
        m_globalsManager.setTargetImageSize(cvTargetImageSize);
    }
    else {
        qDebug() << "Pixmap is null, cannot scale.";
    }
}


void MainWindow::updateBackgroundImage(const cv::Mat &mat) {
    if (mat.empty()) {
        qWarning() << "Empty cv::Mat cannot be converted to QPixmap.";
        return;
    }

    QImage image((unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(image.rgbSwapped());

    ui->targetImageLabel->setPixmap(pixmap);
}


void MainWindow::resetBackgroundImage() {
    auto [distance, imitationDistance] = m_globalsManager.getTrainingParams();
    QString pixmapPath = QString(":/images/images/background_%1.png").arg(imitationDistance);
    qDebug() << "imitation distance:" << pixmapPath;
    QPixmap pixmap(pixmapPath);

    ui->targetImageLabel->setPixmap(pixmap);
}

void MainWindow::openManualIntensityWidget() {
    ManualIntensityWidget *intensityWidget = new ManualIntensityWidget();

    QWidget *overlay = new QWidget(this);
    overlay->setGeometry(0, 0, width(), height());
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
    overlay->show();

    connect(intensityWidget, &ManualIntensityWidget::modalClosed, overlay, &QWidget::close);
    connect(intensityWidget, &ManualIntensityWidget::intensityChanged, overlay, &QWidget::close);
    connect(intensityWidget, &ManualIntensityWidget::intensityChanged, intensityWidget, &ManualIntensityWidget::close);

    intensityWidget->show();
}
