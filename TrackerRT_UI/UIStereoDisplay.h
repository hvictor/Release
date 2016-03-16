#ifndef UISTEREODISPLAY_H
#define UISTEREODISPLAY_H

#include <QDialog>

namespace Ui {
class UIStereoDisplay;
}

class UIStereoDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit UIStereoDisplay(QWidget *parent = 0);
    ~UIStereoDisplay();

private:
    Ui::UIStereoDisplay *ui;
};

#endif // UISTEREODISPLAY_H
