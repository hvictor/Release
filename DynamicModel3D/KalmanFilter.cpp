#include "KalmanFilter.h"
#include <stdio.h>

KalmanFilter::KalmanFilter() {
	// TODO Auto-generated constructor stub
	_k = 0;
	Q = 0;
}

KalmanFilter::~KalmanFilter() {
	// TODO Auto-generated destructor stub
}

double KalmanFilter::getR()
{
	return R;
}

double KalmanFilter::getQ()
{
	return Q;
}


double KalmanFilter::getModelX(int i)
{
	return x[i];
}

double KalmanFilter::getModelY(int i)
{
	return y[i];
}

double KalmanFilter::getModelZ(int i)
{
	return z[i];
}

void KalmanFilter::initParameters()
{
	x[0] = 0.0;
	y[0] = 0.0;
	z[0] = 0.0;

	Px[0]= 1.0;
	Py[0]= 1.0;
	Pz[0]= 1.0;

	A = 1.0;
	B = 0.0;
	H = 1.0;

	/*
	 * Good parameters for noise covariance, 09.12.2015 20:07
	 */
	Q = 0.5;
	R = 0.00009;
}

void KalmanFilter::recalc(int k, double meas_x, double meas_y, double meas_z)
{
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
}
