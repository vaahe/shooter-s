#include "ManualIntensityWidget.h"

ManualIntensityWidget::ManualIntensityWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    setWindowTitle(tr("Set manual intensity"));
}

void ManualIntensityWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_groupBox = new QGroupBox(tr("Manual Intensity Control"), this);
    QVBoxLayout *groupLayout = new QVBoxLayout(m_groupBox);

    int currentIntensity = m_globalsManager.getLightIntensity();
    m_displayLabel = new QLabel(QString(tr("Current intensity: %1")).arg(currentIntensity), this);
    groupLayout->addWidget(m_displayLabel);

    QHBoxLayout *inputLayout = new QHBoxLayout();

    m_intensityInput = new QLineEdit(this);
    m_intensityInput->setPlaceholderText(tr("Enter intensity..."));
    inputLayout->addWidget(m_intensityInput);

    m_setButton = new QPushButton(tr("Set Intensity"), this);
    inputLayout->addWidget(m_setButton);

    groupLayout->addLayout(inputLayout);
    m_groupBox->setLayout(groupLayout);

    mainLayout->addWidget(m_groupBox);
    setLayout(mainLayout);

    connect(m_setButton, &QPushButton::clicked, this, &ManualIntensityWidget::handleButtonClick);
}

void ManualIntensityWidget::handleButtonClick() {
    bool ok;
    int newIntensity = m_intensityInput->text().toInt(&ok);

    if (ok) {
        m_globalsManager.setLightIntensity(newIntensity);
        emit intensityChanged(newIntensity);
    }
}

void ManualIntensityWidget::closeEvent(QCloseEvent *event) {
    emit modalClosed();
    event->accept();
}
