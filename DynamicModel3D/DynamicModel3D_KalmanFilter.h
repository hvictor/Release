#ifndef KalmanFilter_H_
#define KalmanFilter_H_

#include "../Configuration/configs.h"
#include "opencv2/video/tracking.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#define PREDICT_QUEUE_LEN 	15

using namespace cv;

class DynamicModel3D_KalmanFilter {
public:
	DynamicModel3D_KalmanFilter();
	virtual ~DynamicModel3D_KalmanFilter();
	void initParameters(double meas_x, double meas_y, double meas_z);
	void recalc(int k, double meas_x, double meas_y, double meas_z, double *est_x, double *est_y, double *est_z);

private:
	int _k;

	KalmanFilter kf;
	int stateSize;
	int measSize;
	int contrSize;

	Mat state;	// [x, y, z, vx, vy, vz]
	Mat meas;   // [measx, measy, measz]

	unsigned int type;
};

#endif /* KalmanFilter_H_ */
