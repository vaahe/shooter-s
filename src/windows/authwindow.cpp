#include "authwindow.h"
#include "ui_authwindow.h"

AuthWindow::AuthWindow(QWidget *parent) : QWidget(parent), ui(new Ui::AuthWindow), m_dbManager(DatabaseManager::getInstance()) {
    ui->setupUi(this);
    loadLoginData();

    connect(ui->loginBtn, &QPushButton::clicked, this, &AuthWindow::onLogin);
    connect(ui->registerBtn, &QPushButton::clicked, this, &AuthWindow::onRegister);

    connect(ui->redirectToLoginBtn, &QPushButton::clicked, this, &AuthWindow::redirectToLogin);
    connect(ui->redirectToRegisterBtn, &QPushButton::clicked, this, &AuthWindow::redirectToRegister);

    connect(this, &AuthWindow::registerSucceeded, this, &AuthWindow::redirectToLogin);

    connect(m_dbManager, &DatabaseManager::loginFailed, this, &AuthWindow::loginFailed);
    connect(m_dbManager, &DatabaseManager::loginSucceeded, this, &AuthWindow::loginSucceeded);
    connect(m_dbManager, &DatabaseManager::loginSucceeded, this, &AuthWindow::resetLoginValues);

    connect(m_dbManager, &DatabaseManager::registerFailed, this, &AuthWindow::registerFailed);
    connect(m_dbManager, &DatabaseManager::registerSucceeded, this, &AuthWindow::registerSucceeded);
    connect(m_dbManager, &DatabaseManager::registerSucceeded, this, &AuthWindow::resetRegisterValues);

    connect(ui->rememberMeCheckbox, &QCheckBox::stateChanged, this, &AuthWindow::onRememberMe);
}

AuthWindow::~AuthWindow() {
    delete ui;
}


void AuthWindow::onLogin() {
    const QString usernameInputStr = ui->loginUsernameInput->text().trimmed();
    const QString passwordInputStr = ui->loginPasswordInput->text().trimmed();

    validateInput(ui->loginUsernameInput, usernameInputStr);
    validateInput(ui->loginPasswordInput, passwordInputStr);

    Database::User user;
    user.username = usernameInputStr;
    user.password = passwordInputStr;

    m_dbManager->signIn(user);
}

void AuthWindow::onRegister() {
    const QString usernameInputStr = ui->registerUsernameInput->text().trimmed();
    const QString passwordInputStr = ui->registerPasswordInput->text().trimmed();
    const QString repeatPasswordInputStr = ui->registerRepeatPasswordInput->text().trimmed();

    validateInput(ui->registerUsernameInput, usernameInputStr);
    validateInput(ui->registerPasswordInput, passwordInputStr);
    validateInput(ui->registerRepeatPasswordInput, repeatPasswordInputStr);

    if (passwordInputStr != repeatPasswordInputStr) {
        return;
    }

    Database::User user;
    user.username = usernameInputStr;
    user.password = passwordInputStr;

    m_dbManager->signUp(user);
}

void AuthWindow::redirectToLogin() {
    resetRegisterValues();
    ui->stackedWidget->setCurrentIndex(0);
}

void AuthWindow::redirectToRegister() {
    resetLoginValues();
    ui->stackedWidget->setCurrentIndex(1);
}

void AuthWindow::resetLoginValues() {
    ui->loginUsernameInput->setText("");
    ui->loginPasswordInput->setText("");

    ui->loginUsernameInput->setStyleSheet("");
    ui->loginPasswordInput->setStyleSheet("");
}

void AuthWindow::resetRegisterValues() {
    ui->registerUsernameInput->setText("");
    ui->registerPasswordInput->setText("");
    ui->registerRepeatPasswordInput->setText("");

    ui->registerUsernameInput->setStyleSheet("");
    ui->registerPasswordInput->setStyleSheet("");
    ui->registerRepeatPasswordInput->setStyleSheet("");
}

void AuthWindow::validateInput(QLineEdit *inputElement, const QString inputString) {
    if (inputString.isEmpty()) {
        inputElement->setStyleSheet("border: 1px solid red");
    }
}

void AuthWindow::saveLoginData() {
    const QString usernameInputStr = ui->loginUsernameInput->text().trimmed();
    const QString passwordInputStr = ui->loginPasswordInput->text().trimmed();

    QSettings settings("ycrdi", "shooter-s");
    settings.setValue("rememberMe", true);
    settings.setValue("username", usernameInputStr);
    settings.setValue("password", passwordInputStr);
}

void AuthWindow::clearLoginData() {
    QSettings settings("ycrdi", "shooter-s");
    settings.remove("rememberMe");
    settings.remove("username");
    settings.remove("password");
}

void AuthWindow::loadLoginData() {
    QSettings settings("ycrdi", "shooter-s");

    bool rememberMe = settings.value("rememberMe", false).toBool();
    ui->rememberMeCheckbox->setChecked(rememberMe);

    if (rememberMe) {
        QString savedUsername = settings.value("username").toString();
        QString savedPassword = settings.value("password").toString();

        ui->loginUsernameInput->setText(savedUsername);
        ui->loginPasswordInput->setText(savedPassword);
    }
}

void AuthWindow::onRememberMe() {
    if (ui->rememberMeCheckbox->isChecked()) {
       saveLoginData();
    } else {
        clearLoginData();
    }
}
