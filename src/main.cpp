#include "src/windows/mainwindow.h"
#include "src/windows/authwindow.h"
#include "src/database/databasemanager.h"

#include <QApplication>
#include <QStackedWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager *dbManager = DatabaseManager::getInstance();
    dbManager->connect();

    QStackedWidget stackedWidget;
    AuthWindow authWindow;
    MainWindow mainWindow;

    authWindow.setWindowTitle("auth window");
    mainWindow.setWindowTitle("main window");

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
