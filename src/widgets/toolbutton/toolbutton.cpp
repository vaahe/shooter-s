#include "toolbutton.h"
#include <QIcon>


ToolButton::ToolButton(QLineEdit *parentLineEdit) : QToolButton(parentLineEdit), m_parentLineEdit(parentLineEdit), m_passwordVisible(false) {
    setIcon(QIcon(":/images/images/show.png"));
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QToolButton { border: none; padding: 0px; }");
    setFixedSize(20, 20);

    connect(this, &QToolButton::clicked, this, &ToolButton::togglePasswordVisibility);

    m_parentLineEdit->setTextMargins(0, 0, width(), 0);
    updatePosition();
}


void ToolButton::updatePosition() {
    move(m_parentLineEdit->width() - width(), m_parentLineEdit->height() - height());
}


void ToolButton::togglePasswordVisibility() {
    if (m_passwordVisible) {
        m_parentLineEdit->setEchoMode(QLineEdit::Password);
        setIcon(QIcon(":/images/images/show.png"));
    } else {
        m_parentLineEdit->setEchoMode(QLineEdit::Normal);
        setIcon(QIcon(":/images/images/hide.png"));
    }

    m_passwordVisible = !m_passwordVisible;
}
