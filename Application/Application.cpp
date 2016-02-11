/*
 * *****************************************************************
 * Module: 		Real-Time Tennis Tracker Application
 * Description:		Main Module
 *
 * Author:		Victor Huerlimann
 * *****************************************************************
 */

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <time.h>
#include <Dense3D.h>

#include "gpu_players_detect.h"

#include "../Configuration/configs.h"
#include "../FastMemory/fast_mem_pool.h"
#include "../SpinlockQueue/array_spinlock_queue.h"
#include "../RealTime/nanotimer_rt.h"

#include "../OpticalLayer/TrajectoryRecognizer.h"
#include "../OpticalLayer/OpticalFlowRegionRecognizer.h"
#include "../DynamicModel3D/DynamicModel3D.h"

#include "../RealTime/nanotimer_rt.h"
#include "../StereoVision/StereoVision.h"
#include "../AugmentedReality/OverlayRenderer.h"
#include "../StaticModel/TennisFieldStaticModel.h"
#include "../OpticalLayer/pyrlk_optical_flow.h"

#include "../OpticalLayer/FlowProcessor.h"
#include "../OpticalLayer/StatefulObjectFilter.h"
#include "../Calibration/TennisFieldCalibrator.h"
#include "../Calibration/IntersectionPointsDetector.h"
#include "../Configuration/Configuration.h"
#include "../SensorAbstractionLayer/ZEDStereoSensorDriver.h"
#include "../SensorAbstractionLayer/DUOStereoSensorDriver.h"
#include "../SensorAbstractionLayer/KinectIRSensorDriver.h"

#include "../VirtualCameraDevice/VirtualMonoCamera.h"
#include "../VirtualCameraDevice/VirtualStereoCamera.h"
#include "../VirtualCameraDevice/StereoRecorder.h"
#include "../VirtualCameraDevice/MonoRecorder.h"

using namespace std;
using namespace cv;
using namespace cv::gpu;

// DUO Frame dimensions
#define WIDTH	320
#define HEIGHT	240

// CUDA lines detector command line parameters
double minLineLength = 40;
double maxLineGap = 5;

// Linear Low-Pass filter decoupled parameters
double ax = 0.5;
double ay = 0.5;
int filter_iterat = 1;

// Parallel Optical Flow data types
typedef struct
{
	int camera_flag;	// 0 = Left, 1 = Right
	GpuMat *gpu_frame0;
	GpuMat *gpu_frame1;
} ParallelOpticalFlowData;

// Destination GPU memory containing optical flow displacement information
GpuMat d_flowx_L, d_flowy_L;
GpuMat d_flowx_R, d_flowy_R;

// Detected players
vector<Rect> players;
Dense3DInstance dense3d;

// Trajectory recognizers for left and right frames
TrajectoryRecognizer *trajectoryRecognizer_L;
TrajectoryRecognizer *trajectoryRecognizer_R;

//
// Global objects
//

// Fast concurrent spinlock queues
static SpinlockQueue inputFramesQueue;
static SpinlockQueue outputFramesQueue;

// Configuration
Configuration *configuration;

// Thread handles
pthread_t frame_processor_thread;
pthread_t frame_output_thread;

