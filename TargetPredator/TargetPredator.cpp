/*
 * TargetPredator.cpp
 *
 *  Created on: Apr 28, 2016
 *      Author: sled
 */

#include "TargetPredator.h"
#include <stdio.h>
#include <math.h>

TargetPredator::TargetPredator()
{
	_configuration = Configuration::getInstance();
	_staticModel = TennisFieldStaticModel::getInstance();
	_lowPassFilterX = Configuration::getInstance()->getOpticalLayerParameters().linearLowPassFilterX;
	_lowPassFilterY = Configuration::getInstance()->getOpticalLayerParameters().linearLowPassFilterY;
	TargetLost = false;
	_tgtUnseen = 0;
}

TargetPredator *TargetPredator::getInstance()
{
	static TargetPredator *instance = 0;

	if (!instance) {
		instance = new TargetPredator();
	}

	return instance;
}

TargetPredator::~TargetPredator() {
	// TODO Auto-generated destructor stub
}

void TargetPredator::update_filter_param()
{
	_lowPassFilterX = Configuration::getInstance()->getOpticalLayerParameters().linearLowPassFilterX;
	_lowPassFilterY = Configuration::getInstance()->getOpticalLayerParameters().linearLowPassFilterY;

	printf("TargetPredator :: Filter :: x=%.2f, y=%.2f\n", _lowPassFilterX, _lowPassFilterY);
}

void TargetPredator::update_free_play_param()
{
	_freePlay = Configuration::getInstance()->dynamicModelParameters.freePlay;

	printf("TargetPredator :: Play :: Free Play = %d\n", _freePlay);
}

list<pred_state_t> *TargetPredator::get_state()
{
	return &state;
}

bool TargetPredator::compute_impact_status(double old_velocityX, double old_velocityY, double velocityX, double velocityY)
{

	// Multiply by a factor of 100 to avoid numeric errors
	old_velocityX *= 100.0;
	old_velocityY *= 100.0;
	velocityX *= 100.0;
	velocityY *= 100.0;

	double inner_prod = old_velocityX * velocityX + old_velocityY * velocityY;
	double norm_old = sqrt(old_velocityX * old_velocityX + old_velocityY * old_velocityY);
	double norm = sqrt(velocityX * velocityX + velocityY * velocityY);

	double cos_val = inner_prod / (norm * norm_old);
	double angle = acos(cos_val) * 180.0 / M_PI;

	if ((angle > 90.0 && (old_velocityY > 0.0 && velocityY < 0.0)))// || ((old_velocityY * velocityY < 0.0) && velocityY < 0.0))
		return true;

	return false;
}

pred_state_t TargetPredator::confirm_latest_impact()
{
	pred_state_t ret;
	ret.x = -1;
	ret.y = -1;

	for (list<pred_state_t>::iterator it = state.begin(); it != state.end(); ++it)
	{
		if (it->impact_status) {
			/*
			if (it->confirmation) {
				ret = *it;
				break;
			}
			*/

			it->confirmation = true;

			ret.x = it->x;
			ret.y = it->y;
			break;
		}
	}

	return ret;
}

void TargetPredator::update_state(int x, int y)
{
	pred_state_t tracker_state;
	list<pred_state_t>::iterator prev;

	tracker_state.x = x;
	tracker_state.y = y;

	if (state.size() > 0) {
		prev = state.begin();
		tracker_state.Vx = x - prev->x;
		tracker_state.Vy = y - prev->y;

		tracker_state.display_x = _lowPassFilterX * x + (1.0 - _lowPassFilterX) * prev->display_x;
		tracker_state.display_y = _lowPassFilterY * y + (1.0 - _lowPassFilterY) * prev->display_y;
	}
	else {
		tracker_state.Vx = 0;
		tracker_state.Vy = 0;

		tracker_state.display_x = x;
		tracker_state.display_y = y;
	}

	// Reset value
	tracker_state.impact_status = false;

	// Impact signaling
	if (state.size() > 1) {
		if (compute_impact_status((double)prev->Vx, (double)prev->Vy, (double)tracker_state.Vx, (double)tracker_state.Vy)) {
			tracker_state.impact_status = true;

			/* Testing
			if (!_freePlay) {
				_staticModel->updateScoreTracking_2D((double)prev->x, (double)prev->y);
			}
			*/
		}
	}

	state.push_front(tracker_state);

	trackingWnd.cx = x;
	trackingWnd.cy = y;

	if (_configuration->dynamicModelParameters.trackingWndEnabled) {

		if (_configuration->dynamicModelParameters.trackingWndMode == StaticTrackingWindow) {
			trackingWnd.w = _configuration->dynamicModelParameters.trackingWndSize;
			trackingWnd.h = _configuration->dynamicModelParameters.trackingWndSize;
		}
		else if (_configuration->dynamicModelParameters.trackingWndMode == AdaptiveTrackingWindow) {
			_configuration->dynamicModelParameters.trackingWndSize = std::max(tracker_state.Vx, tracker_state.Vy);
			trackingWnd.w = _configuration->dynamicModelParameters.trackingWndSize * _configuration->dynamicModelParameters.trackingWndAdaptiveFactorX;
			trackingWnd.h = _configuration->dynamicModelParameters.trackingWndSize * _configuration->dynamicModelParameters.trackingWndAdaptiveFactorY;
		}
	}
}

