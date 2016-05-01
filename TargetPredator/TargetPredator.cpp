/*
 * TargetPredator.cpp
 *
 *  Created on: Apr 28, 2016
 *      Author: sled
 */

#include "TargetPredator.h"

TargetPredator::TargetPredator() {
	// TODO Auto-generated constructor stub

}

TargetPredator::~TargetPredator() {
	// TODO Auto-generated destructor stub
}

pred_scan_t TargetPredator::engage_8UC1(uint8_t *data, int width, int height)
{
	pred_scan_t engage_data;
	int scan_xl, scan_xr;
	int scan_len_max = 0;
	int row_scan_max = 0;

	for (int i = 0; i < height; i++) {

		bool scan = false;
		int scan_len = 0;
		int xl, xr;

		uint8_t *offset_buf = data + (width * i) * sizeof(uint8_t);

		for (int j = 0; j < width; j++) {
			if (offset_buf[j] == 255) {
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
				}
			}

			if (scan_len > scan_len_max) {
				scan_len_max = scan_len;
				row_scan_max = i;
				scan_xl = xl;
				scan_xr = xr;
			}
		}
	}

	return engage_data;
}