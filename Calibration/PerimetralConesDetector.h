/*
 * PerimetralConesDetector.h
 *
 *  Created on: May 9, 2016
 *      Author: sled
 */

#ifndef PERIMETRALCONESDETECTOR_H_
#define PERIMETRALCONESDETECTOR_H_

#include "../Common/data_types.h"
#include "../Common/opencv_headers.h"

typedef struct
{
	Rect topLeft;
	Rect bottomLeft;
	Rect bottomRight;
	Rect topRight;
} PerimetralConeSet4;

class PerimetralConesDetector {

public:

	PerimetralConesDetector();
	virtual ~PerimetralConesDetector();
	static PerimetralConesDetector *getInstance();
	PerimetralConeSet4 process_data_8UC4(uint8_t *data, int width, int height);
};

#endif /* PERIMETRALCONESDETECTOR_H_ */
