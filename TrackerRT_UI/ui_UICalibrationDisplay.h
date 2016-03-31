/********************************************************************************
** Form generated from reading UI file 'UICalibrationDisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UICALIBRATIONDISPLAY_H
#define UI_UICALIBRATIONDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UICalibrationDisplay
{
public:
    QGridLayout *gridLayout;
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
    QMdiArea *mdiArea;

    void setupUi(QDialog *UICalibrationDisplay)
    {
        if (UICalibrationDisplay->objectName().isEmpty())
            UICalibrationDisplay->setObjectName(QStringLiteral("UICalibrationDisplay"));
        UICalibrationDisplay->resize(859, 579);
        gridLayout = new QGridLayout(UICalibrationDisplay);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame_2 = new QFrame(UICalibrationDisplay);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton = new QPushButton(frame_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(frame_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        checkBox = new QCheckBox(frame_2);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout_2->addWidget(checkBox);

        checkBox_2 = new QCheckBox(frame_2);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout_2->addWidget(checkBox_2);


        horizontalLayout->addLayout(verticalLayout_2);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label = new QLabel(frame_2);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

        gpuSegmentsSlider = new QSlider(frame_2);
        gpuSegmentsSlider->setObjectName(QStringLiteral("gpuSegmentsSlider"));
        gpuSegmentsSlider->setMinimum(1);
        gpuSegmentsSlider->setMaximum(30);
        gpuSegmentsSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(gpuSegmentsSlider, 0, 1, 1, 1);

        gpuDistanceSlider = new QSlider(frame_2);
        gpuDistanceSlider->setObjectName(QStringLiteral("gpuDistanceSlider"));
        gpuDistanceSlider->setMinimum(1);
        gpuDistanceSlider->setMaximum(1024);
        gpuDistanceSlider->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(gpuDistanceSlider, 1, 1, 1, 1);

        lcdGPUSegments = new QLCDNumber(frame_2);
        lcdGPUSegments->setObjectName(QStringLiteral("lcdGPUSegments"));

        gridLayout_2->addWidget(lcdGPUSegments, 0, 2, 1, 1);

        lcdGPUDistance = new QLCDNumber(frame_2);
        lcdGPUDistance->setObjectName(QStringLiteral("lcdGPUDistance"));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        font.setKerning(true);
        lcdGPUDistance->setFont(font);
        lcdGPUDistance->setAutoFillBackground(false);

        gridLayout_2->addWidget(lcdGPUDistance, 1, 2, 1, 1);


        horizontalLayout->addLayout(gridLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_3->addWidget(label_3);

        progressBar_2 = new QProgressBar(frame_2);
        progressBar_2->setObjectName(QStringLiteral("progressBar_2"));
        progressBar_2->setValue(0);

        verticalLayout_3->addWidget(progressBar_2);


        horizontalLayout->addLayout(verticalLayout_3);


        gridLayout->addWidget(frame_2, 1, 0, 1, 1);

        mdiArea = new QMdiArea(UICalibrationDisplay);
        mdiArea->setObjectName(QStringLiteral("mdiArea"));

        gridLayout->addWidget(mdiArea, 0, 0, 1, 1);


        retranslateUi(UICalibrationDisplay);
        QObject::connect(gpuSegmentsSlider, SIGNAL(valueChanged(int)), lcdGPUSegments, SLOT(display(int)));
        QObject::connect(gpuDistanceSlider, SIGNAL(valueChanged(int)), lcdGPUDistance, SLOT(display(int)));

        QMetaObject::connectSlotsByName(UICalibrationDisplay);
    } // setupUi

    void retranslateUi(QDialog *UICalibrationDisplay)
    {
        UICalibrationDisplay->setWindowTitle(QApplication::translate("UICalibrationDisplay", "Tracker RT Calibrator", 0));
        pushButton->setText(QApplication::translate("UICalibrationDisplay", "Acquire Frame", 0));
        pushButton_2->setText(QApplication::translate("UICalibrationDisplay", "Calibrate", 0));
        checkBox->setText(QApplication::translate("UICalibrationDisplay", "3D Ground Mapping", 0));
        checkBox_2->setText(QApplication::translate("UICalibrationDisplay", "Network Reconstruction", 0));
        label->setText(QApplication::translate("UICalibrationDisplay", "[GPU] Segments: ", 0));
        label_2->setText(QApplication::translate("UICalibrationDisplay", "[GPU] Distance: ", 0));
        label_3->setText(QApplication::translate("UICalibrationDisplay", "Elapsed: ---- [ms]", 0));
    } // retranslateUi

};

namespace Ui {
    class UICalibrationDisplay: public Ui_UICalibrationDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UICALIBRATIONDISPLAY_H
