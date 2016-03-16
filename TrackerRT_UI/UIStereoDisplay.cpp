#include "UIStereoDisplay.h"
#include "ui_UIStereoDisplay.h"

UIStereoDisplay::UIStereoDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIStereoDisplay)
{
    ui->setupUi(this);
}

UIStereoDisplay::~UIStereoDisplay()
{
    delete ui;
}
