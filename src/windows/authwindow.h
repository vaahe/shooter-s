#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include "src/database/databasemanager.h"
#include "src/utilities/globalsmanager.h"
#include "src/widgets/toolbutton/toolbutton.h"

#include <QMenu>
#include <QWidget>
#include <QSettings>
#include <QLineEdit>
#include <QToolButton>
#include <QTranslator>

namespace Ui {
class AuthWindow;
}

class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();


public slots:
    void onLogin();
    void onRegister();
    void onRememberMe();
    void onLoginSucceeded(const QString& userId);

    void saveLoginData();
    void clearLoginData();
    void loadLoginData();

    void redirectToLogin();
    void redirectToRegister();

    void resetLoginValues();
    void resetRegisterValues();

    void setLanguageMenu();
    void switchLanguage(const QString &languageFilePath);
    void validateInput(QLineEdit* inputElement, const QString inputString);

signals:
    void loginSucceeded();
    void loginFailed();
    void registerSucceeded();
    void registerFailed();


private:
    Ui::AuthWindow *ui;
    DatabaseManager* m_dbManager = DatabaseManager::getInstance();
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
};

#endif // AUTHWINDOW_H
