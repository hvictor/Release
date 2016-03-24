#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <UIModel.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UIModel *uiModel;

public slots:
    void cpuCoreChanged(int value);
    void startCalibrator();
    void chooseRecordDirectory();
    void updateViewProcessingMode(int processingMode);
    void startApplication();
};

#endif // MAINWINDOW_H