pred_wnd_t TargetPredator::get_tracking_wnd()
{
	return trackingWnd;
}

pred_scan_t TargetPredator::engage_Mat8UC1(Mat f_8UC1, int width, int height)
{
	pred_scan_t engage_data;
	int scan_xl = 0, scan_xr = 0;
	int scan_len_max = 0;
	int row_scan_max = 0;

	engage_data.row = 0;
	engage_data.xl = 0;
	engage_data.xr = 0;

	for (int i = 0; i < height; i++) {

		bool scan = false;
		int scan_len = 0;
		int xl = 0, xr = 0;

		for (int j = 0; j < width; j++) {
			if (f_8UC1.at<uint8_t>(j, i) > 0) {
				if (scan) {
					scan_len++;
				}
				else {
					scan = true;
					xl = j;
				}
			}
			else {
				if (scan) {
					scan = false;
					xr = j - 1;

					if (scan_len > scan_len_max) {
						scan_len_max = scan_len;
						row_scan_max = i;
						scan_xl = xl;
						scan_xr = xr;
					}
				}
			}
		}
	}

	engage_data.row = row_scan_max;
	engage_data.xl = scan_xl;
	engage_data.xr = scan_xr;

	return engage_data;
}

pred_scan_t TargetPredator::engage_8UC1(uint8_t *data, int width, int height)
{
	pred_scan_t engage_data;
	int scan_xl = 0, scan_xr = 0;
	int scan_len_max = 0;
	int row_scan_max = 0;

	engage_data.row = 0;
	engage_data.xl = 0;
	engage_data.xr = 0;

	for (int i = 0; i < height; i++) {

		bool scan = false;
		int scan_len = 0;
		int xl = 0, xr = 0;

		uint8_t *offset_buf = data + (width * i);

		for (int j = 0; j < width; j++) {
			if (offset_buf[j] > 0) {
				if (scan) {
					scan_len++;
				}
				else {
					scan = true;
					xl = j;
				}
			}
			else {
				if (scan) {
					scan = false;
					xr = j - 1;

					if (scan_len > scan_len_max) {
						scan_len_max = scan_len;
						row_scan_max = i;
						scan_xl = xl;
						scan_xr = xr;
					}
				}
			}
		}
	}

	engage_data.row = row_scan_max;
	engage_data.xl = scan_xl;
	engage_data.xr = scan_xr;

	// Increment TGT LST counter
	if (row_scan_max == 0 && scan_xl == 0 && scan_xr == 0)
	{
		_tgtUnseen++;

		if (_tgtUnseen >= _configuration->dynamicModelParameters.targetPredatorTGTLOSTFramesThreshold)
		{
			_tgtUnseen = 0;
			TargetLost = true;
			state.clear();
		}
	}
	else
	{
		TargetLost = false;
	}

	return engage_data;
}

pred_scan_t TargetPredator::engage_8UC4(uint8_t *data, int width, int height)
{
	pred_scan_t engage_data;
	int scan_xl, scan_xr;
	int scan_len_max = 0;
	int row_scan_max = 0;

	engage_data.row = 0;
	engage_data.xl = 0;
	engage_data.xr = 0;

	for (int i = 0; i < height; i++) {

		bool scan = false;
		int scan_len = 0;
		int xl = 0, xr = 0;

		uint8_t *offset_buf = data + (width * i) * 4;

		for (int j = 0; j < width; j++) {
			if (offset_buf[j] > 0) {
				if (scan) {
					scan_len++;
				}
				else {
					scan = true;
					xl = j;
				}
			}
			else {
				if (scan) {
					scan = false;
					xr = j - 1;

					if (scan_len > scan_len_max) {
						scan_len_max = scan_len;
						row_scan_max = i;
						scan_xl = xl;
						scan_xr = xr;
					}
				}
			}
		}
	}

	engage_data.row = row_scan_max;
	engage_data.xl = scan_xl;
	engage_data.xr = scan_xr;

	return engage_data;
}
