#include "basicmodal.h"

BasicModal::BasicModal(QWidget *parent) : QWidget{parent}
{
    setFixedSize(300, 200);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(windowFlags() | Qt::Dialog);
}

void BasicModal::openModal() {
    show();
}


void BasicModal::closeEvent(QCloseEvent *event) {
    emit modalClosed();

    event->accept();
}
