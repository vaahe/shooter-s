#ifndef NEWTRAININGMODAL_H
#define NEWTRAININGMODAL_H

#include "src/widgets/basicmodal.h"

#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QVariant>

class NewTrainingModal : public BasicModal
{
    Q_OBJECT

public:
    NewTrainingModal();

private slots:
    void approveSelections();
    void cancelSelections();

signals:
    void paramsSelected(const std::pair<int, int>& selectedParams);

private:
    void setupModal();

    QRadioButton *distance3m;
    QRadioButton *distance4m;
    QRadioButton *distance5m;
    QRadioButton *distance6m;

    QRadioButton *imitationDistance10m;
    QRadioButton *imitationDistance25m;

    QPushButton *approveButton;
    QPushButton *cancelButton;
};

#endif // NEWTRAININGMODAL_H
