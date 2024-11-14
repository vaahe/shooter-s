#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include "src/database/databasemanager.h"
#include "src/widgets/toolbutton/toolbutton.h"

#include <QWidget>
#include <QSettings>
#include <QLineEdit>
#include <QToolButton>

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

    void saveLoginData();
    void clearLoginData();
    void loadLoginData();

    void redirectToLogin();
    void redirectToRegister();

    void resetLoginValues();
    void resetRegisterValues();

    void validateInput(QLineEdit* inputElement, const QString inputString);

signals:
    void loginSucceeded();
    void loginFailed();
    void registerSucceeded();
    void registerFailed();

private:
    Ui::AuthWindow *ui;
    DatabaseManager* m_dbManager;
};

#endif // AUTHWINDOW_H
