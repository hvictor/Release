/********************************************************************************
** Form generated from reading UI file 'device_conf_widget_duo.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICE_CONF_WIDGET_DUO_H
#define UI_DEVICE_CONF_WIDGET_DUO_H

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
    QLCDNumber *lcdNumber_2;
    QLCDNumber *lcdNumber_3;
    QLCDNumber *lcdNumber_4;
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
        DUOConfigWidget->resize(275, 197);
        frame = new QFrame(DUOConfigWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 10, 251, 181));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 0, 191, 16));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 30, 71, 16));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 50, 71, 16));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 70, 81, 16));
        duoGainSlider = new QSlider(frame);
        duoGainSlider->setObjectName(QStringLiteral("duoGainSlider"));
        duoGainSlider->setGeometry(QRect(100, 30, 91, 20));
        duoGainSlider->setOrientation(Qt::Horizontal);
        duoExposureSlider = new QSlider(frame);
        duoExposureSlider->setObjectName(QStringLiteral("duoExposureSlider"));
        duoExposureSlider->setGeometry(QRect(100, 50, 91, 20));
        duoExposureSlider->setOrientation(Qt::Horizontal);
        duoIRLEDSlider = new QSlider(frame);
        duoIRLEDSlider->setObjectName(QStringLiteral("duoIRLEDSlider"));
        duoIRLEDSlider->setGeometry(QRect(100, 70, 91, 20));
        duoIRLEDSlider->setOrientation(Qt::Horizontal);
        lcdNumber_2 = new QLCDNumber(frame);
        lcdNumber_2->setObjectName(QStringLiteral("lcdNumber_2"));
        lcdNumber_2->setGeometry(QRect(190, 30, 41, 21));
        lcdNumber_3 = new QLCDNumber(frame);
        lcdNumber_3->setObjectName(QStringLiteral("lcdNumber_3"));
        lcdNumber_3->setGeometry(QRect(190, 50, 41, 21));
        lcdNumber_4 = new QLCDNumber(frame);
        lcdNumber_4->setObjectName(QStringLiteral("lcdNumber_4"));
        lcdNumber_4->setGeometry(QRect(190, 70, 41, 21));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(235, 32, 21, 16));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(234, 50, 21, 16));
        label_7 = new QLabel(frame);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(233, 72, 21, 16));
        line = new QFrame(frame);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(10, 100, 231, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton = new QPushButton(frame);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(10, 120, 111, 51));
        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(121, 120, 121, 51));

        retranslateUi(DUOConfigWidget);

        QMetaObject::connectSlotsByName(DUOConfigWidget);
    } // setupUi

    void retranslateUi(QWidget *DUOConfigWidget)
    {
        DUOConfigWidget->setWindowTitle(QApplication::translate("DUOConfigWidget", "Form", 0));
        label->setText(QApplication::translate("DUOConfigWidget", "DUO Stereo Camera Device:", 0));
        label_2->setText(QApplication::translate("DUOConfigWidget", "Gain:", 0));
        label_3->setText(QApplication::translate("DUOConfigWidget", "Exposure:", 0));
        label_4->setText(QApplication::translate("DUOConfigWidget", "IR LED Array:", 0));
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

#endif // UI_DEVICE_CONF_WIDGET_DUO_H
