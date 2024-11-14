#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>
#include <QLineEdit>

class ToolButton : public QToolButton {
    Q_OBJECT

public:
    explicit ToolButton(QLineEdit *parentLineEdit);
    void updatePosition();

private slots:
    void togglePasswordVisibility();

private:
    QLineEdit *m_parentLineEdit;
    bool m_passwordVisible;
};

#endif // TOOLBUTTON_H
