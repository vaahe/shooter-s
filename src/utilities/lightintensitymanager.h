#ifndef LIGHTINTENSITYMANAGER_H
#define LIGHTINTENSITYMANAGER_H

#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

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
        QLineEdit *intensityInput = new QLineEdit(intensityWidget);
        QPushButton *enterButton = new QPushButton("Submit", intensityWidget);

        verticalLayout->addWidget(intensityLabel);
        verticalLayout->addWidget(intensityInput);
        verticalLayout->addWidget(enterButton);

        QObject::connect(enterButton, &QPushButton::clicked, intensityWidget, [intensityInput, intensityWidget]() {
            QString intensityValue = intensityInput->text();

            if (!intensityValue.isEmpty()) {
                qDebug() << "Entered Intensity Value:" << intensityValue;
            } else {
                qDebug() << "No intensity value entered!";
            }

            intensityWidget->close();
        });

        intensityWidget->show();
    }
};

#endif // LIGHTINTENSITYMANAGER_H
