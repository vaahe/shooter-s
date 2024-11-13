#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->logoutBtn, &QPushButton::clicked, this, &MainWindow::loggedOut);
}

MainWindow::~MainWindow()
{
    delete ui;
}
