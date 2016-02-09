/*
 * VirtualMonoCamera.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef VIRTUALMONOCAMERA_H_
#define VIRTUALMONOCAMERA_H_

#include "VirtualCamera.h"
#include "../Configuration/Configuration.h"

class VirtualMonoCamera : public VirtualCamera {
public:
	VirtualMonoCamera();
	virtual ~VirtualMonoCamera();
	virtual bool OpenFromVideo(string fileName);
	virtual bool OpenFromFrameSequence(string frameSequenceDirectoryPath, char *frameFileNameFormat);
	Mat readFrameFromFrameSequence();

private:
	bool grayscaleFlag;

};

#endif /* VIRTUALMONOCAMERA_H_ */
