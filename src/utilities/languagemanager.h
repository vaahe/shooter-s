#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QDebug>
#include <QMessageBox>
#include <QTranslator>
#include <QActionGroup>
#include <QApplication>


class LanguageManager {
public:
    static void switchLanguage(const QString language) {
        QTranslator translator;

        QMap<QString, QString> languageFiles = {
            {"English", QCoreApplication::applicationDirPath() + "/translations/shooter_s_en_US.qm"},
            {"Armenian", QCoreApplication::applicationDirPath() + "/translations/shooter_s_hy_AM.qm"}
        };

        if (!languageFiles.contains(language)) {
            QMessageBox::warning(nullptr, QObject::tr("Language Error"), QObject::tr("Language '%1' not supported.").arg(language));
            return;
        }

        QString translationFile = languageFiles.value(language);
        if (translator.load(translationFile)) {
            qApp->installTranslator(&translator);
        } else {
            QMessageBox::warning(nullptr, QObject::tr("Language Error"), QObject::tr("Translation file for '%1' not found.").arg(language));
        }
    }
};

#endif // LANGUAGEMANAGER_H
