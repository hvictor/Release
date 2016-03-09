/********************************************************************************
** Form generated from reading UI file 'UICalibrationDisplay.ui'
**
** Created: Wed Mar 9 11:27:35 2016
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UICALIBRATIONDISPLAY_H
#define UI_UICALIBRATIONDISPLAY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QMdiArea>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UICalibrationDisplay
{
public:
    QGridLayout *gridLayout;
    QMdiArea *mdiArea;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_2;
    QSlider *gpuSegmentsSlider;
    QSlider *gpuDistanceSlider;
    QLCDNumber *lcdGPUSegments;
    QLCDNumber *lcdGPUDistance;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QProgressBar *progressBar_2;

    void setupUi(QDialog *UICalibrationDisplay)
    {
        if (UICalibrationDisplay->objectName().isEmpty())
            UICalibrationDisplay->setObjectName(QString::fromUtf8("UICalibrationDisplay"));
        UICalibrationDisplay->resize(859, 579);
        gridLayout = new QGridLayout(UICalibrationDisplay);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        mdiArea = new QMdiArea(UICalibrationDisplay);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));

        gridLayout->addWidget(mdiArea, 0, 0, 1, 1);

        frame_2 = new QFrame(UICalibrationDisplay);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButton = new QPushButton(frame_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(frame_2);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        checkBox = new QCheckBox(frame_2);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        verticalLayout_2->addWidget(checkBox);

        checkBox_2 = new QCheckBox(frame_2);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        verticalLayout_2->addWidget(checkBox_2);


        horizontalLayout->addLayout(verticalLayout_2);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        gpuSegmentsSlider = new QSlider(frame_2);
        gpuSegmentsSlider->setObjectName(QString::fromUtf8("gpuSegmentsSlider"));
        gpuSegmentsSlider->setMinimum(1);
        gpuSegmentsSlider->setMaximum(30);
        gpuSegmentsSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(gpuSegmentsSlider, 0, 1, 1, 1);

        gpuDistanceSlider = new QSlider(frame_2);
        gpuDistanceSlider->setObjectName(QString::fromUtf8("gpuDistanceSlider"));
        gpuDistanceSlider->setMinimum(1);
        gpuDistanceSlider->setMaximum(1024);
        gpuDistanceSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(gpuDistanceSlider, 1, 1, 1, 1);

        lcdGPUSegments = new QLCDNumber(frame_2);
        lcdGPUSegments->setObjectName(QString::fromUtf8("lcdGPUSegments"));

        gridLayout_2->addWidget(lcdGPUSegments, 0, 2, 1, 1);

        lcdGPUDistance = new QLCDNumber(frame_2);
        lcdGPUDistance->setObjectName(QString::fromUtf8("lcdGPUDistance"));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        font.setKerning(true);
        lcdGPUDistance->setFont(font);
        lcdGPUDistance->setAutoFillBackground(false);

        gridLayout_2->addWidget(lcdGPUDistance, 1, 2, 1, 1);


        horizontalLayout->addLayout(gridLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_3->addWidget(label_3);

        progressBar_2 = new QProgressBar(frame_2);
        progressBar_2->setObjectName(QString::fromUtf8("progressBar_2"));
        progressBar_2->setValue(0);

        verticalLayout_3->addWidget(progressBar_2);


        horizontalLayout->addLayout(verticalLayout_3);


        gridLayout->addWidget(frame_2, 1, 0, 1, 1);


        retranslateUi(UICalibrationDisplay);
        QObject::connect(gpuSegmentsSlider, SIGNAL(valueChanged(int)), lcdGPUSegments, SLOT(display(int)));
        QObject::connect(gpuDistanceSlider, SIGNAL(valueChanged(int)), lcdGPUDistance, SLOT(display(int)));

        QMetaObject::connectSlotsByName(UICalibrationDisplay);
    } // setupUi

    void retranslateUi(QDialog *UICalibrationDisplay)
    {
        UICalibrationDisplay->setWindowTitle(QApplication::translate("UICalibrationDisplay", "Tracker RT Calibrator", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("UICalibrationDisplay", "Acquire Frame", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("UICalibrationDisplay", "Calibrate", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("UICalibrationDisplay", "3D Ground Mapping", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("UICalibrationDisplay", "Network Reconstruction", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("UICalibrationDisplay", "[GPU] Segments: ", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("UICalibrationDisplay", "[GPU] Distance: ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("UICalibrationDisplay", "Elapsed: ---- [ms]", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UICalibrationDisplay: public Ui_UICalibrationDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UICALIBRATIONDISPLAY_H
