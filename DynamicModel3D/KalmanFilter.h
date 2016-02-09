#ifndef KalmanFilter_H_
#define KalmanFilter_H_

#include "../Configuration/configs.h"

#define PREDICT_QUEUE_LEN 	15


class KalmanFilter {
public:
	KalmanFilter();
	virtual ~KalmanFilter();
	void initParameters();
	void recalc(int k, double meas_x, double meas_y, double meas_z);
	double getModelX(int i);
	double getModelY(int i);
	double getModelZ(int i);
	double getR();
	double getQ();

private:
	int _k;

	// Output state of the Kalman Filter
	double x[CFG_3D_MODEL_KF_MEM_SIZE];
	double y[CFG_3D_MODEL_KF_MEM_SIZE];
	double z[CFG_3D_MODEL_KF_MEM_SIZE];

	// Predicted state
	double x_predicted[CFG_3D_MODEL_KF_MEM_SIZE];
	double y_predicted[CFG_3D_MODEL_KF_MEM_SIZE];
	double z_predicted[CFG_3D_MODEL_KF_MEM_SIZE];

	// Measured state
	double x_meas[CFG_3D_MODEL_KF_MEM_SIZE];
	double y_meas[CFG_3D_MODEL_KF_MEM_SIZE];
	double z_meas[CFG_3D_MODEL_KF_MEM_SIZE];


	double u[CFG_3D_MODEL_KF_MEM_SIZE];

	double Kx[CFG_3D_MODEL_KF_MEM_SIZE];
	double Ky[CFG_3D_MODEL_KF_MEM_SIZE];
	double Kz[CFG_3D_MODEL_KF_MEM_SIZE];

	double Px[CFG_3D_MODEL_KF_MEM_SIZE];
	double Py[CFG_3D_MODEL_KF_MEM_SIZE];
	double Pz[CFG_3D_MODEL_KF_MEM_SIZE];

	double Px1[CFG_3D_MODEL_KF_MEM_SIZE];
	double Py1[CFG_3D_MODEL_KF_MEM_SIZE];
	double Pz1[CFG_3D_MODEL_KF_MEM_SIZE];

	double A, B, H, Q, R;
};

#endif /* KalmanFilter_H_ */
