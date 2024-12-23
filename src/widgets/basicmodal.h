#ifndef BASICMODAL_H
#define BASICMODAL_H

#include <QWidget>
#include <QCloseEvent>

class BasicModal : public QWidget
{
    Q_OBJECT
public:
    explicit BasicModal(QWidget *parent = nullptr);

    void openModal();

signals:
    void modalClosed();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // BASICMODAL_H
