#include "newtrainingmodal.h"
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

NewTrainingModal::NewTrainingModal()
{
    setupModal();
}

void NewTrainingModal::setupModal()
{
    distance3m = new QRadioButton("3m");
    distance4m = new QRadioButton("4m");
    distance5m = new QRadioButton("5m");
    distance6m = new QRadioButton("6m");

    distance3m->setChecked(true);

    imitationDistance10m = new QRadioButton("10m");
    imitationDistance25m = new QRadioButton("25m");

    imitationDistance10m->setChecked(true);

    approveButton = new QPushButton("Approve");
    cancelButton = new QPushButton("Cancel");

    connect(approveButton, &QPushButton::clicked, this, &NewTrainingModal::approveSelections);
    connect(cancelButton, &QPushButton::clicked, this, &NewTrainingModal::cancelSelections);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *distanceGroupBox = new QGroupBox("Distance");
    QVBoxLayout *distanceLayout = new QVBoxLayout;
    distanceLayout->addWidget(distance3m);
    distanceLayout->addWidget(distance4m);
    distanceLayout->addWidget(distance5m);
    distanceLayout->addWidget(distance6m);
    distanceGroupBox->setLayout(distanceLayout);

    QGroupBox *imitationDistanceGroupBox = new QGroupBox("Imitation Distance");
    QVBoxLayout *imitationDistanceLayout = new QVBoxLayout;
    imitationDistanceLayout->addWidget(imitationDistance10m);
    imitationDistanceLayout->addWidget(imitationDistance25m);
    imitationDistanceGroupBox->setLayout(imitationDistanceLayout);

    mainLayout->addWidget(distanceGroupBox);
    mainLayout->addWidget(imitationDistanceGroupBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(approveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle("New Training Modal");
    setFixedSize(300, 250);
}

void NewTrainingModal::approveSelections()
{
    QString selectedDistance;
    if (distance3m->isChecked()) selectedDistance = "3m";
    else if (distance4m->isChecked()) selectedDistance = "4m";
    else if (distance5m->isChecked()) selectedDistance = "5m";
    else if (distance6m->isChecked()) selectedDistance = "6m";

    QString selectedImitationDistance;
    if (imitationDistance10m->isChecked()) selectedImitationDistance = "10m";
    else if (imitationDistance25m->isChecked()) selectedImitationDistance = "25m";

    const QString content = "Selected Distance: " + selectedDistance + "\nSelected Imitation Distance: " + selectedImitationDistance;
    QMessageBox::information(this, "Selections", content);

    const int parsedSelectedDistance = selectedDistance.split('m')[0].toInt();
    const int parsedSelectedImitationDistance = selectedImitationDistance.split('m')[0].toInt();
    emit paramsSelected(std::pair<int, int>(parsedSelectedDistance, parsedSelectedImitationDistance));

    close();
}

void NewTrainingModal::cancelSelections()
{
    close();
}
