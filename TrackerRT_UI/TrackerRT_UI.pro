#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T00:17:20
#
#-------------------------------------------------

QT += core gui opengl

LIBS += -lpthread

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrackerRT_UI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    circularindicator.cpp \
    duoconfigwidget.cpp \
    UICalibrationDisplay.cpp \
    UIController.cpp \
    UIModel.cpp \
    UIStereoDisplay.cpp \
    GLWidget.cpp

HEADERS  += mainwindow.h \
    circularindicator.h \
    duoconfigwidget.h \
    UICalibrationDisplay.h \
    UIController.h \
    UIModel.h \
    UIStereoDisplay.h \
    GLWidget.h

FORMS    += mainwindow.ui \
    duoconfigwidget.ui \
    UICalibrationDisplay.ui \
    UIStereoDisplay.ui
