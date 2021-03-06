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
#include "../StaticModel/NetModel.h"

#include "../OpticalLayer/pyrlk_optical_flow.h"
#include "../OpticalLayer/FlowProcessor.h"
#include "../OpticalLayer/StatefulObjectFilter.h"
#include "../OpticalLayer/HSVManager.h"

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

#include "../TrajectoryTracking/TrajectoryTracker.h"

#include "../TargetPredator/TargetPredator.h"

#include "../DynamicModel3D/DynamicModel.h"

#include "../PlayLogic/PlayLogicFactory.h"
#include "../PlayLogic/TwoPlayersPlayLogic.h"

#include "../Serialization/serialization.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "/home/ubuntu/opencv-2.4.10/modules/core/include/opencv2/core/opengl_interop.hpp"

using namespace std;
using namespace cv;
using namespace cv::gpu;

volatile bool staticModelReadyReplay;
extern volatile bool systemCalibrated;
extern volatile bool systemRecording;

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

// OpenGL rendering data
struct DrawData
{
    ogl::Arrays arr;
    ogl::Texture2D tex;
    ogl::Buffer indices;
};

struct DrawData data;

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
SpinlockQueue outputFramesQueue;
SpinlockQueue *outputFramesQueueExternPtr;
StereoSensorAbstractionLayer *stereoSALExternPtr;

volatile bool systemReady = false;

// Configuration
Configuration *configuration;

// Thread handles
pthread_t frame_processor_thread;
pthread_t frame_output_thread;

// Thread attributes
pthread_attr_t frame_processor_attr;
pthread_attr_t frame_output_attr;

// Trajectory Tracker
TrajectoryTracker *trajectoryTracker;

// HSV Manager
HSVManager *hsvManager;

// Target Predator
TargetPredator *tgtPredator;

// Static Model
TennisFieldStaticModel *staticModel;

// Dynamic Model
DynamicModel *dynamicModel;

// Play Logic
PlayLogic *playLogic;

void renderScene(void* userdata)
{
    DrawData* data = static_cast<DrawData*>(userdata);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ogl::render(data->arr, data->indices, ogl::TRIANGLES);
}