//
// Frames Processor process
//
void *frames_processor(void *)
{
	int bufp = 0;
	FrameData *frame_data[2];
	StatefulObjectFilter *statefulObjectFilter = new StatefulObjectFilter();

	if (configuration->getOperationalMode().processingMode == Record) {
		printf("Stereo Application :: Frames Processor :: Leaving\n");
		return NULL;
	}

	int width = Configuration::getInstance()->getFrameInfo().width;
	int height = Configuration::getInstance()->getFrameInfo().height;
	int channels = Configuration::getInstance()->getFrameInfo().channels;
	bool force_rgb_output = (Configuration::getInstance()->getFrameInfo().outputType == ForcedRGB);

	while (1) {
		FrameData *fd;

		// Fetch new frame data from fast concurrent input queue
		if (array_spinlock_queue_pull(&inputFramesQueue, (void **)&fd) < 0) {
			continue;
		}

		// Store frame data into local buffer
		frame_data[bufp] = fd;

		if (!bufp) {
			bufp++;
			continue;
		}

		//
		// Process frame pair
		//
		printf("Frames processor :: Processing...\n");

		Mat frame0_L, frame0_R;
		Mat frame1_L, frame1_R;

		if (channels == 1) {
			// Left data
			frame0_L = Mat(Size(width, height), CV_8UC1, frame_data[0]->left_data);
			frame1_L = Mat(Size(width, height), CV_8UC1, frame_data[1]->left_data);

			// Right data
			frame0_R = Mat(Size(width, height), CV_8UC1, frame_data[0]->right_data);
			frame1_R = Mat(Size(width, height), CV_8UC1, frame_data[1]->right_data);
		}
		else if (channels == 3) {
			// Left data
			frame0_L = Mat(Size(width, height), CV_8UC3, frame_data[0]->left_data);
			frame1_L = Mat(Size(width, height), CV_8UC3, frame_data[1]->left_data);

			// Right data
			frame0_R = Mat(Size(width, height), CV_8UC3, frame_data[0]->right_data);
			frame1_R = Mat(Size(width, height), CV_8UC3, frame_data[1]->right_data);
		}

		players = detectPlayers(frame0_L);

		// Compute CUDA Lucas-Kanade sparse Optical Flow
		vector<FlowObject> flowObjects = FlowProcessor_ProcessSparseFlow(frame0_L, frame1_L, players);

		// Compute mean motion centers (Disabled, possible correspondance precision loss)
		//statefulObjectFilter->computeMeanMotionCenters();

		vector<StateRelatedTable *> t = statefulObjectFilter->getTrajectoryCandidateTables();

		// If forcing RGB output
		if (force_rgb_output) {
			printf("[Debug] RGB Forced output Frame Type\n");
			GpuMat d_frame(frame0_L);
			GpuMat d_frame_BGR;
			Mat h_frame_BGR;
			gpu::cvtColor(d_frame, d_frame_BGR, CV_GRAY2BGR);
			d_frame_BGR.download(h_frame_BGR);

			// Draw motion centers
			for (int j = 0; j < statefulObjectFilter->getTables().size(); j++) {
				StateRelatedTable *table = (statefulObjectFilter->getTables())[j];

				for (int k = 0; k < table->relatedStates.size(); k++) {
					Point p(table->relatedStates[k]->state.x, table->relatedStates[k]->state.y);
					Point q = table->relatedStates[k]->state.nextPosition;
					rectangle(h_frame_BGR, Point(p.x - 4, p.y - 4), Point(p.x + 4, p.y + 4), Scalar(0, 0, 255), 1);
					line(h_frame_BGR, p, q, Scalar(0, 200, 255));
					rectangle(h_frame_BGR, Point(q.x - 4, q.y - 4), Point(q.x + 4, q.y + 4), Scalar(0, 200, 255), 1);
					if (k == table->relatedStates.size()-1) {
						char id[30];
						sprintf(id, "%d", table->stateTableID);
						putText(frame, id, Point(p.x-10, p.y-15), FONT_HERSHEY_SIMPLEX, 0.3, cvScalar(50, 205, 50), 1, CV_AA);
					}
				}
				//line(h_frame_BGR, p, Point2f(p.x-flowObjects[j].displacement_x*10, p.y-flowObjects[j].displacement_y*10), Scalar(0, 200, 200));
			}

			/*
			for (int k = 0; k < t.size(); k++) {
				FlowObject tmp = t[k]->relatedStates[t[k]->relatedStates.size()-1]->state;
				circle(h_frame_BGR, Point2f(tmp.x, tmp.y), 8, (0, 0, 255), 2);
				line(h_frame_BGR, Point2f(tmp.x, tmp.y), Point2f(tmp.x+tmp.displacement_x*3, tmp.y+tmp.displacement_y*3), Scalar(0,200,255), 1);
			}
			*/

			statefulObjectFilter->tick();

			drawRectAroundPlayers(h_frame_BGR, players);

			memcpy(frame_data[0]->left_data, h_frame_BGR.data, width * height * 3 * sizeof(uint8_t));
		}
		// Using immuted original Frame Type
		else {
			printf("[Debug] Immuted output Frame Type\n");

			for (int j = 0; j < flowObjects.size(); j++) {
				Point2f p(flowObjects[j].x, flowObjects[j].y);
				rectangle(frame0_L, Point2f(p.x - 4, p.y - 4), Point2f(p.x + 4, p.y + 4), Scalar(0, 0, 255), 1);
				line(frame0_L, p, Point2f(p.x-flowObjects[j].displacement_x*10, p.y-flowObjects[j].displacement_y*10), Scalar(0, 200, 200));
			}

			for (int k = 0; k < t.size(); k++) {
				FlowObject tmp = t[k]->relatedStates[t[k]->relatedStates.size()-1]->state;
				circle(frame0_L, Point2f(tmp.x, tmp.y), 8, (0, 0, 255), 2);
				line(frame0_L, Point2f(tmp.x, tmp.y), Point2f(tmp.x-tmp.displacement_x*3, tmp.y-tmp.displacement_y*3), Scalar(0,255,0), 2);
			}

			statefulObjectFilter->tick();

			drawRectAroundPlayers(frame0_L, players);

			memcpy(frame_data[0]->left_data, frame0_L.data, width * height * channels * sizeof(uint8_t));
		}

		// Enqueue output frame data
		array_spinlock_queue_push(&outputFramesQueue, (void *)frame_data[0]);

		// Left-shift frame data in the local buffer, create space for new data
		frame_data[0] = frame_data[1];
	}

	return NULL;
}

//
// Frames output process
//
void *frames_outpt(void *)
{
	int counter = 0;

	StereoRecorder *stereoRecorder = new StereoRecorder();
	MonoRecorder *monoRecorder = new MonoRecorder();

	int width = Configuration::getInstance()->getFrameInfo().width;
	int height = Configuration::getInstance()->getFrameInfo().height;
	int channels = Configuration::getInstance()->getFrameInfo().channels;
	bool force_rgb_output = (Configuration::getInstance()->getFrameInfo().outputType == ForcedRGB);

	if (configuration->getOperationalMode().processingMode == Record)
	{

		// Start recording
		stereoRecorder->startFramesRecording("/tmp/record");

		while (1) {
			char buffer[300];
			FrameData *frame_data;

			if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
				continue;
			}

			Mat frameL, frameR;

			if (channels == 1 && !force_rgb_output) {
				frameL = Mat(Size(width, height), CV_8UC1, frame_data->left_data);
				frameR = Mat(Size(width, height), CV_8UC1, frame_data->right_data);
			}
			else if (channels == 3 || force_rgb_output) {
				frameL = Mat(Size(width, height), CV_8UC3, frame_data->left_data);
				frameR = Mat(Size(width, height), CV_8UC3, frame_data->right_data);
			}

			stereoRecorder->record(frameL, frameR);

			// Free fast memory
			fast_mem_pool_release_memory(frame_data);
		}
	}
	else if (configuration->getOperationalMode().processingMode == Tracking) {
		// Start recording
		if (configuration->getOperationalMode().inputDevice == MonoCameraVirtual) {
			monoRecorder->startFramesRecording("/tmp/out");

			printf("Frames Output :: Mono Processing Mode: Tracking\n");

			while (1) {
				char buffer[300];
				FrameData *frame_data;

				if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
					continue;
				}

				Mat frame;

				if (channels == 1 && !force_rgb_output) {
					frame = Mat(Size(width, height), CV_8UC1, frame_data->left_data);
				}
				else if (channels == 3 || force_rgb_output) {
					printf("[Debug] Mono recording RGB data\n");
					frame = Mat(Size(width, height), CV_8UC3, frame_data->left_data);
				}

				monoRecorder->record(frame);

				// Free fast memory
				fast_mem_pool_release_memory(frame_data);
			}
		}
		else {
			stereoRecorder->startFramesRecording("/tmp/out");

			printf("Frames Output :: Processing Mode: Tracking\n");

			while (1) {
				char buffer[300];
				FrameData *frame_data;

				if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
					continue;
				}

				Mat frameL, frameR;

				if (channels == 1 && !force_rgb_output) {
					frameL = Mat(Size(width, height), CV_8UC1, frame_data->left_data);
					frameR = Mat(Size(width, height), CV_8UC1, frame_data->right_data);
				}
				else if (channels == 3 || force_rgb_output) {
					frameL = Mat(Size(width, height), CV_8UC3, frame_data->left_data);
					frameR = Mat(Size(width, height), CV_8UC3, frame_data->right_data);
				}

				stereoRecorder->record(frameL, frameR);

				// Free fast memory
				fast_mem_pool_release_memory(frame_data);
			}
		}
	}

	delete stereoRecorder;

	return NULL;
}

