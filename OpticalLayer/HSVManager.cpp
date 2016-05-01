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

	uint8_t *_data = (uint8_t *)malloc(image_width * image_height * 4 * sizeof(uint8_t));
	memcpy(_data, data, image_width * image_height * 4 * sizeof(uint8_t));

	Mat frame(Size(image_width, image_height), CV_8UC4, _data);
	Mat roi = frame(Range(y, y + roi_height), Range(x, x + roi_width));

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

Mat HSVManager::filterHSVRange_8UC1(Mat frame_RGBA, HSVRange hsvRange, int x, int y, int width, int height)
{
	int w = width;
	int h = height;

	if (x + width >= frame_RGBA.size().width)
		w = frame_RGBA.size().width - x;

	if (y + height >= frame_RGBA.size().height)
		h = frame_RGBA.size().height - y;

	Mat roi_RGBA = frame_RGBA(Range(y, y + h), Range(x, x + w));
	Mat roi_RGB(Size(w, h), CV_8UC3);
	Mat roi_HSV;
	Mat filtered(Size(w, h), CV_8UC1);

	cvtColor(roi_RGBA, roi_RGB, CV_RGBA2RGB);
	cvtColor(roi_RGB, roi_HSV, CV_RGB2HSV);

	inRange(roi_HSV, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);

	return filtered;
}

Mat HSVManager::filterHSVRange_8UC4(Mat frame_RGBA, HSVRange hsvRange, int x, int y, int width, int height)
{
	int w = width;
	int h = height;

	if (x + width >= frame_RGBA.size().width)
		w = frame_RGBA.size().width - x;

	if (y + height >= frame_RGBA.size().height)
		h = frame_RGBA.size().height - y;

	Mat roi_RGBA = frame_RGBA(Range(y, y + h), Range(x, x + w));
	Mat roi_RGB(Size(w, h), CV_8UC3);
	Mat roi_HSV;
	Mat filtered(Size(w, h), CV_8UC1);
	Mat filtered_RGBA(Size(w, h), CV_8UC4);

	cvtColor(roi_RGBA, roi_RGB, CV_RGBA2RGB);
	cvtColor(roi_RGB, roi_HSV, CV_RGB2HSV);

	inRange(roi_HSV, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);

	cvtColor(filtered, filtered_RGBA, CV_GRAY2RGBA);

	return filtered_RGBA;
}

void HSVManager::filterHSVRange(const uint8_t *data, int image_width, int image_height, HSVRange hsvRange, uint8_t *output_data)
{
	uint8_t *_data = (uint8_t *)malloc(image_width * image_height * 4 * sizeof(uint8_t));
	memcpy(_data, data, image_width * image_height * 4 * sizeof(uint8_t));

	Mat frame_RGBA(Size(image_width, image_height), CV_8UC4, _data);
	Mat frame_RGB(Size(image_width, image_height), CV_8UC3);
	Mat filtered(Size(image_width, image_height), CV_8UC1);
	Mat filtered_rgba(Size(image_width, image_height), CV_8UC4);
	Mat frame_HSV;

	cvtColor(frame_RGBA, frame_RGB, CV_RGBA2RGB);
	cvtColor(frame_RGB, frame_HSV, CV_RGB2HSV);

	inRange(frame_HSV, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);

	cvtColor(filtered, filtered_rgba, CV_GRAY2RGBA);
	memcpy(output_data, filtered_rgba.data, image_width * image_height * 4 * sizeof(uint8_t));

	free(_data);
}

void filterHSVRange_out_8UC1(const uint8_t *data, int image_width, int image_height, HSVRange hsvRange, uint8_t *output_data)
{
	uint8_t *_data = (uint8_t *)malloc(image_width * image_height * 4 * sizeof(uint8_t));
	memcpy(_data, data, image_width * image_height * 4 * sizeof(uint8_t));

	Mat frame_RGBA(Size(image_width, image_height), CV_8UC4, _data);
	Mat frame_RGB(Size(image_width, image_height), CV_8UC3);
	Mat filtered(Size(image_width, image_height), CV_8UC1);
	Mat filtered_rgba(Size(image_width, image_height), CV_8UC4);
	Mat frame_HSV;

	cvtColor(frame_RGBA, frame_RGB, CV_RGBA2RGB);
	cvtColor(frame_RGB, frame_HSV, CV_RGB2HSV);

	inRange(frame_HSV, Scalar(hsvRange.Hmin, hsvRange.Smin, hsvRange.Vmin), Scalar(hsvRange.Hmax, hsvRange.Smax, hsvRange.Vmax), filtered);

	//cvtColor(filtered, filtered_rgba, CV_GRAY2RGBA);
	memcpy(output_data, filtered.data, image_width * image_height * sizeof(uint8_t));

	free(_data);
}
