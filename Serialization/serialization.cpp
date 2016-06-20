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
		printf("Serialization :: Encoding Depth Data\n");
		fwrite(frame_data->depth_data, sizeof(float), w * h, _fp);
		fwrite(&(frame_data->step_depth), sizeof(int), 1, _fp);
	}

	fwrite(&(frame_data->frame_counter), sizeof(int), 1, _fp);
}

void serialize_static_model()
{
	TennisFieldDelimiter *tennisFieldDelimiter = TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter();
	NetVisualProjection netVisualProjection = NetModel::getInstance()->getNetVisualProjection();
	NetCoordinates3D netCoord3D = NetModel::getInstance()->getNetCoordinates3D();
	CalibrationData calibrationData = Configuration::getInstance()->calibrationData;
	PlaneLinearModel planeLinearModel = GroundModel::getInstance()->getGroundPlaneLinearModel();

	fwrite(&(Configuration::getInstance()->dynamicModelParameters.freePlay), sizeof(bool), 1, _fp);

	fwrite(&(calibrationData.targetHSVRange.Hmin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Smin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Vmin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Hmax), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Smax), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Vmax), sizeof(int), 1, _fp);
	printf("%d\n", calibrationData.targetHSVRange.Hmin);
	printf("%d\n",calibrationData.targetHSVRange.Smin);
	printf("%d\n",calibrationData.targetHSVRange.Vmin);
	printf("%d\n",calibrationData.targetHSVRange.Hmax);
	printf("%d\n",calibrationData.targetHSVRange.Smax);
	printf("%d\n",calibrationData.targetHSVRange.Vmax);

	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y), sizeof(float), 1, _fp);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y);

	fwrite(&(netVisualProjection.baseNear.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseNear.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topNear.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topNear.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topFar.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topFar.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseFar.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseFar.y), sizeof(float), 1, _fp);
	printf("%g\n",netVisualProjection.baseNear.x);
	printf("%g\n",netVisualProjection.baseNear.y);
	printf("%g\n",netVisualProjection.topNear.x);
	printf("%g\n",netVisualProjection.topNear.y);
	printf("%g\n",netVisualProjection.topFar.x);
	printf("%g\n",netVisualProjection.topFar.y);
	printf("%g\n",netVisualProjection.baseFar.x);
	printf("%g\n",netVisualProjection.baseFar.y);

	fwrite(&(netCoord3D.baseNear.x), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.baseNear.y), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.baseNear.z), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topNear.x), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topNear.y), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topNear.z), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topFar.x), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topFar.y), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.topFar.z), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.baseFar.x), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.baseFar.y), sizeof(double), 1, _fp);
	fwrite(&(netCoord3D.baseFar.z), sizeof(double), 1, _fp);
	printf("%g\n", netCoord3D.baseNear.x);
	printf("%g\n",netCoord3D.baseNear.y);
	printf("%g\n",netCoord3D.baseNear.z);
	printf("%g\n",netCoord3D.topNear.x);
	printf("%g\n",netCoord3D.topNear.y);
	printf("%g\n",netCoord3D.topNear.z);
	printf("%g\n",netCoord3D.topFar.x);
	printf("%g\n",netCoord3D.topFar.y);
	printf("%g\n",netCoord3D.topFar.z);
	printf("%g\n",netCoord3D.baseFar.x);
	printf("%g\n",netCoord3D.baseFar.y);
	printf("%g\n",netCoord3D.baseFar.z);

	fwrite(&(planeLinearModel.a), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.b), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.c), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.d), sizeof(double), 1, _fp);
	printf("%g\n", planeLinearModel.a);
	printf("%g\n", planeLinearModel.b);
	printf("%g\n", planeLinearModel.c);
	printf("%g\n", planeLinearModel.d);
}