//
// Frames Processor process
//
void *frames_processor(void *)
{
	int bufp = 0;
	FrameData *frame_data[2];
	StatefulObjectFilter *statefulObjectFilter = new StatefulObjectFilter();
	HSVRange hsvRangeTGT = Configuration::getInstance()->calibrationData.targetHSVRange;
	TennisFieldDelimiter *fieldDelimiter = staticModel->getTennisFieldDelimiter();

	if (configuration->getOperationalMode().processingMode == Record) {
		printf("Stereo Application :: Frames Processor :: Leaving\n");
		return NULL;
	}

	int width = Configuration::getInstance()->getFrameInfo().width;
	int height = Configuration::getInstance()->getFrameInfo().height;
	int channels = Configuration::getInstance()->getFrameInfo().channels;
	bool force_rgb_output = (Configuration::getInstance()->getFrameInfo().outputType == ForcedRGB);

	// U8 buffers
	uint8_t *buf_8UC1_0 = (uint8_t *)malloc(width * height * sizeof(uint8_t));
	Mat f_8UC1_0;
	//uint8_t *buf_8UC1_1 = (uint8_t *)malloc(width * height * sizeof(uint8_t));

	// Net's visual projection on the Reference Camera's Image Plane
	NetVisualProjection netVisualProjection;

	// Field Representation for Two-Players Play mode
	TwoPlayersFieldRepresentation twoPlayersFieldRepresentation;

	// In Replay Mode, the Frames Processor must wait for the Stereo Application thread to complete
	// the load of the binary encoded Static Model, stored in the recording file.
	if (configuration->operationalMode.inputDevice == StereoCameraVirtual) {

		while (!staticModelReadyReplay) {
			printf("Application :: Replay :: Waiting binary Static Model Decode...\n");
			usleep(10000);
		}
	}

	// Fetch the Net's visual projection on the Reference Camera's Image Plane
	netVisualProjection = NetModel::getInstance()->getNetVisualProjection();

	// Generate Field Representation from Static Model in the Play Logic
	// Delegate this unique task on Static Model deserialization complete
	if (configuration->operationalMode.inputDevice != StereoCameraVirtual) {
		printf("Application :: RT Tracking :: Generating Field Representation in Logic\n");
		playLogic->generateFieldRepresentationFromModel();
	}

	if (configuration->playLogicParameters.playLogicType == TwoPlayers)
	{
		printf("Application :: Retrieving Field Representation from Logic\n");
		twoPlayersFieldRepresentation = ((TwoPlayersPlayLogic *)playLogic)->retrieveFieldRepresentation();
	}
	else if (configuration->playLogicParameters.playLogicType == SinglePlayer)
	{
		printf("Application :: PlayLogic :: Single Player\n");
	}
	else {
		printf("Application :: WARNING :: Unknown Play Logic configuration\n");
	}

	// If Recording Mode is active, the Frames Processor runs only when the recording of Dynamic Models data is requested
	// Dynamic Models are fitted, but no Overlay rendering is performed
	if (configuration->operationalMode.processingMode == Record)
	{
		printf("Application :: Frames Processor :: Recording Static Model Data\n");

		while (systemRecording) {
			FrameData *fd;

			// Fetch new frame data from fast concurrent input queue
			if (array_spinlock_queue_pull(&inputFramesQueue, (void **)&fd) < 0) {
				usleep(10);
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
			else if (channels == 4) {
				// Left data
				frame0_L = Mat(Size(width, height), CV_8UC4, frame_data[0]->left_data);
				frame1_L = Mat(Size(width, height), CV_8UC4, frame_data[1]->left_data);

				// Right data
				frame0_R = Mat(Size(width, height), CV_8UC4, frame_data[0]->right_data);
				frame1_R = Mat(Size(width, height), CV_8UC4, frame_data[1]->right_data);
			}

			hsvManager->filterHSVRange_out_8UC1(frame_data[1]->left_data, width, height, hsvRangeTGT, buf_8UC1_0);
			pred_scan_t engage_data = tgtPredator->engage_8UC1(buf_8UC1_0, width, height);

			// Update Predator
			if (engage_data.xl != 0 && engage_data.xr != 0 && engage_data.row != 0) {
				tgtPredator->update_state(engage_data.xl + (engage_data.xr-engage_data.xl)/2, engage_data.row);
				Point targetPosition(engage_data.xl + (engage_data.xr-engage_data.xl)/2, engage_data.row);

				/*
				 *
				 *
				 *
				 * Make Dynamic Model 2D data
				 *
				 *
				 *
				 *
				 */

				if (fd->depth_data_avail) {

					StereoSensorMeasure3D meas;
					meas.z_mm = ZEDStereoSensorDriver::readMeasurementDataDepth(fd->depth_data, targetPosition.x, targetPosition.y, fd->step_depth);
					meas.x_mm = (meas.z_mm * (float)targetPosition.x) / Configuration::getInstance()->zedHardwareParameters.fx_L;
					meas.y_mm = (meas.z_mm * (float)targetPosition.y) / Configuration::getInstance()->zedHardwareParameters.fy_L;

					if (meas.z_mm > 0.0) {

						Vector3D meas_v;
						meas_v.x = (double)meas.x_mm;
						meas_v.y = (double)meas.y_mm;
						meas_v.z = (double)meas.z_mm;

						// Recalc dynamic model
						dyn_model_result_t dynamicModelResult = dynamicModel->recalc(meas_v, fd->t);

						/*
						 *
						 *
						 *
						 * Make Dynamic Model 3D data
						 *
						 *
						 *
						 *
						 */

						if (dynamicModelResult.impact) {
							// Search in the TargetPredator backlog the last impact and mark it as confirmed
							Vector2D opticalLatestImpactData;
							pred_state_t latest_impact = tgtPredator->confirm_latest_impact();
							opticalLatestImpactData.x = (double)latest_impact.x;
							opticalLatestImpactData.y = (double)latest_impact.y;

							/*
							 *
							 *
							 *
							 * Integrate Impact Confirmation Information in Model 3D data
							 *
							 *
							 *
							 *
							 */
						}
					}
				}
			}

			// Buffer shift
			memcpy(frame_data[0]->left_data, frame0_L.data, width * height * channels * sizeof(uint8_t));

			// Enqueue output frame data
			if (array_spinlock_queue_push(&outputFramesQueue, (void *)frame_data[0]) < 0) {
				printf("Stereo Application :: WARNING :: Queue Push failed (@ Frame Counter %d)\n", frame_data[0]->frame_counter);
			}

			// Left-shift frame data in the local buffer, create space for new data
			frame_data[0] = frame_data[1];
		}
	}

	// Tracking Mode: the Frames Processor operates in nominal mode
	else
	{
		while (1) {
			FrameData *fd;

			// Fetch new frame data from fast concurrent input queue
			if (array_spinlock_queue_pull(&inputFramesQueue, (void **)&fd) < 0) {
				usleep(10);
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
			else if (channels == 4) {
				// Left data
				frame0_L = Mat(Size(width, height), CV_8UC4, frame_data[0]->left_data);
				frame1_L = Mat(Size(width, height), CV_8UC4, frame_data[1]->left_data);

				// Right data
				frame0_R = Mat(Size(width, height), CV_8UC4, frame_data[0]->right_data);
				frame1_R = Mat(Size(width, height), CV_8UC4, frame_data[1]->right_data);
			}

			hsvManager->filterHSVRange_out_8UC1(frame_data[1]->left_data, width, height, hsvRangeTGT, buf_8UC1_0);
			pred_scan_t engage_data = tgtPredator->engage_8UC1(buf_8UC1_0, width, height);

			// Render Field Delimiter and Score
			if (!configuration->dynamicModelParameters.freePlay) {

				OverlayRenderer::getInstance()->renderFieldDelimiter_Mat8UC4(frame1_L, fieldDelimiter);
				// Testing: OverlayRenderer::getInstance()->renderStaticModelScoreTracking(frame1_L, staticModel);
				OverlayRenderer::getInstance()->renderTwoPlayersPlayLogicScoreTracking(frame1_L, (TwoPlayersPlayLogic *)playLogic);
				OverlayRenderer::getInstance()->renderNet(frame1_L, netVisualProjection);
				OverlayRenderer::getInstance()->renderTwoPlayersFieldRepresentation(frame1_L, twoPlayersFieldRepresentation);
			}

			// Update Predator
			if (engage_data.xl != 0 && engage_data.xr != 0 && engage_data.row != 0) {

				tgtPredator->update_state(engage_data.xl + (engage_data.xr-engage_data.xl)/2, engage_data.row);
				Point targetPosition(engage_data.xl + (engage_data.xr-engage_data.xl)/2, engage_data.row);
				OverlayRenderer::getInstance()->renderTargetTracker(frame1_L, targetPosition);
				OverlayRenderer::getInstance()->renderPredatorState(frame1_L, tgtPredator);

				if (configuration->dynamicModelParameters.trackingWndEnabled && configuration->dynamicModelParameters.visualizeTrackingWnd) {
					OverlayRenderer::getInstance()->renderPredatorTrackingWnd(frame1_L, tgtPredator->get_tracking_wnd());
				}

				if (fd->depth_data_avail) {

					//OverlayRenderer::getInstance()->renderDepthInformation(frame1_L, targetPosition.x, targetPosition.y,
					//		ZEDStereoSensorDriver::readMeasurementDataDepth(fd->depth_data, targetPosition.x, targetPosition.y, fd->step_depth));

					StereoSensorMeasure3D meas;
					meas.z_mm = ZEDStereoSensorDriver::readMeasurementDataDepth(fd->depth_data, targetPosition.x, targetPosition.y, fd->step_depth);
					meas.x_mm = (meas.z_mm * (float)targetPosition.x) / Configuration::getInstance()->zedHardwareParameters.fx_L;
					meas.y_mm = (meas.z_mm * (float)targetPosition.y) / Configuration::getInstance()->zedHardwareParameters.fy_L;

					if (meas.z_mm > 0.0) {
						//OverlayRenderer::getInstance()->renderTarget3DPosition(frame1_L, targetPosition, meas);

						Vector3D meas_v;
						meas_v.x = (double)meas.x_mm;
						meas_v.y = (double)meas.y_mm;
						meas_v.z = (double)meas.z_mm;

						// Recalc dynamic model, and set 2D impact state for UMA guiding
						// If the prev 2D state is an impact, signal it to the 3D dynamic model before recalc
						if (tgtPredator->prev_state_is_impact()) {

							// Signal that the previously inserted state in the 3D dynamic model is an impact.
							// Prior to recalculating the dynamic model, the 3D impact state is the last in the model.
							dynamicModel->UMA_signal_impact_2D();
						}

						dyn_model_result_t dynamicModelResult = dynamicModel->recalc(meas_v, fd->t);

						if (dynamicModelResult.impact) {
							// Search in the TargetPredator backlog the last impact and mark it as confirmed
							Vector2D opticalLatestImpactData;
							pred_state_t latest_impact = tgtPredator->confirm_latest_impact();
							opticalLatestImpactData.x = (double)latest_impact.x;
							opticalLatestImpactData.y = (double)latest_impact.y;

							if (!configuration->dynamicModelParameters.freePlay) {
								playLogic->feedWithFloorBounceData(dynamicModelResult.impact_pos, opticalLatestImpactData);
							}
						}
					}
				}
			}

			// If the Target has been lost, notify the PlayLogic
			if (tgtPredator->TargetLost && configuration->dynamicModelParameters.notifyTGTLostToModel)
			{
				dynamicModel->UMA_reset();
				((TwoPlayersPlayLogic *)playLogic)->notifyTargetLost();
			}

			// If forcing RGB output
			if (force_rgb_output) {

			}
			// Using immuted original Frame Type
			else {
				memcpy(frame_data[0]->left_data, frame0_L.data, width * height * channels * sizeof(uint8_t));
			}

			// Enqueue output frame data
			if (array_spinlock_queue_push(&outputFramesQueue, (void *)frame_data[0]) < 0) {
				printf("Stereo Application :: WARNING :: Queue Push failed (@ Frame Counter %d)\n", frame_data[0]->frame_counter);
			}

			// Left-shift frame data in the local buffer, create space for new data
			frame_data[0] = frame_data[1];
		}
	}

	printf("Application :: Frames processor :: Processing terminated\n");

	free(buf_8UC1_0);
	//free(buf_8UC1_1);

	return NULL;
}

//
// Frames output process
//
void *frames_output(void *)
{
	struct timespec s, t;

	int counter = 0;

	StereoRecorder *stereoRecorder = new StereoRecorder();
	MonoRecorder *monoRecorder = new MonoRecorder();

	int width = Configuration::getInstance()->getFrameInfo().width;
	int height = Configuration::getInstance()->getFrameInfo().height;
	int channels = Configuration::getInstance()->getFrameInfo().channels;
	bool force_rgb_output = (Configuration::getInstance()->getFrameInfo().outputType == ForcedRGB);

	if (configuration->getOperationalMode().processingMode == Record)
	{
		/*
		printf("System :: CODEC :: Initializing async codec\n");
		codec_async_init();
		printf("System :: CODEC :: Async codec initialized\n");
		*/

		// Open binary serialization channel
		open_serialization_channel(configuration->recordingParameters.recordingFileNameFullPath);

		// Serialize binary Static Model
		serialize_static_model();

		while (systemRecording) {
			FrameData *frame_data;

			if (array_spinlock_queue_pull(&outputFramesQueue, (void **)&frame_data) < 0) {
				usleep(100);
				continue;
			}

			// Direct binary serialization
			serialize_frame_data(frame_data);

			/*
			printf("Serialization time: elapsed: %.2f [ms], Count: %d, Pressure: %.2f%%\n", nanotimer_rt_ms_diff(&s, &t),
					outputFramesQueue.count,
					((double)outputFramesQueue.count)/((double)QUEUE_MAX) * 100.0);
			*/

			// Free fast memory
			fast_mem_pool_release_memory(frame_data);
		}
	}

	close_serialization_channel();

	printf("Application :: Frames output :: Processing terminated\n");

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
	pthread_create(&frame_processor_thread, &frame_processor_attr, frames_processor, 0);
	printf("Stereo Application :: Frame processor thread running\n");

	// Start frame output thread when in Record mode
	if (config->operationalMode.processingMode == Record) {
		pthread_create(&frame_output_thread, &frame_output_attr, frames_output, 0);
		printf("Stereo Application :: Frame output thread running\n");
	}

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

		if (config->operationalMode.recordDynamicModelsData)
			queue = &inputFramesQueue;
		else
			queue = &outputFramesQueue;
		break;
	default:
		return;
	}

	if (config->getOperationalMode().inputDevice == StereoCameraDUO || config->getOperationalMode().inputDevice == StereoCameraZED) {

		if (!stereoSAL->isOpen()) {
			stereoSAL->openCamera();
		}

		printf("Stereo Application :: Input Device: Stereo camera (ZED)\n");

		StereoFrameSize frameSize = stereoSAL->getStereoFrameSize();

		// Frame counter
		int frame_counter = 0;

		// RT Clocks
		struct timespec sensor_acq_clk_start;
		struct timespec sensor_acq_clk_stop;

		// First clock measurement
		nanotimer_rt_start(&sensor_acq_clk_start);

		// Before acquiring the next frame, sleep a sufficient time to reach the current FPS rate
		// Time [ms] that must elapse between two succesive acquisitions (@ 30 FPS): 1000.0 * 0.03333333 = 33.33333333 [ms]
		double sensor_acq_period_ms = configuration->zedHardwareParameters.acquisitionPeriod;

		while (1) {

			// Stop the timer
			nanotimer_rt_stop(&sensor_acq_clk_stop);

			// Currently elapsed time [ms] since last acquisition
			double elapsed_since_last_acq_ms = nanotimer_rt_ms_diff(&sensor_acq_clk_start, &sensor_acq_clk_stop);

			// Difference to sleep in order to reach the acquisition period
			double delay_to_next_acq = (sensor_acq_period_ms - elapsed_since_last_acq_ms);

			// Sleep for the necessary time
			bool acq_late = false;
			if (delay_to_next_acq > 0) {
				usleep(ceil(delay_to_next_acq * 1000.0));
			}
			/* Measure acquisition latency */
			else {
				acq_late = true;
				printf("%.2f\n", delay_to_next_acq);
			}

			StereoFrame stereoFrame = stereoSAL->fetchStereoFrame();

			// Start the timer
			nanotimer_rt_start(&sensor_acq_clk_start);

			frame_counter++;

			if (stereoFrame.bytesLength <= 0) {
				printf("Stereo Application :: WARNING :: Invalid StereoFrame from hardware (@ Frame Counter: %d)\n", frame_counter);
				usleep(1000);
				continue;
			}

			// Allocate fast memory
			FrameData *frameData = fast_mem_pool_fetch_memory();

			while (frameData == NULL) {
				printf("Stereo Application :: WARNING :: Fast Memory Pool temporarily exhausted (@ Frame Counter: %d)\n", frame_counter);
				usleep(1000);
				frameData = fast_mem_pool_fetch_memory();
			}

			frameData->frame_counter = frame_counter;

			memcpy(frameData->left_data, stereoFrame.leftData, stereoFrame.bytesLength);
			//memcpy(frameData->right_data, stereoFrame.rightData, stereoFrame.bytesLength);

			if (stereoFrame.depthData != 0) {
				frameData->depth_data_avail = true;
				memcpy(frameData->depth_data, stereoFrame.depthData, frameSize.width * frameSize.height * sizeof(float));
				frameData->step_depth = stereoFrame.stepDepth;

				// XYZ data
				//frameData->xyzMat = stereoFrame.xyzMat;
				//memcpy(frameData->xyz_data, stereoFrame.xyzData, frameSize.width * frameSize.height * 4 * sizeof(float));
				frameData->step_xyz = stereoFrame.stepXYZ;

				// Confidence data
				//memcpy(frameData->confidence_data, stereoFrame.confidenceData, frameSize.width * frameSize.height * sizeof(float));
				//frameData->step_confidence = stereoFrame.stepConfidence;

				// Time measurement
				frameData->t = stereoFrame.t;
			}
			else {
				frameData->depth_data_avail = false;
			}

			// Enqueue stereo pair data in processing / output queue
			if (array_spinlock_queue_push(queue, (void *)frameData) < 0) {
				printf("Stereo Application :: WARNING :: Queue Push failed (@ Frame Counter %d)\n", frameData->frame_counter);
			}
		}
	}

	else if (config->getOperationalMode().inputDevice == StereoCameraVirtual)
	{
		VirtualStereoCamera *virtualStereoCamera = new VirtualStereoCamera();

		// Open binary recording stream
		virtualStereoCamera->OpenBinaryRecording(config->recordingParameters.recordingFileNameFullPath);

		// Load Static Model from binary recording stream
		virtualStereoCamera->LoadStaticModelBinaryRecording();

		// Field representation from model is performed once: only here if Replaying
		printf("Application :: Replay :: Generating Field Representation in Logic\n");
		playLogic->generateFieldRepresentationFromModel();

		// Signal Static Model Ready in Replay Mode
		staticModelReadyReplay = true;
		printf("Application :: Replay :: Binary Static Model Decoded: signaling Frames Processor\n");

		int prev_frame_counter = -1;

		while (1) {

			// Request Memory
			FrameData *frameData = fast_mem_pool_fetch_memory();

			// Memory unavailable, release CPU for some microsecs
			if (frameData == NULL)
			{
				usleep(1000);
				continue;
			}

			// Read binary recording data until available
			bool binary_data_avail = virtualStereoCamera->ReadBinaryRecordingData(frameData);

			// Check Frame Counter
			if (prev_frame_counter < 0) {
				prev_frame_counter = frameData->frame_counter;
			}
			else {
				if ((frameData->frame_counter - prev_frame_counter) > 1) {
					printf("Frames Processor :: WARNING :: Lost %d Frames (between Frames %d and %d)!\n", (frameData->frame_counter - prev_frame_counter),
							prev_frame_counter, frameData->frame_counter);
				}

				prev_frame_counter = frameData->frame_counter;
			}

			// Artificial latency introduction
			int latency_us = (int)((double)configuration->playbackParameters.playbackBaseLatency_us) / ((double)configuration->playbackParameters.playbackLatencyDivisor);
			usleep(latency_us);

			// No binary data more available, release memory, close virtual sensor device and leave
			if (!binary_data_avail) {
				fast_mem_pool_release_memory(frameData);
				virtualStereoCamera->CloseBinaryRecording();
				break;
			}

			// Enqueue stereo pair data in processing / output queue
			if (array_spinlock_queue_push(queue, (void *)frameData) < 0) {
				printf("Stereo Application :: Replay :: WARNING :: Queue Push failed (@ Frame Counter %d)\n", frameData->frame_counter);
			}

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
			if (array_spinlock_queue_push(queue, (void *)frameData) < 0) {
				printf("Stereo Application :: WARNING :: Queue Push failed (@ Frame Counter %d)\n", frameData->frame_counter);
			}

			nanotimer_rt_stop(&t);

			rt_elapsed = nanotimer_rt_ms_diff(&s, &t);
		}
	}

	printf("Frames and Measurements Acquisition :: Hardware or recorded data stream exhausted.\n");

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
// Direct Init
//
void directInit(StereoSensorAbstractionLayer **stereoSAL)
{
	systemReady = false;

	*stereoSAL = new ZEDStereoSensorDriver();
	(*stereoSAL)->openCamera();

	systemReady = true;
}

//
// Direct Fetch RAW Stereo Data from the ZED Camera Sensor
//
FrameData directFetchRawStereoData(StereoSensorAbstractionLayer *stereoSAL)
{
	FrameData frameData;

	StereoFrame sFrame = stereoSAL->fetchStereoFrame();

	frameData.left_data = sFrame.leftData;
	frameData.right_data = sFrame.rightData;

	return frameData;
}

//
// Application Entry Point
//
void run()
{
	// Affinity variables
	cpu_set_t cpu_set_main_thread;
	cpu_set_t cpu_set_frames_processor_thread;
	cpu_set_t cpu_set_frames_output_thread;

	CPU_ZERO(&cpu_set_main_thread);
	CPU_ZERO(&cpu_set_frames_processor_thread);
	CPU_ZERO(&cpu_set_frames_output_thread);

	CPU_SET(1, &cpu_set_main_thread);
	CPU_SET(2, &cpu_set_frames_processor_thread);
	CPU_SET(3, &cpu_set_frames_output_thread);

	int frames_processor_affinity_result = pthread_attr_setaffinity_np(&frame_processor_attr, sizeof(cpu_set_t), &cpu_set_frames_processor_thread);
	int frames_output_affinity_result = pthread_attr_setaffinity_np(&frame_output_attr, sizeof(cpu_set_t), &cpu_set_frames_output_thread);

	if (frames_processor_affinity_result == 0) {
		printf("Threading :: Success: Frames Processor assigned to CPU core 2\n");
	}
	else {
		printf("Threading :: Error: Frames Processor was not restricted to a specific CPU core\n");
	}

	if (frames_output_affinity_result == 0) {
		printf("Threading :: Success: Frames Output assigned to CPU core 3\n");
	}
	else {
		printf("Threading :: Error: Frames Output was not restricted to a specific CPU core\n");
	}

	int main_thread_affinity_result = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set_main_thread);
	if (main_thread_affinity_result == 0) {
		printf("Threading :: Success: Main Process assigned to CPU core 1\n");
	}
	else {
		printf("Threading :: Error: Main Process was not restricted to a specific CPU core\n");
	}

	systemReady = false;
	staticModelReadyReplay = false;

	StereoSensorAbstractionLayer *stereoSAL = 0;
	IRSensorAbstractionLayer *irSAL = 0;
	DepthSensorTechnology depthTech;

	// Instantiate objects
	configuration = Configuration::getInstance();
	hsvManager = HSVManager::getInstance();
	trajectoryTracker = new TrajectoryTracker();
	tgtPredator = TargetPredator::getInstance();
	staticModel = TennisFieldStaticModel::getInstance();
	dynamicModel = DynamicModel::getInstance();

	configuration->loadConfigFile("/home/ubuntu/Release/config_recording.xml");

	// Initialize fast memory pool
	printf("Initializing fast memory pool...\n");
	if (configuration->getFrameInfo().outputType == ForcedRGB) {
		fast_mem_pool_init(configuration->getFrameInfo().width, configuration->getFrameInfo().height, configuration->getFrameInfo().channels);
	}
	else {
		fast_mem_pool_init(configuration->getFrameInfo().width, configuration->getFrameInfo().height, configuration->getFrameInfo().channels);
	}
	printf("Fast memory pool initialized.\n");

	// Initialize array-based spinlock queues
	printf("Initializing I/O Frame queues...\n");
	array_spinlock_queue_init(&inputFramesQueue);
	array_spinlock_queue_init(&outputFramesQueue);
	outputFramesQueueExternPtr = &outputFramesQueue;
	printf("I/O Frame queues initialized, outputFramesQueueExternPtr = %p\n", outputFramesQueueExternPtr);

	// Operating Mode check
	switch(configuration->getOperationalMode().inputDevice)
	{
	case StereoCameraDUO:
		stereoSAL = new DUOStereoSensorDriver(configuration->getDUOStereoCameraHardwareParameters());
		depthTech = Stereo;
		stereoSALExternPtr = stereoSAL;
		break;
	case StereoCameraZED:
		stereoSAL = ZEDStereoSensorDriver::getInstance();
		depthTech = Stereo;
		stereoSALExternPtr = stereoSAL;
		break;
	case IrCameraKinect:
		irSAL = new KinectIRSensorDriver();
		depthTech = IR;
		break;
	}

	// Signal that the system hardware is ready
	if (!systemReady) {
		systemReady = true;
	}

	if (Configuration::getInstance()->getOperationalMode().inputDevice == StereoCameraDUO || Configuration::getInstance()->getOperationalMode().inputDevice == StereoCameraZED) {
		if (!stereoSAL->isOpen()) {
			stereoSAL->openCamera();
		}
	}

	// Wait for UI to complete calibration
	CalibrationData calibData = configuration->calibrationData;

	printf("SYSTEM :: CALIB :: (ENABLE CALIBRATION PARAM CHECK!) Param RESET: Calibration required.");

	while (!systemCalibrated) {
		usleep(1000000);
		printf("SYSTEM :: Waiting for calibration parameters...\n");
	}

	printf("Application :: Setting up Play Logic\n");

	switch(configuration->playLogicParameters.playLogicType)
	{
	case TwoPlayers:
		printf("Application :: Requesting TwoPlayersLogic to PlayLogicFactory\n");
		playLogic = PlayLogicFactory::getInstance()->requestTwoPlayersPlayLogic();
		break;
	case SinglePlayer:
		printf("Application :: Requesting SinglePlayerLogic to PlayLogicFactory\n");
		playLogic = PlayLogicFactory::getInstance()->requestSinglePlayerPlayLogic();
		break;
	default:
		break;
	}

//	printf("SYSTEM :: (DISABLE ME) Writing config file.\n");
//	configuration->writeConfigFile("config_recording.xml");

	switch (depthTech)
	{
	case Stereo:
		startStereoApplication(stereoSAL, configuration);
		break;
	case IR:
		startIrApplication(irSAL, configuration);
		break;
	}
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
						//TennisFieldStaticModel::checkTennisField_2D(&field1, (double)Cx, (double)Cy);
						//TennisFieldStaticModel::checkTennisField_2D(&field2, (double)Cx, (double)Cy);

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
				if (std::isinf(v.x) || std::isinf(v.y) || std::isinf(v.z) || std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || v.z >= 10000.0) {
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

				if (std::isinf(v.x) || std::isinf(v.y) || std::isinf(v.z) || std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || v.z >= 10000.0) {
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
