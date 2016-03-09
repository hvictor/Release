#ifndef DUOCONFIGWIDGET_H
#define DUOCONFIGWIDGET_H

#include <QWidget>
#include <UIModel.h>

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
    UIModel *uiModel;
};

#endif // DUOCONFIGWIDGET_H
