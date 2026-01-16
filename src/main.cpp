#include "src/windows/mainwindow.h"
#include "src/windows/authwindow.h"
#include "src/security/usbchecker.h"
#include "src/database/databasemanager.h"

#include <QMessageBox>
#include <QApplication>
#include <QStackedWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // if (!checkDevice()) {
    //     QMessageBox::warning(nullptr, "Flash key is not found", "Insert the correct flash", QMessageBox::Ok);
    //     return -1;
    // }

    qRegisterMetaType<const Result>("Result");
    qRegisterMetaType<const Result&>("Result&");

    DatabaseManager *dbManager = DatabaseManager::getInstance();

    dbManager->connect();

    QStackedWidget stackedWidget;
    AuthWindow authWindow;
    MainWindow mainWindow;

    authWindow.setWindowTitle("Shooter S");
    mainWindow.setWindowTitle("Shooter S");

    stackedWidget.addWidget(&mainWindow);
    stackedWidget.addWidget(&authWindow);

    QObject::connect(&stackedWidget, &QStackedWidget::currentChanged, &stackedWidget, [&]() {
        QWidget *currentWidget = stackedWidget.currentWidget();

        if (currentWidget) {
            stackedWidget.setWindowTitle(currentWidget->windowTitle());
            stackedWidget.showMaximized();
        }
    });

    QObject::connect(&authWindow, &AuthWindow::loginSucceeded, &stackedWidget, [&]() {
        stackedWidget.setCurrentWidget(&mainWindow);
    });

    QObject::connect(&mainWindow, &MainWindow::loggedOut, &stackedWidget, [&]() {
        stackedWidget.setCurrentWidget(&authWindow);
    });

    stackedWidget.setCurrentWidget(&authWindow);
    stackedWidget.show();

    return a.exec();
}
