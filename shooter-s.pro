QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 debug

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\Users\User\Downloads\opencv\build\include

LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_ml480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_dnn480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_core480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_flann480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_video480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_photo480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_highgui480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_imgproc480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_calib3d480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_videoio480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_imgcodecs480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_stitching480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_objdetect480.dll
LIBS += C:\Users\User\Downloads\opencv\build\bin\libopencv_features2d480.dll
LIBS += -lsetupapi

SOURCES += \
    src/calibration/cameracalibrator.cpp \
    src/calibration/cameracalibratorworker.cpp \
    src/communication/httpclient.cpp \
    src/database/databasemanager.cpp \
    src/main.cpp \
    src/processing/frameprocessor.cpp \
    src/processing/frameprocessorworker.cpp \
    src/widgets/basicmodal.cpp \
    src/widgets/newtrainingmodal/newtrainingmodal.cpp \
    src/widgets/resultswidget/resultswidget.cpp \
    src/widgets/toolbutton/toolbutton.cpp \
    src/windows/mainwindow.cpp \
    src/windows/authwindow.cpp

HEADERS += \
    src/calibration/cameracalibrator.h \
    src/calibration/cameracalibratorworker.h \
    src/communication/httpclient.h \
    src/database/databasemanager.h \
    src/processing/frameprocessor.h \
    src/processing/frameprocessorworker.h \
    src/security/usbchecker.h \
    src/utilities/calibrationresult.h \
    src/utilities/globalsmanager.h \
    src/utilities/jsonmanager.h \
    src/utilities/languagemanager.h \
    src/utilities/lightintensitymanager.h \
    src/utilities/soundplayer.h \
    src/widgets/basicmodal.h \
    src/widgets/newtrainingmodal/newtrainingmodal.h \
    src/widgets/resultswidget/resultswidget.h \
    src/widgets/toolbutton/toolbutton.h \
    src/windows/mainwindow.h \
    src/windows/authwindow.h

FORMS += \
    src/windows/mainwindow.ui \
    src/windows/authwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = resources/images/favicon.ico

RESOURCES += \
    resources/app.qrc

TRANSLATIONS += \
    translations/shooter_s_en_US.ts \
    translations/shooter_s_hy_AM.ts
