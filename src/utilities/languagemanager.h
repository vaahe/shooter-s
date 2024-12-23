#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QActionGroup>
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QMessageBox>

class LanguageManager {
public:
    static void switchLanguage(const QString language) {
        qDebug() << "Switching to language:" << language.toLower();

        QTranslator translator;
        QString translationFile = QCoreApplication::applicationDirPath();
        qDebug() << translationFile;

        if (translator.load(translationFile)) {
            qApp->installTranslator(&translator);
            qDebug() << "Language switched to" << language;

            // Optionally, refresh UI elements (like retranslate UI)
            QApplication::instance()->processEvents();
        } else {
            qDebug() << "Translation file for" << language << "not found!";
            // Optionally, show a message if translation file is not found
            QMessageBox::warning(nullptr, QObject::tr("Language Error"), QObject::tr("Translation file for '%1' not found.").arg(language));
        }
    }
};

#endif // LANGUAGEMANAGER_H
