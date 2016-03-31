/********************************************************************************
** Form generated from reading UI file 'UIStereoDisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UISTEREODISPLAY_H
#define UI_UISTEREODISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UIStereoDisplay
{
public:
    QWidget *widget;

    void setupUi(QDialog *UIStereoDisplay)
    {
        if (UIStereoDisplay->objectName().isEmpty())
            UIStereoDisplay->setObjectName(QStringLiteral("UIStereoDisplay"));
        UIStereoDisplay->resize(640, 480);
        widget = new QWidget(UIStereoDisplay);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 640, 480));

        retranslateUi(UIStereoDisplay);

        QMetaObject::connectSlotsByName(UIStereoDisplay);
    } // setupUi

    void retranslateUi(QDialog *UIStereoDisplay)
    {
        UIStereoDisplay->setWindowTitle(QApplication::translate("UIStereoDisplay", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class UIStereoDisplay: public Ui_UIStereoDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UISTEREODISPLAY_H
