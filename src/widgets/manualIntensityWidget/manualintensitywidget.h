#ifndef MANUALINTENSITYWIDGET_H
#define MANUALINTENSITYWIDGET_H

#include "src/utilities/globalsmanager.h"

#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QCloseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ManualIntensityWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ManualIntensityWidget(QWidget *parent = nullptr);
    void setupUI();

signals:
    void intensityChanged(double newValue);
    void modalClosed();

private slots:
    void handleButtonClick();

private:
    QLineEdit *m_intensityInput;
    QPushButton *m_setButton;
    QLabel *m_displayLabel;
    QGroupBox *m_groupBox;
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MANUALINTENSITYWIDGET_H
