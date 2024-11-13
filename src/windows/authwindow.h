#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include "src/database/databasemanager.h"

#include <QWidget>
#include <QLineEdit>

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
    void onRegister();
    void onLogin();

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