//
// Calibration based on Frames acquisition from a real Stereo Vision Device
//
void calibrate_stereo_device()
{
}

//
// Calibration based on Frames acquisition from a Virtual Device
//
void calibrate_virtual_device(double minLineLength, double maxLineGap)
{
	static int counter = 0;

	struct timespec s, t;
	VirtualMonoCamera *virtualMonoCamera = new VirtualMonoCamera();
	virtualMonoCamera->OpenFromFrameSequence("calib", "calibrationFrame%04d.jpeg");

	Mat calibrationFrame = virtualMonoCamera->readFrameFromFrameSequence();

	if (calibrationFrame.empty()) {
		printf("Error: invalid calibration frame\n");
		return;
	}

	TennisFieldCalibrator *calibrator = new TennisFieldCalibrator();
	calibrator->regulateVisualCalibrationWindow(calibrationFrame.clone());
	calibrator->setCalibrationWindow(Point(calibrationFrame.cols/2, calibrationFrame.rows/2), 540, 400, 800, 30);
	calibrator->getCUDALinesDetector()->setCUDADetectorParameters(minLineLength, maxLineGap, 4096, 1);

	printf("CALIB :: calibrator ready\n");

	printf("CALIB :: starting calibration...\n");

	nanotimer_rt_start(&s);
	TennisFieldDelimiter *tennisFieldDelimiter = calibrator->calibrate(calibrationFrame.clone());
	nanotimer_rt_stop(&t);

	printf("CALIB :: performed in %g ms\n", nanotimer_rt_ms_diff(&s, &t));

	line(calibrationFrame, tennisFieldDelimiter->bottomLeft, tennisFieldDelimiter->bottomRight, Scalar(0, 230, 255), 4);
	line(calibrationFrame, tennisFieldDelimiter->bottomRight, tennisFieldDelimiter->topRight, Scalar(0, 230, 255), 4);
	line(calibrationFrame, tennisFieldDelimiter->topRight, tennisFieldDelimiter->topLeft, Scalar(0, 230, 255), 4);
	line(calibrationFrame, tennisFieldDelimiter->topLeft, tennisFieldDelimiter->bottomLeft, Scalar(0, 230, 255), 4);

	char buffer[300];
	sprintf(buffer, "PARAM: minLineLength=%g maxLineGap=%g", minLineLength, maxLineGap);
	putText(calibrationFrame, buffer, cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1.0, cvScalar(50, 205, 50), 1, CV_AA);
	sprintf(buffer, "/tmp/calibrationFrameOutput%04d.png", counter);
	imwrite(buffer, calibrationFrame);

	counter++;
}

//
// Preliminary Calibration procedure
//
void calibrate()
{
	int selection = 0;

	printf("------------------------------------------------------\n");
	printf("System calibration\n");
	printf("------------------------------------------------------\n\n");
	printf("0\tCalibrate using attached stereo camera device\n");
	printf("1\tCalibrate using virtual mono camera device\n");
	printf(">> ");

	scanf("%d", &selection);

	switch (selection) {
	case 0:
		printf("Launching system calibration using stereo camera device...\n");
		calibrate_stereo_device(); // TODO: importa da standalone
		break;
	case 1:
		printf("Launching system calibration using virtual mono camera device...\n");
		calibrate_virtual_device(minLineLength, maxLineGap);
		break;
	}
}

