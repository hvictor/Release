/*
 * HSVManager.h
 *
 *  Created on: Apr 20, 2016
 *      Author: sled
 */

#ifndef HSVMANAGER_H_
#define HSVMANAGER_H_

#include "../Common/opencv_headers.h"

using namespace std;
using namespace cv;

typedef struct
{
	unsigned int Hmin, Smin, Vmin;
	unsigned int Hmax, Smax, Vmax;
} HSVRange;

class HSVManager {

public:
	virtual ~HSVManager();
	HSVRange getHSVRange(Mat roi);
	HSVRange getHSVRange(const uint8_t *data, int image_width, int image_height, int x, int y, int roi_width, int roi_height);
	Mat filterHSVRange_8UC1(Mat frame_RGBA, HSVRange hsvRange, int x, int y, int width, int height);
	Mat filterHSVRange_8UC4(Mat frame_RGBA, HSVRange hsvRange, int x, int y, int width, int height);
	void filterHSVRange(const uint8_t *data, int image_width, int image_height, HSVRange hsvRange, uint8_t *output_data);
	static HSVManager *getInstance();
private:
	HSVManager();

};

#endif /* HSVMANAGER_H_ */
