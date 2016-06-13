/*
 * converters.h
 *
 *  Created on: Jun 13, 2016
 *      Author: sled
 */

#ifndef CONVERTERS_H_
#define CONVERTERS_H_

#include "opencv_headers.h"
#include "data_types.h"

using namespace cv;

inline Point2f vector2d_to_point2f(Vector2D v)
{
	return Point2f(v.x, v.y);
}


#endif /* CONVERTERS_H_ */