//
// Stereo Depth Sensing Application starter
//
void startStereoApplication(StereoSensorAbstractionLayer *stereoSAL, Configuration *config)
{
	// Start frame processor thread
	pthread_create(&frame_processor_thread, 0, frames_processor, 0);
	printf("Stereo Application :: Frame processor thread running\n");

	// Start frame output thread
	pthread_create(&frame_output_thread, 0, frames_outpt, 0);
	printf("Stereo Application :: Frame output thread running\n");

	struct timespec s;
	struct timespec t;
	double rt_elapsed;

	// Set the queue to use
	SpinlockQueue *queue;

	switch (config->getOperationalMode().processingMode)
	{
	case Tracking:
		printf("Stereo Application :: Operating Mode: Tracking\n");
		queue = &inputFramesQueue;
		break;
	case Record:
		printf("Stereo Application :: Operating Mode: Recording\n");
		queue = &outputFramesQueue;
		break;
	default:
		return;
	}

	if (config->getOperationalMode().inputDevice == StereoCameraDUO) {

		stereoSAL->openCamera();

		printf("Stereo Application :: Input Device: Stereo camera (DUO)\n");

		while (1) {
			nanotimer_rt_start(&s);

			StereoFrameSize frameSize = stereoSAL->getStereoFrameSize();
			StereoFrame stereoFrame = stereoSAL->fetchStereoFrame();

			// Allocate fast memory
			FrameData *frameData = fast_mem_pool_fetch_memory();

			if (frameData == NULL) continue;

			memcpy(frameData->left_data, stereoFrame.leftData, stereoFrame.bytesLength);
			memcpy(frameData->right_data, stereoFrame.rightData, stereoFrame.bytesLength);

			// Enqueue stereo pair data in processing / output queue
			array_spinlock_queue_push(queue, (void *)frameData);

			nanotimer_rt_stop(&t);

			rt_elapsed = nanotimer_rt_ms_diff(&s, &t);
		}
	}
	else if (config->getOperationalMode().inputDevice == StereoCameraVirtual)
	{
		VirtualStereoCamera *virtualStereoCamera = new VirtualStereoCamera();
		virtualStereoCamera->OpenFromFrameSequence("record", "%04d.png");

		printf("Stereo Application :: Input Device: Virtual stereo camera\n");

		while (1) {
			nanotimer_rt_start(&s);

			FramePair framePair = virtualStereoCamera->readFramePairFromFrameSequence();
			FrameData *frameData = fast_mem_pool_fetch_memory();

			if (frameData == NULL) continue;

			memcpy(frameData->left_data, framePair.L.data, framePair.L.rows * framePair.L.cols * sizeof(uint8_t));
			memcpy(frameData->right_data, framePair.R.data, framePair.R.rows * framePair.R.cols * sizeof(uint8_t));

			// Enqueue stereo pair data in processing / output queue
			array_spinlock_queue_push(queue, (void *)frameData);

			nanotimer_rt_stop(&t);

			rt_elapsed = nanotimer_rt_ms_diff(&s, &t);
		}
	}
	else if (config->getOperationalMode().inputDevice == MonoCameraVirtual)
	{
		VirtualMonoCamera *virtualMonoCamera = new VirtualMonoCamera();
		virtualMonoCamera->OpenFromFrameSequence("record", "VIDEO-%d.jpg");

		printf("Mono Application :: Input Device: Virtual mono camera\n");
		int mem_bytes = sizeof(uint8_t) * Configuration::getInstance()->getFrameInfo().width * Configuration::getInstance()->getFrameInfo().height * Configuration::getInstance()->getFrameInfo().channels;

		while (1) {
			nanotimer_rt_start(&s);

			Mat frame = virtualMonoCamera->readFrameFromFrameSequence();
			FrameData *frameData = fast_mem_pool_fetch_memory();

			if (frame.empty()) {
				break;
			}
			if (frameData == NULL) continue;

			memcpy(frameData->left_data, frame.data, mem_bytes);

			// Enqueue stereo pair data in processing / output queue
			array_spinlock_queue_push(queue, (void *)frameData);

			nanotimer_rt_stop(&t);

			rt_elapsed = nanotimer_rt_ms_diff(&s, &t);
		}
	}

	pthread_join(frame_processor_thread, 0);
}

//
// Infrared Depth Sensing Application starter
//
void startIrApplication(IRSensorAbstractionLayer *irSAL, Configuration *config)
{
	// TODO: importare, on hold
}

