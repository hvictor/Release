#include "mainwindow.h"
#include <QApplication>
#include <QSlider>
#include <circularindicator.h>

int _argc;
char *_argv[];

extern void run(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    _argc = argc;
    _argv = argv;

    run(argc, argv);
    return 0;

    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    //return a.exec();
}
