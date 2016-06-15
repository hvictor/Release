#include "serialization.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

using namespace std;

static FILE *_fp;

void serialize_frame_data(FrameData *frame_data)
{
	int w = 640;
	int h = 480;
	short depth_data_avail = (frame_data->depth_data_avail) ? 1 : 0;

	// Left Data:			w * h * 4 * sizeof(uint8_t)
	// Depth Data Avail:	sizeof(short)
	// Depth Data:			w * h * sizeof(float)
	// Step Depth:			sizeof(int)

	fwrite(frame_data->left_data, sizeof(uint8_t), w * h * 4, _fp);
	fwrite(&depth_data_avail, sizeof(short), 1, _fp);
	if (depth_data_avail == 1) {
		fwrite(frame_data->depth_data, sizeof(float), w * h, _fp);
		fwrite(&(frame_data->step_depth), sizeof(int), 1, _fp);
	}
}

bool deserialize_next_frame_data(FrameData *dst)
{
	int w = 640;
	int h = 480;
	short depth_data_avail;

	int rbytes = 0;

	// Test bytes to read available in filesystem buffer
	rbytes = fread(dst->left_data, sizeof(uint8_t), w * h, _fp);

	// No bytes more to deserialize, leave
	if (rbytes <= 0)
		return false;

	// Read depth data availability flag
	fread(&depth_data_avail, sizeof(short), 1, _fp);

	// Depth data is available, set flag in dst and read UINT8 data and matrix step
	if (depth_data_avail == 1) {
		dst->depth_data_avail = true;
		fread(dst->depth_data, sizeof(float), w * h, _fp);
		fread(&(dst->step_depth), sizeof(int), 1, _fp);
	}

	// No depth data available, unset flag in dst and leave
	else {
		dst->depth_data_avail = false;
	}

	return true;
}

void open_deserialization_channel(char *file_name)
{
	_fp = fopen(file_name, "rb");
}

void close_deserialization_channel()
{
	fclose(_fp);
}

void open_serialization_channel(char *file_name)
{
	_fp = fopen(file_name, "wb");
}

void close_serialization_channel()
{
	fclose(_fp);
}
