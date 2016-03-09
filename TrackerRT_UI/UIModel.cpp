#include "UIModel.h"

UIModel::UIModel()
{

}

UIModel *UIModel::getInstance()
{
    static UIModel *instance = 0;

    if (!instance) {
        instance = new UIModel();
    }

    return instance;
}


// Frame Info
void UIModel::setFrameWidth(QString& wStr)
{
    frameInfo.width = wStr.toInt();
}

void UIModel::setFrameHeight(QString& hStr)
{
    frameInfo.height = hStr.toInt();
}

void UIModel::setFrameChannels(int chans)
{
    if (chans == 0) {
        frameInfo.channels = 1;
    }
    else if (chans == 1) {
        frameInfo.channels = 3;
    }
}

void UIModel::setFrameOutputType(int type)
{
    if (type == 0) {
        frameInfo.outputType = Immuted;
    }
    else if (type == 1) {
        frameInfo.outputType = ForcedRGB;
    }
}

// Input Device
void UIModel::setInputDevice(int inputDevice)
{
    switch (inputDevice) {

        // ZED
    case 0:
        operationalMode.inputDevice = StereoCameraDUO;
        break;

        // DUO
    case 1:
        operationalMode.inputDevice = StereoCameraZED;
        break;

        // Kinect
    case 2:
        operationalMode.inputDevice = IrCameraKinect;
        break;

        // Virtual MONO
    case 3:
        operationalMode.inputDevice = MonoCameraVirtual;
        break;

        // Virtual STEREO
    case 4:
        operationalMode.inputDevice = StereoCameraVirtual;
        break;

    default: break;

    }
}

// Processing Mode
void UIModel::setProcessingMode(int processingMode)
{
    switch(processingMode) {
    case 0:
        operationalMode.processingMode = Tracking;
        break;
    case 1:
        operationalMode.processingMode = Record;
        break;
    default:
        break;
    }
}

void UIModel::setDUOGain(int gainPercent)
{
    duoParam.gain = gainPercent;
}

void UIModel::setDUOExposure(int exposurePercent)
{
    duoParam.exposure = exposurePercent;
}

void UIModel::setDUOIRLedArrayIntensity(int irLEDIntensityPercent)
{
    duoParam.leds = irLEDIntensityPercent;
}

void UIModel::apply()
{
    Configuration::getInstance()->setDUOStereoCameraHardwareParameters(duoParam);
    Configuration::getInstance()->setFrameInfo(frameInfo);
    Configuration::getInstance()->setOperationalMode(operationalMode.inputDevice, operationalMode.processingMode);
    Configuration::getInstance()->setOpticalLayerParameters(opticalLayerParam);
}
