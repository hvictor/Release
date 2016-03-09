#include <circularindicator.h>

static const float cpu_freq_data[] =
{
    204,
    312,
    564,
    696,
    828,
    960,
    1092,
    1122,
    1224,
    1326,
    1428,
    1530,
    1632,
    1734,
    1836 ,
    1938 ,
    2145 ,
    21165,
    22185,
    23205
};

static const float gpu_freq_data[] =
{
    72, 108, 180, 252, 324, 396, 468, 540, 612, 648, 684, 708, 756, 804, 852
};

const float *CircularIndicator::cpu_freq = cpu_freq_data;
const float *CircularIndicator::gpu_freq = gpu_freq_data;

void CircularIndicator::setProgress(int val)
{
    double cpu_factor = 0.055;
    double gpu_factor = 0.0699;

    if (!flag_gpu) {
        progress = (double)val * cpu_factor;
        freq = cpu_freq[val];
    }
    else {
        progress = (double)val * gpu_factor;
        freq = gpu_freq[val];
    }

    this->update();
}

void CircularIndicator::setCaption(QString caption)
{
    this->caption = caption;
    this->update();
}

void CircularIndicator::setFrequency(int freq)
{
    this->freq = freq;
}

CircularIndicator::CircularIndicator(QString caption, QRectF rectangle, QWidget *parent)
{
    freq = 0;
    this->caption = caption;
    this->rectangle = rectangle;
    this->flag_gpu = false;
}

void CircularIndicator::setGPU()
{
    flag_gpu = true;
}

void CircularIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    //QRectF rectangle(7.0, 7.0, 80.0, 80.0);

    QConicalGradient gradient;
    gradient.setCenter((int)rectangle.center().x(), (int)rectangle.center().y());
    gradient.setAngle(-10);

    if (!flag_gpu) {
        gradient.setColorAt(0.0, QColor(50, 205, 50));
        gradient.setColorAt(0.3, QColor(50, 205, 50));
        gradient.setColorAt(0.6, Qt::yellow);
        gradient.setColorAt(1.0, Qt::red);
    }
    else {
        gradient.setColorAt(0.0, Qt::yellow);
        gradient.setColorAt(1.0, Qt::red);
    }

    QPen pen;
    QFont font = p.font() ;
    font.setPixelSize(11);
    p.setFont(font);
    pen.setColor(Qt::black);
    p.drawText(rectangle, Qt::AlignCenter, caption + "\n" + QString::number(freq) + " MHz");
    //p.drawText(rectangle.bottomLeft().x(), rectangle.bottomLeft().y(), Qt::AlignHCenter, QString::number(freq) + "MHz");
    pen.setBrush(QBrush(gradient));
    pen.setWidth(10);
    p.setPen(pen);

    p.setRenderHint(QPainter::Antialiasing);

    //to understand these magic numbers, look drawArc method in Qt doc
    int startAngle = 0 * 16;
    int spanAngle = progress * 360 * 16;

    p.drawArc(rectangle, startAngle, spanAngle);

}
