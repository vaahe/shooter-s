#ifndef LIGHTINTENSITYMANAGER_H
#define LIGHTINTENSITYMANAGER_H

#include "src/utilities/globalsmanager.h"

#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIntValidator>


class LightIntensityManager {
public:
    static void changeLightIntensity(const QString language) {
        qDebug() << language.toLower();
    }


    static void enterLightIntensity() {
        QWidget *intensityWidget = new QWidget();
        intensityWidget->setWindowTitle("Enter Light Intensity");
        intensityWidget->setFixedSize(300, 150);

        QVBoxLayout *verticalLayout = new QVBoxLayout(intensityWidget);
        QLabel *intensityLabel = new QLabel("Enter intensity value:", intensityWidget);
        QPushButton *enterButton = new QPushButton("Submit", intensityWidget);

        QLineEdit *intensityInput = new QLineEdit(intensityWidget);
        QIntValidator *intValidator = new QIntValidator(0, 10000, intensityInput);
        intensityInput->setValidator(intValidator);

        verticalLayout->addWidget(intensityLabel);
        verticalLayout->addWidget(intensityInput);
        verticalLayout->addWidget(enterButton);

        QObject::connect(enterButton, &QPushButton::clicked, intensityWidget, [intensityInput, intensityWidget]() {
            int intensityValue = intensityInput->text().toInt();

            if (intensityValue > 0) {
                GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
                m_globalsManager.setLightIntensity(intensityValue);
            }

            intensityWidget->close();
        });

        intensityWidget->show();
    }
};

#endif // LIGHTINTENSITYMANAGER_H
