#ifndef DUOCONFIGWIDGET_H
#define DUOCONFIGWIDGET_H

#include <QWidget>

namespace Ui {
class DUOConfigWidget;
}

class DUOConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DUOConfigWidget(QWidget *parent = 0);
    ~DUOConfigWidget();

private:
    Ui::DUOConfigWidget *ui;
};

#endif // DUOCONFIGWIDGET_H
