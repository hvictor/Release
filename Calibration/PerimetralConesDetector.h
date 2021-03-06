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
#include <list>
#include <vector>
#include <algorithm>


typedef struct
{
	Rect topLeft;
	Rect bottomLeft;
	Rect bottomRight;
	Rect topRight;

	Point vertex_topLeft;
	Point vertex_bottomLeft;
	Point vertex_bottomRight;
	Point vertex_topRight;
} PerimetralConeSet4;

class PerimetralConesDetector {

public:

	PerimetralConesDetector();
	virtual ~PerimetralConesDetector();
	static PerimetralConesDetector *getInstance();
	PerimetralConeSet4 process_data_8UC4(uint8_t *data, int width, int height, bool *status);

private:
	bool perimeter_overlap(Rect A, Rect B);
	bool valueInRange(int value, int min, int max);
	Rect merge_perimeters(vector<Rect> regions);
};

#endif /* PERIMETRALCONESDETECTOR_H_ */
