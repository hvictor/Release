#ifndef UIMODEL_H
#define UIMODEL_H

//#include "../Configuration/Configuration.h"
#include <QString>
#include <QObject>

class UIModel : public QObject
{
public:
    static UIModel *getInstance();

public slots:
    // Frame Info
    /*
    void setFrameWidth(QString& wStr);
    void setFrameHeight(QString& hStr);
    void setFrameChannels(int chans);
    void setFrameOutputType(int type);

    // Input Device
    void setInputDevice(int inputDevice);

    // Processing Mode
    void setProcessingMode(int processingMode);

    // DUO Device
    void setDUOGain(int gainPercent);
    void setDUOExposure(int exposurePercent);
    void setDUOIRLedArrayIntensity(int irLEDIntensityPercent);

    // Apply
    void apply();
    */

private:
    UIModel();
    /*
    FrameInfo frameInfo;
    OperationalMode operationalMode;
    OpticalLayerParameters opticalLayerParam;
    DUOStereoCameraHardwareParameters duoParam;
    */
};

#endif // UIMODEL_H