void deserialize_static_model()
{
	TennisFieldDelimiter *tennisFieldDelimiter = TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter();
	NetVisualProjection netVisualProjection = NetModel::getInstance()->getNetVisualProjection();
	NetCoordinates3D netCoord3D = NetModel::getInstance()->getNetCoordinates3D();
	CalibrationData calibrationData;

	fread(&(Configuration::getInstance()->dynamicModelParameters.freePlay), sizeof(bool), 1, _fp);

	fread(&(calibrationData.targetHSVRange.Hmin), sizeof(int), 1, _fp);
	fread(&(calibrationData.targetHSVRange.Smin), sizeof(int), 1, _fp);
	fread(&(calibrationData.targetHSVRange.Vmin), sizeof(int), 1, _fp);
	fread(&(calibrationData.targetHSVRange.Hmax), sizeof(int), 1, _fp);
	fread(&(calibrationData.targetHSVRange.Smax), sizeof(int), 1, _fp);
	fread(&(calibrationData.targetHSVRange.Vmax), sizeof(int), 1, _fp);

	printf("%d\n", calibrationData.targetHSVRange.Hmin);
	printf("%d\n",calibrationData.targetHSVRange.Smin);
	printf("%d\n",calibrationData.targetHSVRange.Vmin);
	printf("%d\n",calibrationData.targetHSVRange.Hmax);
	printf("%d\n",calibrationData.targetHSVRange.Smax);
	printf("%d\n",calibrationData.targetHSVRange.Vmax);

	Configuration::getInstance()->calibrationData = calibrationData;

	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x), sizeof(float), 1, _fp);
	fread(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y), sizeof(float), 1, _fp);

	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x);
	printf("%g\n", TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y);

	NetVisualProjection tmpNetVisualProjection;
	fread(&(tmpNetVisualProjection.baseNear.x), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.baseNear.y), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.topNear.x), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.topNear.y), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.topFar.x), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.topFar.y), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.baseFar.x), sizeof(float), 1, _fp);
	fread(&(tmpNetVisualProjection.baseFar.y), sizeof(float), 1, _fp);

	printf("%g\n",tmpNetVisualProjection.baseNear.x);
	printf("%g\n",tmpNetVisualProjection.baseNear.y);
	printf("%g\n",tmpNetVisualProjection.topNear.x);
	printf("%g\n",tmpNetVisualProjection.topNear.y);
	printf("%g\n",tmpNetVisualProjection.topFar.x);
	printf("%g\n",tmpNetVisualProjection.topFar.y);
	printf("%g\n",tmpNetVisualProjection.baseFar.x);
	printf("%g\n",tmpNetVisualProjection.baseFar.y);
	NetModel::getInstance()->setNetVisualProjection(tmpNetVisualProjection);

	NetCoordinates3D tmpNetCoord3D;
	fread(&(tmpNetCoord3D.baseNear.x), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.baseNear.y), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.baseNear.z), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topNear.x), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topNear.y), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topNear.z), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topFar.x), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topFar.y), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.topFar.z), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.baseFar.x), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.baseFar.y), sizeof(double), 1, _fp);
	fread(&(tmpNetCoord3D.baseFar.z), sizeof(double), 1, _fp);

	printf("%g\n", tmpNetCoord3D.baseNear.x);
	printf("%g\n",tmpNetCoord3D.baseNear.y);
	printf("%g\n",tmpNetCoord3D.baseNear.z);
	printf("%g\n",tmpNetCoord3D.topNear.x);
	printf("%g\n",tmpNetCoord3D.topNear.y);
	printf("%g\n",tmpNetCoord3D.topNear.z);
	printf("%g\n",tmpNetCoord3D.topFar.x);
	printf("%g\n",tmpNetCoord3D.topFar.y);
	printf("%g\n",tmpNetCoord3D.topFar.z);
	printf("%g\n",tmpNetCoord3D.baseFar.x);
	printf("%g\n",tmpNetCoord3D.baseFar.y);
	printf("%g\n",tmpNetCoord3D.baseFar.z);
	NetModel::getInstance()->setNetCoordinates3D(tmpNetCoord3D);
	NetModel::getInstance()->setReady(true);

	double a, b, c, d;
	fread(&a, sizeof(double), 1, _fp);
	fread(&b, sizeof(double), 1, _fp);
	fread(&c, sizeof(double), 1, _fp);
	fread(&d, sizeof(double), 1, _fp);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorX(a);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorY(b);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorZ(c);
	GroundModel::getInstance()->setGroundPlaneLinearModelFactorD(d);
	printf("%g\n",a);
	printf("%g\n",b);
	printf("%g\n",c);
	printf("%g\n",d);
}

bool deserialize_next_frame_data(FrameData *dst)
{
	int w = 640;
	int h = 480;
	short depth_data_avail;

	int rbytes = 0;

	// Test bytes to read available in filesystem buffer
	rbytes = fread(dst->left_data, sizeof(uint8_t), w * h * 4, _fp);

	// No bytes more to deserialize, leave
	if (rbytes <= 0)
		return false;

	// Read depth data availability flag
	fread(&depth_data_avail, sizeof(short), 1, _fp);

	// Depth data is available, set flag in dst and read UINT8 data and matrix step
	if (depth_data_avail == 1) {
		printf("Serialization :: Decoding Depth Data\n");
		dst->depth_data_avail = true;
		fread(dst->depth_data, sizeof(float), w * h, _fp);
		fread(&(dst->step_depth), sizeof(int), 1, _fp);
	}

	// No depth data available, unset flag in dst and leave
	else {
		dst->depth_data_avail = false;
	}

	fread(&(dst->frame_counter), sizeof(int), 1, _fp);

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
