#include "DynamicModel3D_KalmanFilter.h"
#include <stdio.h>
#include <iostream>

using namespace std;

DynamicModel3D_KalmanFilter::DynamicModel3D_KalmanFilter() {
	// TODO Auto-generated constructor stub
	_k = 0;
	type = CV_64F;
	stateSize = 6;
	measSize = 3;
	contrSize = 0;

	kf.init(stateSize, measSize, contrSize, type);

	state = Mat(stateSize, 1, type);
	meas = Mat(measSize, 1, type);

	// Transition State Matrix A
	// Note: set dT at each processing step!
	// [ 1 0 0 dt  0 0 ]
	// [ 0 1 0 0  dt 0 ]
	// [ 0 0 1 0  0  dt]
	// [ 0 0 0 1  0  0 ]
	// [ 0 0 0 0  1 0  ]
	// [ 0 0 0 0  0 1  ]
	setIdentity(kf.transitionMatrix);
	kf.transitionMatrix.at<double>(0, 3) = 1.0;
	kf.transitionMatrix.at<double>(1, 4) = 1.0;
	kf.transitionMatrix.at<double>(2, 5) = 1.0;

	// Measure Matrix H
	// [ 1 0 0 0 0 0 ]
	// [ 0 1 0 0 0 0 ]
	// [ 0 0 1 0 0 0 ]

	kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
	kf.measurementMatrix.at<double>(0, 0) = 1.0f;
	kf.measurementMatrix.at<double>(1, 1) = 1.0f;
	kf.measurementMatrix.at<double>(2, 2) = 1.0f;

	// Process Noise Covariance Matrix Q
	// [ Ex 0  0    0 0    0 ]
	// [ 0  Ey 0    0 0    0 ]
	// [ 0  0  Ev_x 0 0    0 ]
	// [ 0  0  0    1 Ev_y 0 ]
	// [ 0  0  0    0 1    Ew ]
	// [ 0  0  0    0 0    Eh ]
	cv::setIdentity(kf.processNoiseCov, cv::Scalar(KF_PROCESS_NOISE_COV));
	// Measures Noise Covariance Matrix R
	setIdentity(kf.measurementNoiseCov, cv::Scalar(KF_AXIS_ERR_SIGMA*KF_AXIS_ERR_SIGMA));
	setIdentity(kf.errorCovPre, Scalar::all(KF_INIT_VALUE_P));

}

DynamicModel3D_KalmanFilter::~DynamicModel3D_KalmanFilter() {
	// TODO Auto-generated destructor stub
}

/*
double DynamicModel3D_KalmanFilter::getR()
{
	return R;
}

double DynamicModel3D_KalmanFilter::getQ()
{
	return Q;
}

double DynamicModel3D_KalmanFilter::getModelX(int i)
{
	//return x[i];
}

double DynamicModel3D_KalmanFilter::getModelY(int i)
{
	//return y[i];
}

double DynamicModel3D_KalmanFilter::getModelZ(int i)
{
	//return z[i];
}
*/

void DynamicModel3D_KalmanFilter::initParameters(double meas_x, double meas_y, double meas_z)
{
	kf.init(stateSize, measSize, contrSize, type);

	state = Mat(stateSize, 1, type);
	meas = Mat(measSize, 1, type);

	setIdentity(kf.transitionMatrix);
	kf.transitionMatrix.at<double>(0, 3) = 1.0;
	kf.transitionMatrix.at<double>(1, 4) = 1.0;
	kf.transitionMatrix.at<double>(2, 5) = 1.0;

	kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
	kf.measurementMatrix.at<double>(0, 0) = 1.0f;
	kf.measurementMatrix.at<double>(1, 1) = 1.0f;
	kf.measurementMatrix.at<double>(2, 2) = 1.0f;

	cv::setIdentity(kf.processNoiseCov, cv::Scalar(KF_PROCESS_NOISE_COV));
	// Measures Noise Covariance Matrix R
	setIdentity(kf.measurementNoiseCov, cv::Scalar(KF_AXIS_ERR_SIGMA*KF_AXIS_ERR_SIGMA));
	setIdentity(kf.errorCovPre, Scalar::all(KF_INIT_VALUE_P));

	kf.statePost.at<double>(0) = meas_x;
	kf.statePost.at<double>(1) = meas_y;
	kf.statePost.at<double>(2) = meas_z;
	kf.statePost.at<double>(3) = 0.0;
	kf.statePost.at<double>(4) = 0.0;
	kf.statePost.at<double>(5) = 0.0;

	setIdentity(kf.errorCovPost, Scalar::all(0.1));

	/*
	x[0] = 0.0;
	y[0] = 0.0;
	z[0] = 0.0;

	Px[0]= 1.0;
	Py[0]= 1.0;
	Pz[0]= 1.0;

	A = 1.0;
	B = 0.0;
	H = 1.0;

	//
	// Good parameters for noise covariance, 09.12.2015 20:07
	//
	//Q = 0.5;
	//R = 0.00009;

	Q = 0.5;
	R = 0.0000;
	*/
}


void DynamicModel3D_KalmanFilter::recalc(int k, double meas_x, double meas_y, double meas_z, double *est_x, double *est_y, double *est_z)
{
	cout << "KalmanFilter: Prediction" << endl;
	// Predict to update the internal state P matrix (statePre matrix)
	Mat prediction = kf.predict();

	double pred_x = prediction.at<double>(0);
	double pred_y = prediction.at<double>(1);
	double pred_z = prediction.at<double>(2);

	meas.at<double>(0) = meas_x;
	meas.at<double>(1) = meas_y;
	meas.at<double>(2) = meas_z;

	cout << "KalmanFilter: Correction" << endl;
	// Correct and get an estimate of the state
	Mat estimate = kf.correct(meas);

	*est_x = estimate.at<double>(0);
	*est_y = estimate.at<double>(1);
	*est_z = estimate.at<double>(2);

	cout << "KalmanFilter: Predict/Correct performed, status estimate available" << endl;
	/*
	_k = k;

	x_meas[k] = meas_x;
	y_meas[k] = meas_y;
	z_meas[k] = meas_z;

	// Predict
	x_predicted[k] = A * x[k - 1] + B * u[k];
	Px1[k] = A * A * Px[k - 1] + Q;

	y_predicted[k] = A * y[k - 1] + B * u[k];
	Py1[k] = A * A * Py[k - 1] + Q;

	z_predicted[k] = A * z[k - 1] + B * u[k];
	Pz1[k] = A * A * Pz[k - 1] + Q;

	//printf("[model] predictor: x=%lf, y=%lf, z=%lf", x_predicted[k], y_predicted[k], z_predicted[k]);

	// Correct
	Kx[k] = Px1[k] * H / (H * H * Px1[k] + R);
	x[k] = x_predicted[k] + Kx[k] * (x_meas[k] - H * x_predicted[k]);
	Px[k] = (1.0 - H * Kx[k]) * Px1[k];

	Ky[k] = Py1[k] * H / (H * H * Py1[k] + R);
	y[k] = y_predicted[k] + Ky[k] * (y_meas[k] - H * y_predicted[k]);
	Py[k] = (1.0 - H * Ky[k]) * Py1[k];

	Kz[k] = Pz1[k] * H / (H * H * Pz1[k] + R);
	z[k] = z_predicted[k] + Kz[k] * (z_meas[k] - H * z_predicted[k]);
	Pz[k] = (1.0 - H * Kz[k]) * Pz1[k];

	printf("[model][%d] filter output: x=%lf, y=%lf, z=%lf\n", k, x[k], y[k], z[k]);
	*/
}
