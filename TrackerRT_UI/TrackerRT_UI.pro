#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T00:17:20
#
#-------------------------------------------------

QT       += core gui
QT += opengl

LIBS += -lglut -lGLU -lpthread

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
    UIOpenGLFrameRenderer.cpp

HEADERS  += mainwindow.h \
    circularindicator.h \
    duoconfigwidget.h \
    UICalibrationDisplay.h \
    UIController.h \
    UIModel.h \
    UIStereoDisplay.h \
    UIOpenGLFrameRenderer.h

FORMS    += mainwindow.ui \
    duoconfigwidget.ui \
    UICalibrationDisplay.ui \
    UIStereoDisplay.ui \
    UIOpenGLFrameRenderer.ui
