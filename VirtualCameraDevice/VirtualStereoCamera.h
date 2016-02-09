/*
 * VirtualStereoCamera.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef VIRTUALSTEREOCAMERA_H_
#define VIRTUALSTEREOCAMERA_H_

#include "VirtualCamera.h"
#include "../Configuration/Configuration.h"

typedef struct
{
	Mat L;
	Mat R;
} FramePair;

class VirtualStereoCamera : public VirtualCamera {
public:
	VirtualStereoCamera();
	virtual ~VirtualStereoCamera();
	bool OpenFromVideo(string fileName);
	bool OpenFromFrameSequence(string frameSequenceDirectoryPath, char *frameFileNameFormat);
	FramePair readFramePairFromFrameSequence();
	bool grayscaleFlag;
};

#endif /* VIRTUALSTEREOCAMERA_H_ */
