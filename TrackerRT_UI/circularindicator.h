#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class CircularIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit CircularIndicator(QString caption, QRectF rectangle, QWidget *parent = 0);
    void setGPU();
    static const float *cpu_freq;
    static const float *gpu_freq;

signals:

public slots:
    void setProgress(int val);
    void setCaption(QString caption);
    void setFrequency(int freq);

protected:
    void paintEvent(QPaintEvent *);

private:
    QRectF rectangle;
    double progress;
    QString caption;
    int freq;
    bool flag_gpu;

};

#endif // WIDGET_H
