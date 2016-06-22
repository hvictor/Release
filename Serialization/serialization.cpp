#include "serialization.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <aio.h>
#include <limits.h>
#include <signal.h>
#include "codec_async_mem.h"

using namespace std;

static FILE *_fp;
static int fd;
static int offset = 0;

static codec_buffer_t *used_buffers[80];

void codec_async_init()
{
	codec_async_mem_init(640, 480, 4);
}
/*
void __hdl_codec_encode_completed(int signo, siginfo_t *info, void *context)
{
	printf("codec :: handler :: encode complete\n");
	if (info->si_signo == SIGIO) {
		printf("codec :: handler :: releasing memory of encode buffer %d\n", info->si_value.sival_int);
		codec_async_mem_release_memory(used_buffers[info->si_value.sival_int]);
		printf("codec :: handler :: memory released\n");
	}

  return;
}
*/

void __hdl_codec_encode_completed_thread(sigval_t val)
{
	printf("Codec :: thread handler :: encode complete\n");

	printf("Codec :: thread handler :: releasing memory of encode buffer %d\n", val.sival_int);
	//codec_async_mem_release_memory(used_buffers[val.sival_int]);
	printf("Codec :: thread handler :: memory released\n");
}

void open_serialization_channel_async(char *fileName)
{
	fd = open(fileName, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, 0x0777);
}

void serialize_frame_data_async(FrameData *frame_data)
{
	size_t offs = 0;

	int frame_width = 640;
	int frame_height = 480;
	int channels = 4;
	size_t bufsiz = 100;/*frame_width * frame_height * channels * sizeof(uint8_t) +	// Reference Camera Frame, UINT8, 4 channels
			sizeof(short) +												// Depth Data availability flag
			frame_width * frame_height * sizeof(float) +				// Depth Data, float, 1 channels
			sizeof(int) +												// Depth Data aligned representation step offset
			sizeof(int);*/												// Frame Counter;

	// Fetch fast codec memory for encode
	printf("codec :: fetching encode buffer memory\n");
	codec_buffer_t *encode_buf = codec_async_mem_fetch_memory();
	printf("codec :: encode buffer memory acquired at %p\n", encode_buf);

	short depth_data_avail = (frame_data->depth_data_avail) ? 1 : 0;

	memset(encode_buf->data, 0, bufsiz);
	// Encode data
	/*
	memcpy(encode_buf->data, frame_data->left_data, 640 * 480 * 4 * sizeof(uint8_t));
	offs += 640 * 480 * 4 * sizeof(uint8_t);
	memcpy(encode_buf->data + offs, &depth_data_avail, sizeof(short));
	offs += sizeof(short);
	*/

	if (depth_data_avail) {
		// Encode depth data
		offs += 640 * 480 * sizeof(float);

		// Encode depth step value
		offs += sizeof(int);

	}
	/*
	memcpy(encode_buf->data + offs, &(frame_data->frame_counter), sizeof(int));
	offs += sizeof(int);
	*/

	// Request async data write
	struct aiocb w_aio;
	bzero((char *)&w_aio, sizeof(struct aiocb));

	w_aio.aio_fildes = fd;
	w_aio.aio_buf = encode_buf->data;
	w_aio.aio_nbytes = bufsiz;
	w_aio.aio_offset = 0;
	w_aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
	//w_aio.aio_sigevent.sigev_notify_function = __hdl_codec_encode_completed_thread;
	//w_aio.aio_sigevent.sigev_signo = SIGIO;
	w_aio.aio_sigevent.sigev_value.sival_int = encode_buf->index;

	used_buffers[encode_buf->index] = encode_buf;

	// Notification callback
	/*
	struct sigaction sig_act;
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = SA_SIGINFO;
	sig_act.sa_sigaction = __hdl_codec_encode_completed;
	sigaction(SIGIO, &sig_act, NULL);
	*/

	// Write request
	printf("codec :: OFFS ZERO :: requesting write of (%d over %d) bytes, encode buffer index: %d\n", offs, bufsiz, encode_buf->index);
	aio_write(&w_aio);
	printf("codec :: OFFS ZERO :: request submitted, encode buffer index: %d\n", encode_buf->index);

	/*
	if (INT_MAX - offset >= (640 * 480 * 4 * sizeof(uint8_t) + sizeof(short) + sizeof(int)))
	{
		printf("serialization :: ERROR :: offset overflow\n");
	}

	offset += 640 * 480 * 4 * sizeof(uint8_t) + sizeof(short) + sizeof(int);
	*/
}

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
	offset += sizeof(bool);

	fwrite(&(calibrationData.targetHSVRange.Hmin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Smin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Vmin), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Hmax), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Smax), sizeof(int), 1, _fp);
	fwrite(&(calibrationData.targetHSVRange.Vmax), sizeof(int), 1, _fp);
	offset += sizeof(int) * 6;

	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomLeft.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->bottomRight.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topRight.y), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.x), sizeof(float), 1, _fp);
	fwrite(&(TennisFieldStaticModel::getInstance()->getTennisFieldDelimiter()->topLeft.y), sizeof(float), 1, _fp);
	offset += sizeof(float) * 8;

	fwrite(&(netVisualProjection.baseNear.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseNear.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topNear.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topNear.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topFar.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.topFar.y), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseFar.x), sizeof(float), 1, _fp);
	fwrite(&(netVisualProjection.baseFar.y), sizeof(float), 1, _fp);
	offset += sizeof(float) * 8;

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
	offset += sizeof(double) * 12;

	fwrite(&(planeLinearModel.a), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.b), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.c), sizeof(double), 1, _fp);
	fwrite(&(planeLinearModel.d), sizeof(double), 1, _fp);
	offset += sizeof(double) * 4;
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
