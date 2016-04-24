/*
 * HSVManager.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: sled
 */

#include "HSVManager.h"

HSVManager::~HSVManager()
{
}

HSVManager::HSVManager()
{
}

HSVManager *HSVManager::getInstance()
{
	static HSVManager *instance = 0;

	if (!instance) {
		instance = new HSVManager();
	}

	return instance;
}

HSVRange HSVManager::getHSVRange(const uint8_t *data, int image_width, int image_height, int x, int y, int roi_width, int roi_height)
{
	unsigned int Hmin = 255, Smin = 255, Vmin = 255;
	unsigned int Hmax = 0, Smax = 0, Vmax = 0;
	HSVRange range;

	printf("getHSVRange :: copying data...\n");

	uint8_t *_data = (uint8_t *)malloc(image_width * image_height * 4 * sizeof(uint8_t));
	memcpy(_data, data, image_width * image_height * 4 * sizeof(uint8_t));

	printf("getHSVRange :: OK copying data\n");

	Mat frame(Size(image_width, image_height), CV_8UC4, _data);
	Mat roi = frame(Range(y, y + roi_height), Range(x, x + roi_width));

	Mat hsv_roi;
	Mat roi_rgb;

	printf("getHSVRange :: ROI formed\n");

	cvtColor(roi, roi_rgb, CV_RGBA2RGB);
	cvtColor(roi_rgb, hsv_roi, CV_RGB2HSV);

	printf("getHSVRange :: ROI converted to HSV\n");

	for (int i = 0; i < hsv_roi.rows; i++) {
		for (int j = 0; j < hsv_roi.cols; j++) {
			Vec3b hsv_values = hsv_roi.at<Vec3b>(i, j);
			unsigned int h = hsv_values.val[0];
			unsigned int s = hsv_values.val[1];
			unsigned int v = hsv_values.val[2];

			if (h > Hmax) Hmax = h;
			if (s > Smax) Smax = s;
			if (v > Vmax) Vmax = v;

			if (h < Hmin) Hmin = h;
			if (s < Smin) Smin = s;
			if (v < Vmin) Vmin = v;
		}
	}

	range.Hmin = Hmin;
	range.Smin = Smin;
	range.Vmin = Vmin;
	range.Hmax = Hmax;
	range.Smax = Smax;
	range.Vmax = Vmax;

	printf("getHSVRange :: returning range\n");

	return range;
}


HSVRange HSVManager::getHSVRange(Mat roi)
{
	unsigned int Hmin = 255, Smin = 255, Vmin = 255;
	unsigned int Hmax = 0, Smax = 0, Vmax = 0;
	HSVRange range;

	Mat hsv_roi;
	Mat roi_rgb;

	cvtColor(roi, roi_rgb, CV_RGBA2RGB);
	cvtColor(roi_rgb, hsv_roi, CV_RGB2HSV);

	for (int i = 0; i < hsv_roi.rows; i++) {
		for (int j = 0; j < hsv_roi.cols; j++) {
			Vec3b hsv_values = hsv_roi.at<Vec3b>(i, j);
			unsigned int h = hsv_values.val[0];
			unsigned int s = hsv_values.val[1];
			unsigned int v = hsv_values.val[2];

			if (h > Hmax) Hmax = h;
			if (s > Smax) Smax = s;
			if (v > Vmax) Vmax = v;

			if (h < Hmin) Hmin = h;
			if (s < Smin) Smin = s;
			if (v < Vmin) Vmin = v;
		}
	}

	range.Hmin = Hmin;
	range.Smin = Smin;
	range.Vmin = Vmin;
	range.Hmax = Hmax;
	range.Smax = Smax;
	range.Vmax = Vmax;

	return range;
}

Mat HSVManager::filterHSVRange(Mat frame, HSVRange hsvRange)
{
	Mat filtered(frame.size(), CV_8UC1);
	Mat filtered_rgba(Size(frame.size().width, frame.size().height), CV_8UC4);

	inRange(frame, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);
	cvtColor(filtered, filtered_rgba, CV_GRAY2RGBA);

	return filtered_rgba;
}

Mat HSVManager::filterHSVRange(const uint8_t *data, int image_width, int image_height, HSVRange hsvRange)
{
	printf("filterHSVRange :: copying data\n");

	uint8_t *_data = (uint8_t *)malloc(image_width * image_height * 4 * sizeof(uint8_t));
	memcpy(_data, data, image_width * image_height * 4 * sizeof(uint8_t));

	printf("filterHSVRange :: data copied\n");

	Mat frame(Size(image_width, image_height), CV_8UC4, _data);
	Mat filtered(Size(image_width, image_height), CV_8UC1);
	Mat filtered_rgba(Size(image_width, image_height), CV_8UC4);

	printf("filterHSVRange :: thresholding...\n");

	inRange(frame, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);
	cvtColor(filtered, filtered_rgba, CV_GRAY2RGBA);

	printf("filterHSVRange :: returning\n");

	return filtered_rgba;
}

