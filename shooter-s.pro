QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/database/databasemanager.cpp \
    src/main.cpp \
    src/windows/mainwindow.cpp \
    src/windows/authwindow.cpp

HEADERS += \
    src/database/databasemanager.h \
    src/windows/mainwindow.h \
    src/windows/authwindow.h

FORMS += \
    src/windows/mainwindow.ui \
    src/windows/authwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