//
// Application Entry Point
//
int main(int argc, const char* argv[])
{
	StereoSensorAbstractionLayer *stereoSAL = 0;
	IRSensorAbstractionLayer *irSAL = 0;
	DepthSensorTechnology depthTech;

	configuration = Configuration::getInstance();

	/*
	configuration->setOperationalMode(VirtualStereoCamera, Tracking);
	OpticalLayerParameters opticalLayerParam;
	opticalLayerParam.linearLowPassFilterX = 0.93;
	opticalLayerParam.linearLowPassFilterY = 0.99;
	configuration->setOpticalLayerParameters(opticalLayerParam);
	configuration->writeConfigFile("config.xml");
	*/

	if (argc > 2) {
		if (!strcmp(argv[1], "-c") || !strcmp(argv[1], "--config")) {
			printf("Loading configuration file %s...\n", argv[2]);
			configuration->loadConfigFile(argv[2]);
		}
	}

	configuration->display();

	// Initialize fast memory pool
	printf("Initializing fast memory pool...\n");
	if (configuration->getFrameInfo().outputType == ForcedRGB) {
		fast_mem_pool_init(configuration->getFrameInfo().width, configuration->getFrameInfo().height, 3);
	}
	else {
		fast_mem_pool_init(configuration->getFrameInfo().width, configuration->getFrameInfo().height, configuration->getFrameInfo().channels);
	}
	printf("Fast memory pool initialized.\n");

	// Initialize array-based spinlock queues
	printf("Initializing I/O Frame queues...\n");
	array_spinlock_queue_init(&inputFramesQueue);
	array_spinlock_queue_init(&outputFramesQueue);
	printf("I/O Frame queues initialized.\n");

	// Operating Mode check
	switch(configuration->getOperationalMode().inputDevice)
	{
	case StereoCameraDUO:
		stereoSAL = new DUOStereoSensorDriver(configuration->getDUOStereoCameraHardwareParameters());
		depthTech = Stereo;
		break;
	case StereoCameraZED:
		stereoSAL = new ZEDStereoSensorDriver();
		depthTech = Stereo;
		break;
	case IrCameraKinect:
		irSAL = new KinectIRSensorDriver();
		depthTech = IR;
		break;
	}

	switch (depthTech)
	{
	case Stereo:
		startStereoApplication(stereoSAL, configuration);
		break;
	case IR:
		startIrApplication(irSAL, configuration);
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
// Farneback Dense Optical Flow procedures
//
////////////////////////////////////////////////////////////////////////////

// Optical Flow thread routine
void *optical_flow(void *args)
{
	ParallelOpticalFlowData *pOpticalFlowData = (ParallelOpticalFlowData *)args;

	gpu::FarnebackOpticalFlow farn;
	farn.winSize = 15;

	// Left camera optical flow
	if (pOpticalFlowData->camera_flag == 0) {
		farn(*(pOpticalFlowData->gpu_frame0), *(pOpticalFlowData->gpu_frame1), d_flowx_L, d_flowy_L);
	}

	// Right camera optical flow
	else if (pOpticalFlowData->camera_flag == 1) {
		farn(*(pOpticalFlowData->gpu_frame0), *(pOpticalFlowData->gpu_frame1), d_flowx_R, d_flowy_R);
	}

	return NULL;
}

// Compute Optical Flow in parallel for Left and Right data
void computeParallelOpticalFlow(Mat frame0_L, Mat frame1_L, Mat frame0_R, Mat frame1_R)
{
	// Upload Left data to GPU device
	GpuMat d_L0(frame0_L);
	GpuMat d_L1(frame1_L);

	// Upload Right data to GPU device
	GpuMat d_R0(frame0_R);
	GpuMat d_R1(frame1_R);

	ParallelOpticalFlowData *pOpticalFlowData_L = new ParallelOpticalFlowData();
	ParallelOpticalFlowData *pOpticalFlowData_R = new ParallelOpticalFlowData();

	pOpticalFlowData_L->camera_flag = 0;
	pOpticalFlowData_L->gpu_frame0 = &d_L0;
	pOpticalFlowData_L->gpu_frame1 = &d_L1;

	pOpticalFlowData_R->camera_flag = 1;
	pOpticalFlowData_R->gpu_frame0 = &d_R0;
	pOpticalFlowData_R->gpu_frame1 = &d_R1;

	pthread_t t0, t1;
	pthread_create(&t0, 0, optical_flow, (void *)pOpticalFlowData_L);
	pthread_create(&t1, 0, optical_flow, (void *)pOpticalFlowData_R);

	pthread_join(t0, 0);
	pthread_join(t1, 0);

	delete pOpticalFlowData_L;
	delete pOpticalFlowData_R;
}

//
// Fetch precise object positions from Farneback Optical Flow
//
TennisField_3D field1, field2;
vector<MovingObject> fetchLatestPreciseObjectPosition(Mat frame, uint16_t frameTime, const Mat_<float>& flowx, const Mat_<float>& flowy, Mat& dst, float maxmotion = -1, char cameraFlag = 'X')
{
	vector<MovingObject> rawTrajectoryHeads;

	Mat frameCopy;
	frame.copyTo(frameCopy);

	/*
	// Render Tennis field on reference frame
	if (cameraFlag == 'L') {
		tennisFieldReprojectRender(frame);
	}
	*/

	vector<OpticalFlowRegion *> flowRegions = CPU_recognizeOpticalFlowRegions(flowx, flowy);
	printf("=== %d === [Optical Layer] Recognized %d flow regions\n", frameTime, flowRegions.size());

	vector<Trajectory *> trajectories;

	for (int s = 0; s < flowRegions.size(); s++) {
		recognizeTrueRegionCenter(flowRegions[s], frameCopy);

		if (flowRegions[s]->trueCenterFound) {
			circle(frame, Point(flowRegions[s]->trueCenterX, flowRegions[s]->trueCenterY), 4, Scalar(0, 0, 255), -1);
			circle(frame, Point(flowRegions[s]->trueCenterX, flowRegions[s]->trueCenterY), 12, Scalar(0, 0, 255), 1);
		}

	}

	// Render regions: DEBUG DISABLED
	/*
	for (int i = 0; i < flowRegions.size(); i++) {
		rectangle(frame, Point(flowRegions[i]->x0, flowRegions[i]->y0), Point(flowRegions[i]->x1, flowRegions[i]->y1), Scalar(0, 0, 255));
	}
	*/

	if (cameraFlag == 'L')
		trajectories = trajectoryRecognizer_L->CPU_TrajectoryRecognizerFeed(frameTime, flowRegions);
	else if (cameraFlag == 'R')
		trajectories = trajectoryRecognizer_R->CPU_TrajectoryRecognizerFeed(frameTime, flowRegions);

	printf("[%c] [TRAJECT] %d trajectories active\n", cameraFlag, trajectories.size());

	OverlayRenderer::getInstance()->renderModelInformation(frame, trajectories, cameraFlag);

	for (int i = 0; i < trajectories.size(); i++) {
		Trajectory *t = trajectories[i];
		if (t->motionFlow.size() <= 2) continue;

		// Get last raw object (unfiltered position)
		MovingObject lastUnfilteredObject = t->lastRawObject;

		cout << "RawObject: Trajectory Label: " << t->trajectoryID << "     Object Label: " << lastUnfilteredObject.trajectoryID << endl;

		rawTrajectoryHeads.push_back(lastUnfilteredObject);

		int interpLevel = 8;
		int B = 50, G = 205, R = 50;

		for (int j = 0; j < t->motionFlow.size()-1; j++) {
			MovingObject *obj = t->motionFlow[j];
			//line(dst, Point(t->motionFlow[j]->centerOfMotionX, t->motionFlow[j]->centerOfMotionY), Point(t->motionFlow[j+1]->centerOfMotionX, t->motionFlow[j+1]->centerOfMotionY), Scalar(0, 0, 255));
			double Vx = (t->motionFlow[j+1]->centerOfMotionX - obj->centerOfMotionX); // Vx = dx / dt, dt=1 here
			double Vy = (t->motionFlow[j+1]->centerOfMotionY - obj->centerOfMotionY); // Vy = dy / dt, dt=1 here
			for (int k = 0; k <= interpLevel; k++) {
				int Cx = (double)t->motionFlow[j]->centerOfMotionX + ((double)(Vx * k) / (double)interpLevel);
				int Cy = (double)t->motionFlow[j]->centerOfMotionY + ((double)(Vy * k) / (double)interpLevel);
				circle(frame, Point(Cx, Cy), 3.0, Scalar(B, G, R));
				if (obj->impacted && (k == 0)) {
					circle(frame, Point(Cx, Cy), 4.0, Scalar(0, 255, 255), 2);
					if (cameraFlag == 'L') {
						TennisFieldStaticModel::checkTennisField_2D(&field1, (double)Cx, (double)Cy);
						TennisFieldStaticModel::checkTennisField_2D(&field2, (double)Cx, (double)Cy);

					}
				}
				//MovingObject's center of motion: circle(dst, Point(Cx, Cy), 1.0, Scalar(0, 0, 255), 2.0);
				B += 5;
			}

			if (R > 20) R -= 5;
			if (G > 20) G -= 5;
		}

		// Get next predicted position, Look Ahead = 2
		Scalar predictionColor(0, 255, 255);
		for (int i = 1; i < 12; i++) {
			PredictedPosition p;

			if (cameraFlag == 'L')
				p = trajectoryRecognizer_L->CPU_TrajectoryRecognizerPredict(t, i);
			else if (cameraFlag == 'R')
				p = trajectoryRecognizer_R->CPU_TrajectoryRecognizerPredict(t, i);

			if (i == 1) t->predictedPosition = p;

			//printf("[Simulated predicted] lastObject = [%f, %f],    predictedPosition = [%f, %f], delta t = %d\n", t->motionFlow[t->motionFlow.size()-1]->centerOfMotionX, t->motionFlow[t->motionFlow.size()-1]->centerOfMotionY, p.Cx, p.Cy, i);

			if (p.Cx >= frame.cols || p.Cy >= frame.rows || p.Cx <= 0 || p.Cy <= 0) continue;

			Point S(t->motionFlow[t->motionFlow.size()-1]->centerOfMotionX, t->motionFlow[t->motionFlow.size()-1]->centerOfMotionY);
			double dist = sqrt((p.Cx - S.x)*(p.Cx - S.x) + (p.Cy - S.y)*(p.Cy -S.y));

			// Versor identifying the line to the predicted position
			double versorX = (p.Cx - S.x) / dist;
			double versorY = (p.Cy - S.y) / dist;

			// Normal versor to the predicted direction
			double normalX = -versorY;
			double normalY = versorX;

			double r = p.r / 2.0;

			circle(frame, Point(p.Cx, p.Cy), 1, predictionColor, 2);
			predictionColor[1] -= 40;
		}
	}

	return rawTrajectoryHeads;
}

//
// Process parallel bilateral Farneback Optical Flow
//
static void processFlow(Mat frame_L, Mat frame_R, uint16_t frameTime, const char* fileName_L, char *fileName_R,
		const GpuMat& d_flowx_L, const GpuMat& d_flowy_L, const GpuMat& d_flowx_R, const GpuMat& d_flowy_R)
{
	Mat flowx_L(d_flowx_L);
	Mat flowy_L(d_flowy_L);

	Mat flowx_R(d_flowx_R);
	Mat flowy_R(d_flowy_R);

	Mat out_L, out_R;
	Mat frame_RGB_L, frame_RGB_R;

	cvtColor(frame_L, frame_RGB_L, CV_GRAY2RGB);
	cvtColor(frame_R, frame_RGB_R, CV_GRAY2RGB);

	// WARNING: Inefficient for integrated infromation rendering
	// TODO: decouple rendering from precise object position fetching
	// TODO: parallelize the two processes with pthread
	vector<MovingObject> objsL = fetchLatestPreciseObjectPosition(frame_RGB_L, frameTime, flowx_L, flowy_L, out_L, 10, 'L');
	vector<MovingObject> objsR = fetchLatestPreciseObjectPosition(frame_RGB_R, frameTime, flowx_R, flowy_R, out_R, 10, 'R');

	// 3D Dynamic Model fitting
	// Happens if not object is occluded, else data might be inconsistent
	if (objsL.size() == objsR.size()) {

		for (int i = 0; i < objsL.size(); i++) {

#ifdef CFG_3D_MODEL_FITTING_USE_TRUEOBJECT_ONLY
			// Model fitting happens only if the true object has been identified
			if (!objsL[i].trueObject || !objsR[i].trueObject) {
				continue;
			}
#endif

			// Find out if last known trajectory object has impacted
			Trajectory *tmp = trajectoryRecognizer_L->getTrajectoryByID(objsL[i].trajectoryID);
			if (tmp->motionFlow[tmp->motionFlow.size()-2]->impacted) {
				DynamicModel3D_SetLastObjectImpactState(tmp->trajectoryID, true);
			}

			// Centers of the object detected in left and right frames
			Point p0(objsL[i].centerOfMotionX, objsL[i].centerOfMotionY);
			Point p1(objsR[i].centerOfMotionX, objsR[i].centerOfMotionY);

			// Disparity and depth map buffers
			Mat1f disparity = Mat(Size(WIDTH, HEIGHT), CV_32FC1);
			Mat3f depth3d = Mat(Size(WIDTH, HEIGHT), CV_32FC3);

			// Temporary: use DUO3D GPU-accelerated depth map computation
			// TODO: Activate epipolar single-line matching in Release
			Dense3DGetDepth(dense3d, (uint8_t *)frame_L.data, (uint8_t *)frame_R.data, (float*)disparity.data, (PDense3DDepth)depth3d.data);

			// EXPERIMENTAL: Montecarlo in a small neighborhood of the center
			// OBJECTIVE: 3D coordinates averaging/denoising
			Vector3D sumVector;
			Vector3D v;

			srand(time(0));

			int montecarlo_effective_iterat = 0;
			for (int e = 0; e < CFG_3D_MODEL_OBJECT_CENTER_MONTECARLO_ITERAT; e++) {
				int rx = (p0.x - 5) + (rand() % (10 + 1));
				int ry = (p0.y - 5) + (rand() % (10 + 1));

				Vec3f tmp = depth3d.at<Vec3f>(Point(rx, ry));

				v.x = tmp.val[0];
				v.y = tmp.val[1];
				v.z = tmp.val[2];

				// Discard upon invalid data
				if (isinf(v.x) || isinf(v.y) || isinf(v.z) || isnan(v.x) || isnan(v.y) || isnan(v.z) || v.z >= 10000.0) {
					continue;
				}

				sumVector.x += (double)v.x;
				sumVector.y += (double)v.y;
				sumVector.z += (double)v.z;

				montecarlo_effective_iterat++;
			}

			if (montecarlo_effective_iterat > 0) {
				v.x = sumVector.x / (double)montecarlo_effective_iterat;
				v.y = sumVector.y / (double)montecarlo_effective_iterat;
				v.z = sumVector.z / (double)montecarlo_effective_iterat;
			}
			else {
				Vec3f tmp = depth3d.at<Vec3f>(Point(p0.x, p0.y));

				v.x = tmp.val[0];
				v.y = tmp.val[1];
				v.z = tmp.val[2];

				if (isinf(v.x) || isinf(v.y) || isinf(v.z) || isnan(v.x) || isnan(v.y) || isnan(v.z) || v.z >= 10000.0) {
					continue;
				}
			}

			// Feed 3D Dynamic Model with new data
			// Data is associated with a trajectory by the TrajectoryID
			DynamicModel3D_RecalcTrajectoryMechanicalState(v, objsL[i].impacted, frameTime, objsL[i].trajectoryID);

			// If desired, predict future 3D trajectory for a parameterized lookahead time
			vector<Vector3D> futurePoints = DynamicModel3D_PredictMechanicalState(objsL[i].trajectoryID, 10);


			// Render Future Points [DISABLED]
			/*
			if (futurePoints.size() > 0) {
				vector<Point2f> futureImagePoints = project3DCoordToLeftImagePlane(futurePoints);

				for (int w = 0; w < futureImagePoints.size(); w++) {
					Point2f p = futureImagePoints[w];
					circle(frame_RGB_L, p, 2.0, Scalar(255, 80, 80), 2);
				}
			}
			*/
		}
	}

	// If desired, enqueue the frame in the output queue
	//system_output_enqueue_frame(frame_RGB_L);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// LEGACY: codice vecchio lasciato qui temporaneamente come riferimento: ignora.
//
////////////////////////////////////////////////////////////////////////////////////////

//
// To use Farneback parallelized Optical Flow:
//
/*
computeParallelOpticalFlow(frame0_L, frame1_L, frame0_R, frame1_R);

// Download flow from device memory
Mat flowx_L(d_flowx_L);
Mat flowy_L(d_flowy_L);
Mat flowx_R(d_flowx_L);
Mat flowy_R(d_flowy_L);

vector<FlowObject> flowObjects = FlowProcessor_RecognizeFlowObjects(flowx_L, flowy_L);

for (int i = 0; i < flowObjects.size(); i++) {
	rectangle(frame0_L , flowObjects[i].rect, Scalar(0,255,0), 2, 8, 0);
}
*/

//
// TODO: Legacy. integrare con modulo floor detection
//
/*
TennisField_3D field1;
TennisField_3D field2;

void tennisFieldReprojectRender(Mat frame)
{
	Mat objectPoints1(1, 4, CV_32FC3);
		Mat objectPoints2(1, 4, CV_32FC3);
		Mat netPoints3D(1, 4, CV_32FC3);

		// Height displacement in camera far
		double h_displacement = 200;

		// Field1
		field1.ground_h = 600;

		field1.near_L_x = -400;
		field1.near_L_z = 1000;

		field1.near_R_x = 200;
		field1.near_R_z = 1000;

		field1.far_R_x = 200;
		field1.far_R_z = 4000;

		field1.far_L_x = -400;
		field1.far_L_z = 4000;

		// Field 2
		field2.ground_h = 600;

		field2.near_L_x = 200;
		field2.near_L_z = 1000;

		field2.near_R_x = 800;
		field2.near_R_z = 1000;

		field2.far_R_x = 800;
		field2.far_R_z = 4000;

		field2.far_L_x = 200;
		field2.far_L_z = 4000;

		// Ground coordinates
		objectPoints1.at<Vec3f>(0, 0) = Point3f(field1.near_L_x, field1.ground_h, field1.near_L_z);
		objectPoints1.at<Vec3f>(0, 1) = Point3f(field1.near_R_x, field1.ground_h, field1.near_R_z);
		objectPoints1.at<Vec3f>(0, 2) = Point3f(field1.far_R_x, field1.ground_h+h_displacement, field1.far_R_z);
		objectPoints1.at<Vec3f>(0, 3) = Point3f(field1.far_L_x, field1.ground_h+h_displacement, field1.far_L_z);

		objectPoints2.at<Vec3f>(0, 0) = Point3f(field2.near_L_x, field2.ground_h, field2.near_L_z);
		objectPoints2.at<Vec3f>(0, 1) = Point3f(field2.near_R_x, field2.ground_h, field2.near_R_z);
		objectPoints2.at<Vec3f>(0, 2) = Point3f(field2.far_R_x, field2.ground_h+h_displacement, field2.far_R_z);
		objectPoints2.at<Vec3f>(0, 3) = Point3f(field2.far_L_x, field2.ground_h+h_displacement, field2.far_L_z);

		// Net coordinates
		netPoints3D.at<Vec3f>(0, 0) = Point3f(field1.near_R_x, field1.ground_h, field1.near_R_z);
		netPoints3D.at<Vec3f>(0, 1) = Point3f(field1.near_R_x, field1.ground_h-500, field1.near_R_z);
		netPoints3D.at<Vec3f>(0, 2) = Point3f(field1.far_R_x, field1.ground_h+h_displacement-500, field1.far_R_z);
		netPoints3D.at<Vec3f>(0, 3) = Point3f(field1.far_R_x, field1.ground_h+h_displacement, field1.far_R_z);

		Mat visualPoints3D1(1, 15 * 11, CV_32FC3);
		Mat visualPoints3D2(1, 15 * 11, CV_32FC3);
		Mat netVisualPoints3D(1, 10 * 11, CV_32FC3);

		TennisFieldStaticModel::computeTennisFieldVisualPoints(objectPoints1, visualPoints3D1, h_displacement);
		TennisFieldStaticModel::computeTennisFieldVisualPoints(objectPoints2, visualPoints3D2, h_displacement);
		TennisFieldStaticModel::computeTennisNetVisualPoints(netPoints3D, netVisualPoints3D, h_displacement);

		GpuMat d_objectPoints1(objectPoints1);
		GpuMat d_objectPoints2(objectPoints2);
		GpuMat d_imagePoints1;
		GpuMat d_imagePoints2;
		GpuMat d_netPointsImage;

		GpuMat d_visualPoints3D1(visualPoints3D1);
		GpuMat d_visualPoints3D2(visualPoints3D2);
		GpuMat d_netVisualPoints3D(netVisualPoints3D);
		GpuMat d_visualPointsImage1;
		GpuMat d_visualPointsImage2;
		GpuMat d_netVisualPointsImage;
		GpuMat d_netPoints3D(netPoints3D);

		gpu::projectPoints(d_objectPoints1, rvecL, tvecL, cameraMatrixL, Mat(), d_imagePoints1);
		gpu::projectPoints(d_visualPoints3D1, rvecL, tvecL, cameraMatrixL, Mat(), d_visualPointsImage1);

		gpu::projectPoints(d_objectPoints2, rvecL, tvecL, cameraMatrixL, Mat(), d_imagePoints2);
		gpu::projectPoints(d_visualPoints3D2, rvecL, tvecL, cameraMatrixL, Mat(), d_visualPointsImage2);
		gpu::projectPoints(d_netVisualPoints3D, rvecL, tvecL, cameraMatrixL, Mat(), d_netVisualPointsImage);

		gpu::projectPoints(d_netPoints3D, rvecL, tvecL, cameraMatrixL, Mat(), d_netPointsImage);

		Mat imagePoints1(d_imagePoints1);
		Mat h_visualPointsImage1(d_visualPointsImage1);

		Mat imagePoints2(d_imagePoints2);
		Mat h_visualPointsImage2(d_visualPointsImage2);

		Mat h_netVisualPointsImage(d_netVisualPointsImage);

		Mat h_netPointsImage(d_netPointsImage);

		// Overlay rendering of Tennis Field elements
		OverlayRenderer::getInstance()->renderTennisField(&field1, frame, imagePoints1, h_visualPointsImage1);
		OverlayRenderer::getInstance()->renderTennisField(&field2, frame, imagePoints2, h_visualPointsImage2);
		OverlayRenderer::getInstance()->renderTennisNet(frame, h_netPointsImage, h_netVisualPointsImage);
}
*/


/*
 * OLD MAIN Using Farneback optical flow
 *

field1.score = 0;
field2.score = 0;

char *endp;

if (argc > 9) {
	if (!strcmp(argv[7], "--calib")) {
		char *endp;
		minLineLength = strtod(argv[8], &endp);
		maxLineGap = strtod(argv[9], &endp);

		printf("[CALIB] Loaded parameters: minLineLength=%g minLineGap=%g\n", minLineLength, minLineLength);
	}
}

// Calibration
calibrate();

ax = strtod(argv[4], &endp);		// Linear Low-Pass filter x
ay = strtod(argv[5], &endp);		// Linear Low-Pass filter y
filter_iterat = atoi(argv[6]);		// Trajectory smoothing iterations

// Set high-performance
printf("[WARNING] Did you set the maximal GPU and GPU performance?\n\n");
system("/bin/sh /home/ubuntu/init.sh");
printf("------------------------------------------\n");
printf("Gain:\t\t\t%d\n", atoi(argv[1]));
printf("Exposure:\t\t%d\n", atoi(argv[2]));
printf("IR LEDs:\t\t%d\n\n", atoi(argv[3]));
printf("Low-Pass filter x:\t%g\n", ax);
printf("Low-Pass filter y:\t%g\n", ay);
printf("Trajectory filter iter:\t%d\n", filter_iterat);
printf("------------------------------------------\n");


trajectoryRecognizer_L = new TrajectoryRecognizer();
trajectoryRecognizer_R = new TrajectoryRecognizer();

Dense3DOpen(&dense3d);
SetDense3DLicense(dense3d, "KOXAN-9DO6T-25K96-Z7O7U-K1X1O");
SetDense3DImageSize(dense3d, 320, 240);

// Get DUO stereo camera parameters
INTRINSICS intr;
EXTRINSICS extr;
GetCameraParameters(&intr, &extr);

// Set Dense3D parameters
SetDense3DMode(dense3d, 0);
SetDense3DCalibration(dense3d, &intr, &extr);
SetDense3DNumDisparities(dense3d, 2); // original: 2
SetDense3DSADWindowSize(dense3d, 10);   // original: 6
SetDense3DPreFilterCap(dense3d, 63);
SetDense3DUniquenessRatio(dense3d, 0);
SetDense3DSpeckleWindowSize(dense3d, 256);
SetDense3DSpeckleRange(dense3d, 32);

// Initialize Dynamic Model 3D with camera parameters
DynamicModel3D_Init(320, 240, 198.479953, 200.207839, -30.262322);

cpu_set_t cpuset;

printf("Assigning threads to CPU cores...\n");
// Set CPU cores
CPU_ZERO(&cpuset);
CPU_SET(0, &cpuset);
//pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

CPU_ZERO(&cpuset);
CPU_SET(1, &cpuset);
//pthread_setaffinity_np(frame_processor_thread, sizeof(cpu_set_t), &cpuset);

CPU_ZERO(&cpuset);
CPU_SET(2, &cpuset);
//pthread_setaffinity_np(frame_output_thread, sizeof(cpu_set_t), &cpuset);


return 0;

for (int i = 0; i < 800; i++) {


	// Fetch two consecutive frames from the DUO camera
	PDUOFrame pFrameData0 = GetDUOFrame();
	PDUOFrame pFrameData1 = GetDUOFrame();

	if(pFrameData0 == NULL || pFrameData1 == NULL) {
		continue;
	}



	// Left data
	Mat frame0_L = Mat(Size(pFrameData0->width, pFrameData0->height), CV_8UC1, pFrameData0->leftData).clone();
	Mat frame1_L = Mat(Size(pFrameData1->width, pFrameData1->height), CV_8UC1, pFrameData1->leftData).clone();

	// Right data
	Mat frame0_R = Mat(Size(pFrameData0->width, pFrameData0->height), CV_8UC1, pFrameData0->rightData).clone();
	Mat frame1_R = Mat(Size(pFrameData1->width, pFrameData1->height), CV_8UC1, pFrameData1->rightData).clone();

	// Compute GPU Farneback optical flow in parallel for stereo-pair at t and (t + 1)
	computeParallelOpticalFlow(frame0_L, frame1_L, frame0_R, frame1_R);

	// Detect players
	players = detectPlayers(frame0_L);

	// Process optical flow information from both cameras
	processFlow(frame1_L, frame1_R, i, NULL, NULL, d_flowx_L, d_flowy_L, d_flowx_R, d_flowy_R);

}
*/

////////////////////////////////////////////////////////////////////////////////////////
//
// END LEGACY
//
////////////////////////////////////////////////////////////////////////////////////////

