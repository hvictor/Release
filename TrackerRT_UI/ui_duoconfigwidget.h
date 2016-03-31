/********************************************************************************
** Form generated from reading UI file 'duoconfigwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DUOCONFIGWIDGET_H
#define UI_DUOCONFIGWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DUOConfigWidget
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QSlider *duoGainSlider;
    QSlider *duoExposureSlider;
    QSlider *duoIRLEDSlider;
    QLCDNumber *lcdGain;
    QLCDNumber *lcdExposure;
    QLCDNumber *lcdIRLED;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QFrame *line;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *DUOConfigWidget)
    {
        if (DUOConfigWidget->objectName().isEmpty())
            DUOConfigWidget->setObjectName(QStringLiteral("DUOConfigWidget"));
        DUOConfigWidget->resize(421, 171);
        frame = new QFrame(DUOConfigWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(0, 0, 421, 161));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 0, 231, 16));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 30, 91, 16));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 50, 131, 16));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 70, 131, 16));
        duoGainSlider = new QSlider(frame);
        duoGainSlider->setObjectName(QStringLiteral("duoGainSlider"));
        duoGainSlider->setGeometry(QRect(150, 30, 181, 20));
        duoGainSlider->setMinimum(1);
        duoGainSlider->setMaximum(100);
        duoGainSlider->setOrientation(Qt::Horizontal);
        duoExposureSlider = new QSlider(frame);
        duoExposureSlider->setObjectName(QStringLiteral("duoExposureSlider"));
        duoExposureSlider->setGeometry(QRect(150, 50, 181, 20));
        duoExposureSlider->setMinimum(1);
        duoExposureSlider->setMaximum(100);
        duoExposureSlider->setOrientation(Qt::Horizontal);
        duoIRLEDSlider = new QSlider(frame);
        duoIRLEDSlider->setObjectName(QStringLiteral("duoIRLEDSlider"));
        duoIRLEDSlider->setGeometry(QRect(150, 70, 181, 20));
        duoIRLEDSlider->setMinimum(1);
        duoIRLEDSlider->setMaximum(100);
        duoIRLEDSlider->setOrientation(Qt::Horizontal);
        lcdGain = new QLCDNumber(frame);
        lcdGain->setObjectName(QStringLiteral("lcdGain"));
        lcdGain->setGeometry(QRect(345, 30, 41, 21));
        lcdExposure = new QLCDNumber(frame);
        lcdExposure->setObjectName(QStringLiteral("lcdExposure"));
        lcdExposure->setGeometry(QRect(345, 50, 41, 21));
        lcdIRLED = new QLCDNumber(frame);
        lcdIRLED->setObjectName(QStringLiteral("lcdIRLED"));
        lcdIRLED->setGeometry(QRect(345, 70, 41, 21));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(390, 32, 21, 16));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(389, 50, 21, 16));
        label_7 = new QLabel(frame);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(388, 72, 21, 16));
        line = new QFrame(frame);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(10, 90, 401, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton = new QPushButton(frame);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 110, 121, 41));
        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(290, 110, 121, 41));

        retranslateUi(DUOConfigWidget);
        QObject::connect(duoGainSlider, SIGNAL(valueChanged(int)), lcdGain, SLOT(display(int)));
        QObject::connect(duoExposureSlider, SIGNAL(valueChanged(int)), lcdExposure, SLOT(display(int)));
        QObject::connect(duoIRLEDSlider, SIGNAL(valueChanged(int)), lcdIRLED, SLOT(display(int)));

        QMetaObject::connectSlotsByName(DUOConfigWidget);
    } // setupUi

    void retranslateUi(QWidget *DUOConfigWidget)
    {
        DUOConfigWidget->setWindowTitle(QApplication::translate("DUOConfigWidget", "Form", 0));
        label->setText(QApplication::translate("DUOConfigWidget", "DUO3D DUO MLX Parameters:", 0));
        label_2->setText(QApplication::translate("DUOConfigWidget", "Camera Gain:", 0));
        label_3->setText(QApplication::translate("DUOConfigWidget", "Camera Exposure:", 0));
        label_4->setText(QApplication::translate("DUOConfigWidget", "IR Array Intensity:", 0));
        label_5->setText(QApplication::translate("DUOConfigWidget", "%", 0));
        label_6->setText(QApplication::translate("DUOConfigWidget", "%", 0));
        label_7->setText(QApplication::translate("DUOConfigWidget", "%", 0));
        pushButton->setText(QApplication::translate("DUOConfigWidget", "Recalibrate", 0));
        pushButton_2->setText(QApplication::translate("DUOConfigWidget", "Firmware Update", 0));
    } // retranslateUi

};

namespace Ui {
    class DUOConfigWidget: public Ui_DUOConfigWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DUOCONFIGWIDGET_H
