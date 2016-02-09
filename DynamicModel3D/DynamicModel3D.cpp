#include "DynamicModel3D.h"
#include <math.h>
#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <gsl/gsl_multifit.h>

using namespace cv;
using namespace cv::gpu;

Mat Q;

vector<MechanicalStateTracking *> mechanicalStateTrackings;

double lagrange(double *x, double *y, int n, double x_eval)
{
	double sigma = 0.0;

	for (int j = 0; j <= n; j++) {

		double accum = 1.0;

		for (int m = 0; m <= n; m++) {
			if (m == j) continue;

			accum *= (x_eval - x[m]) / (x[j] - x[m]);
		}

		sigma += y[j] * accum;
	}

	return sigma;
}

void DynamicModel3D_Init(double width, double height, double focalLengthCoeffX_R, double focalLengthCoeffX_L, double translationCoeffX)
{
	printf("width = %lf\nheight = %lf\nfxR = %lf\nfxL = %lf, tx = %lf\n", width, height, focalLengthCoeffX_R, focalLengthCoeffX_L, translationCoeffX);

	/*
    double _q[16] =
    {
        1.0, 0.0, 0.0, -width / 2.0,
        0.0, 1.0, 0.0, -height / 2.0,
        0.0, 0.0, 0.0, (focalLengthCoeffX_L + focalLengthCoeffX_R) / 2.0,
        0.0, 0.0, -1.0 / translationCoeffX, 0.0
    };
    */
    
    Q = Mat(4, 4, CV_32F);

    Q.at<float>(0, 0) = 1.0;
    Q.at<float>(0, 1) = 0.0;
    Q.at<float>(0, 2) = 0.0;
    Q.at<float>(0, 3) = -width / 2.0;

    Q.at<float>(1, 0) = 0.0;
    Q.at<float>(1, 1) = 1.0;
    Q.at<float>(1, 2) = 0.0;
    Q.at<float>(1, 3) = -height / 2.0;

    Q.at<float>(2, 0) = 0.0;
    Q.at<float>(2, 1) = 0.0;
    Q.at<float>(2, 2) = 0.0;
    Q.at<float>(2, 3) = (focalLengthCoeffX_L + focalLengthCoeffX_R) / 2.0;

    Q.at<float>(3, 0) = 0.0;
    Q.at<float>(3, 1) = 0.0;
    Q.at<float>(3, 2) = -1.0 / translationCoeffX;
    Q.at<float>(3, 3) = 0.0;
}

Mat get_Q()
{
	// Mat_<double> used here for easy << initialization
	cv::Mat_<double> cameraMatrix1(3,3); // 3x3 matrix
	cv::Mat_<double> distCoeffs1(8,1);   // 5x1 matrix for five distortion coefficients
	cv::Mat_<double> cameraMatrix2(3,3); // 3x3 matrix
	cv::Mat_<double> distCoeffs2(8,1);   // 5x1 matrix
	cv::Mat_<double> R(3,3);             // 3x3 matrix, rotation left to right camera
	cv::Mat_<double> T(3,1);             // * 3 * x1 matrix, translation left to right proj. center


	cameraMatrix1 << 200.207839, 0.0,153.666066,0.0,193.858211,115.804223,0.0,0.0,1.0;
	cameraMatrix2 << 198.479953,0.0,153.019315,0.0,191.988453,110.981719,0.0,0.0,1.0;

	distCoeffs1   << -16.984079,136.844508,0.004092,0.003719,29.086982, -16.506053,128.605165,96.928460;
	distCoeffs2   << -14.369877,96.287525,0.005821,0.002754,19.725816, -13.895703,89.381439,66.987963;

	R << 0.999988,0.001198, 0.004648, -0.001155, 0.999955, -0.009381, -0.004659, 0.009375, 0.999945;
	T << -30.262322, 0.083806, -0.575650;

	cv::Mat R1,R2,P1,P2,Q1;   // you're safe to leave OutpuArrays empty !
	Size imgSize(320, 240); // wild guess from you cameramat ( not that it matters )

	cv::stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, imgSize, R, T, R1, R2, P1, P2, Q1);

	cout << "Computed Q1 = " << Q1 << endl;

	return Q1;
}

void storeTracking(MechanicalStateTracking *tracking, uint64_t trajectoryID)
{
	char fileName[100];

	sprintf(fileName, "/tmp/mech-state-%ld.txt", (long)trajectoryID);

	FILE *fp = fopen(fileName, "w");

	for (int i = 0; i < tracking->status.size(); i++) {

		MechanicalState *currentState = tracking->status[i];

		fprintf(fp, "%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
				currentState->objectLocation.x,
				currentState->objectLocation.y,
				currentState->objectLocation.z,
				currentState->velocityX,
				currentState->velocityY,
				currentState->velocityZ,
				currentState->accelX,
				currentState->accelY,
				currentState->accelZ);

	}

	fclose(fp);
}

// Make new Mechanical State object
MechanicalState *makeMechanicalState(Vector3D objectLocation, uint32_t instant, bool impacted)
{
	MechanicalState *state = new MechanicalState();

	state->objectLocation = objectLocation;
	state->instant = instant;

	state->impacted = impacted;

	state->velocityX = 0.0;
	state->velocityY = 0.0;
	state->velocityZ = 0.0;

	state->accelX = 0.0;
	state->accelY = 0.0;
	state->accelZ = 0.0;

	return state;
}

// Make new Mechanical State Tracking object
MechanicalStateTracking *makeMechanicalStateTracking(MechanicalState *state, uint64_t trajectoryID)
{
	MechanicalStateTracking *tracking = new MechanicalStateTracking();
	tracking->kalmanFilter = new DynamicModel3D_KalmanFilter();

	tracking->filterCallCounter = 1;

	cout << "KalmanFilter: Initializing filter for Tracker " << trajectoryID << endl;
	tracking->kalmanFilter->initParameters(state->objectLocation.x, state->objectLocation.y, state->objectLocation.z);

	tracking->status.push_back(state);
	tracking->trajectoryID = trajectoryID;

	return tracking;
}

// Search Mechanical State Tracking object by Trajectory ID
MechanicalStateTracking *searchMechanicalStateTracking(uint64_t trajectoryID)
{
	for (vector<MechanicalStateTracking *>::iterator it = mechanicalStateTrackings.begin(); it != mechanicalStateTrackings.end(); ++it) {
		if ((*it)->trajectoryID == trajectoryID) {
			return *it;
		}
	}

	return 0;
}

double computeMechanicalStatesDistance(MechanicalState *s0, MechanicalState *s1)
{
	double delta_x = s1->objectLocation.x - s0->objectLocation.x;
	double delta_y = s1->objectLocation.y - s0->objectLocation.y;
	double delta_z = s1->objectLocation.z - s0->objectLocation.z;

	//cout << "computeMechanicalStateDistance(): delta_x = " << delta_x << ", delta_y = " << delta_y << " delta_z = " << delta_z << endl;

	double norm = sqrt(delta_x*delta_x + delta_y*delta_y + delta_z*delta_z);

	//cout << "computeMechanicalStateDistance(): norm = " << norm << endl;

	return norm;
}

// Get Trajectory 3D discontinuity indexes
// A discontinuity index is the index of the first element of another trajectory segment
vector<int> getTrajectory3DDiscontinuityIndexes(vector<MechanicalState *> status)
{
	vector<int> indexes;

	for (int i = 0; i < status.size() - 1; i++) {
		MechanicalState *s1 = status[i + 1];
		MechanicalState *s0 = status[i];

		cout << "getTrajectory3DDiscontinuityIndexes(): S1: " << s1->objectLocation.x << ", " << s1->objectLocation.y << ", " << s1->objectLocation.z << endl;
		cout << "getTrajectory3DDiscontinuityIndexes(): S0: " << s0->objectLocation.x << ", " << s0->objectLocation.y << ", " << s0->objectLocation.z << endl;

		double norm = computeMechanicalStatesDistance(s0, s1);

		if (status[i]->impacted || norm >= CFG_3D_MODEL_FITTING_TRAJECTORY_JUMP_NORM) {
			indexes.push_back(i + 1);

			if (status[i]->impacted)
				printf("getTrajectory3DDiscontinuityIndexes(): [IMPACT] added discontinuity index %d\n", i);
			else
				printf("getTrajectory3DDiscontinuityIndexes(): added discontinuity index %d with norm=%.2f\n", i, norm);
		}
	}

	return indexes;
}

// Trajectory 3D filtering
vector<int> filterTrajectory3D(MechanicalStateTracking *t)
{
	vector<int> discontinuityIndexes = getTrajectory3DDiscontinuityIndexes(t->status);

	printf("filterTrajectory3D(): have %d discontinuity indexes\n", discontinuityIndexes.size());

	unsigned int subTrajectoryStartIndex = 0;

	if (discontinuityIndexes.size() == 0) {
		unsigned int subTrajectoryStopIndex = t->status.size();

		// Smooth the trajectory section between [subTrajectoryStartIndex : subTrajectoryStopIndex]
		MechanicalState *startObject = t->status[subTrajectoryStartIndex];
		MechanicalState *endObject = t->status[subTrajectoryStopIndex];

		double coarseDeltaX = endObject->objectLocation.x - startObject->objectLocation.x;
		double coarseDeltaY = endObject->objectLocation.y - startObject->objectLocation.y;
		double coarseDeltaZ = endObject->objectLocation.y - startObject->objectLocation.y;

		// Correct trajectory
		for (unsigned int k = subTrajectoryStartIndex + 1; k < subTrajectoryStopIndex; k++) {
			// Use k and (k-1)

		}

	}
	else {
		for (unsigned int i = 0; i < t->status.size(); i++) {
			unsigned int subTrajectoryStopIndex = discontinuityIndexes[i];

			// Smooth the trajectory section between [subTrajectoryStartIndex : subTrajectoryStopIndex]
			MechanicalState *startObject = t->status[subTrajectoryStartIndex];
			MechanicalState *endObject = t->status[subTrajectoryStopIndex];

			double coarseDeltaX = endObject->objectLocation.x - startObject->objectLocation.x;
			double coarseDeltaY = endObject->objectLocation.y - startObject->objectLocation.y;
			double coarseDeltaZ = endObject->objectLocation.y - startObject->objectLocation.y;

			// Quadratic curve fitting using low-pass filter
			for (unsigned int k = subTrajectoryStartIndex + 1; k < subTrajectoryStopIndex; k++) {
				// Use k and (k-1)

			}

			subTrajectoryStartIndex = subTrajectoryStopIndex;
		}

		// Process last trajectory section
		for (unsigned int k = subTrajectoryStartIndex + 1; k < t->status.size(); k++) {
			// Use k and (k-1)
		}

	}

	return discontinuityIndexes;
}

void storeInterp(uint64_t trajectoryID, vector<double> x1, vector<double> y1, vector<double> z)
{
	char fileName[100];
	sprintf(fileName, "/tmp/trajectory%d.txt", (unsigned int)trajectoryID);
	FILE *fp = fopen(fileName, "w");

	for (int i = 0; i < x1.size(); i++) {
		fprintf(fp, "%.2f,%.2f,%.2f\n", x1[i], y1[i], z[i]);
		cout << "storeInterp(): Trajectory:[" << trajectoryID << "] z = " << z[i] << endl;
	}

	fclose(fp);
}

void getMechanicalStateDepthMinMax(vector<MechanicalState *> status, int fromIndex, int toIndex, double *zRangeStart, double *zRangeEnd)
{
	double zMin = status[0]->objectLocation.z;
	double zMax = status[0]->objectLocation.z;

	printf("getMechanicalStateDepthMinMax(): zMin=%g zMax=%g\n", zMin, zMax);

	for (int i = fromIndex + 1; i <= toIndex; i++) {
		if (status[i]->objectLocation.z < zMin) zMin = status[i]->objectLocation.z;
		if (status[i]->objectLocation.z > zMax) zMax = status[i]->objectLocation.z;
	}

	*zRangeStart = zMin;
	*zRangeEnd = zMax;
}

void DynamicModel3D_PolyInterpFitting(double *xi, double *yi, int n, double **coeffs)
{
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	double chisq;

	// Macros
#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))


	if (n >= 4) {
		printf("DynamicModel3D_PolyInterpFitting(): Using cubic polynomial interpolation\n");

		X = gsl_matrix_alloc (n, 4);
		y = gsl_vector_alloc (n);
		//w = gsl_vector_alloc (n);

		c = gsl_vector_alloc (4);
		cov = gsl_matrix_alloc (4, 4);

		for (int u = 0; u < n; u++) {
			gsl_matrix_set(X, u, 0, 1.0);
			gsl_matrix_set(X, u, 1, xi[u]);
			gsl_matrix_set(X, u, 2, xi[u] * xi[u]);
			gsl_matrix_set(X, u, 3, xi[u] * xi[u] * xi[u]);

			gsl_vector_set(y, u, yi[u]);
			//gsl_vector_set(w, u, 1.0);
		}

		gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 4);
		//gsl_multifit_wlinear(X, w, y, c, cov, &chisq, work);
		gsl_multifit_linear(X, y, c, cov, &chisq, work);
		gsl_multifit_linear_free(work);

		printf ("DynamicModel3D_PolyInterpFitting(): Y = %g + %gX + %gX^2 + %gX^3\n", C(0), C(1), C(2), C(3));
		(*coeffs)[0] = C(0);
		(*coeffs)[1] = C(1);
		(*coeffs)[2] = C(2);
		(*coeffs)[3] = C(3);

	}
	else {
		printf("DynamicModel3D_PolyInterpFitting(): Using quadratic polynomial interpolation\n");
		X = gsl_matrix_alloc (n, 3);
		y = gsl_vector_alloc (n);
		//w = gsl_vector_alloc (n);

		c = gsl_vector_alloc (3);
		cov = gsl_matrix_alloc (3, 3);

		for (int u = 0; u < n; u++) {
			gsl_matrix_set(X, u, 0, 1.0);
			gsl_matrix_set(X, u, 1, xi[u]);
			gsl_matrix_set(X, u, 2, xi[u] * xi[u]);

			gsl_vector_set(y, u, yi[u]);
			//gsl_vector_set(w, u, 1.0);
		}

		gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 3);
		//gsl_multifit_wlinear(X, w, y, c, cov, &chisq, work);
		gsl_multifit_linear(X, y, c, cov, &chisq, work);
		gsl_multifit_linear_free(work);

		printf ("DynamicModel3D_PolyInterpFitting(): Y = %g + %gX + %gX^2\n", C(0), C(1), C(2));
		(*coeffs)[0] = C(0);
		(*coeffs)[1] = C(1);
		(*coeffs)[2] = C(2);
		(*coeffs)[3] = 0.0;
	}


	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);

	printf("DynamicModel3D_PolyInterpFitting(): leaving\n");
}

void DynamicModel3D_SetLastObjectImpactState(uint64_t trajectoryID, bool impactStatus)
{
	MechanicalStateTracking *t;
	if (!(t = searchMechanicalStateTracking(trajectoryID))) {
		return;
	}

	t->status[t->status.size() - 1]->impacted = impactStatus;
}

// Fit Trajectory
void fitTrajectory3D(MechanicalStateTracking *t, vector<int> discontinuityIndexes)
{
	unsigned int subTrajectoryStartIndex = 0;

	double *x;
	double *y;
	double *z;

	double *coeffs_poly_x = (double *)malloc(sizeof(double) * 4);
	double *coeffs_poly_y = (double *)malloc(sizeof(double) * 4);

	vector<double> x1;
	vector<double> y1;
	vector<double> z1;

	if (t->status.size() < 4) return;

	double globalZStart;
	double globalZEnd;
	double globalDeltaZ;
	double globalZPos;

	getMechanicalStateDepthMinMax(t->status, 0, t->status.size()-1, &globalZStart, &globalZEnd);
	globalDeltaZ = (globalZEnd - globalZStart) / (double)((discontinuityIndexes.size()+1) * 100.0);
	globalZPos = globalZStart;

	printf("Fitting3D[%d] Last Z: %.4f\n", (int)t->trajectoryID, t->status[t->status.size()-1]->objectLocation.z);


	if (discontinuityIndexes.size() == 0) {
		unsigned int subTrajectoryStopIndex = t->status.size();

		x = (double *)malloc(t->status.size() * sizeof(double));
		y = (double *)malloc(t->status.size() * sizeof(double));
		z = (double *)malloc(t->status.size() * sizeof(double));

		int pos = 0;

		for (unsigned int k = subTrajectoryStartIndex; k < subTrajectoryStopIndex; k++) {
			// Use k and (k-1)
			x[pos] = t->status[k]->objectLocation.x;
			y[pos] = t->status[k]->objectLocation.y;
			z[pos] = t->status[k]->objectLocation.z;
			pos++;
		}

		if (pos < 3) {
			for (int q = 0; q < pos; q++) {
				globalZPos += pos*globalDeltaZ;
				x1.push_back(x[q]);
				y1.push_back(y[q]);
				z1.push_back(z[q]);
				cout << "Fitting3D[" << t->trajectoryID << "] storing " << pos << " points without interpolating" << endl;
				cout << "Fitting3D[" << t->trajectoryID << "] -----------------------------------" << endl;
			}
			return;
		}

		// Interpolate 1000 points on the ZX projection plane

		double zRangeEnd;
		double zRangeStart;

#ifdef CFG_3D_MODEL_ASSUMPTION_DEPTH_VELOCITY_MONOTONIC
		getMechanicalStateDepthMinMax(t->status, subTrajectoryStartIndex, subTrajectoryStopIndex-1, &zRangeStart, &zRangeEnd);
#else
		zRangeEnd = t->status[subTrajectoryStopIndex-1]->objectLocation.z;
		zRangeStart = t->status[subTrajectoryStartIndex]->objectLocation.z;
#endif

		double deltaZ = (zRangeEnd - zRangeStart) / 100.0;

		cout << "Fitting3D[" << t->trajectoryID << "] zRangeStart: " << zRangeStart << ", zRangeEnd: " << zRangeEnd << ", deltaZ: " << deltaZ << endl;

		cout << "fitTrajectory3D:    stateTracking:[" << t->trajectoryID << "]   interp: z range [" << zRangeStart << ", " << zRangeEnd << "]" << endl;

		// Interpolate x behaviour along z
		DynamicModel3D_PolyInterpFitting(z, x, pos, &coeffs_poly_x);

		// Interpolate y behaviour along z
		DynamicModel3D_PolyInterpFitting(z, y, pos, &coeffs_poly_y);

		for (int s = 0; s < 100; s++) {
			double zPos;
			double poly_eval;

			// X behaviour
#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
			zPos = globalZPos;
#else
			zPos = zRangeStart + s*deltaZ;
#endif

			poly_eval = coeffs_poly_x[0] + coeffs_poly_x[1]*zPos + coeffs_poly_x[2]*(zPos*zPos) + coeffs_poly_x[3]*(zPos*zPos*zPos);
			x1.push_back(poly_eval);

			// Y behaviour
			poly_eval = coeffs_poly_y[0] + coeffs_poly_y[1]*zPos + coeffs_poly_y[2]*(zPos*zPos) + coeffs_poly_y[3]*(zPos*zPos*zPos);
			y1.push_back(poly_eval);

			z1.push_back(zPos);

#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
			globalZPos += globalDeltaZ;
#endif
		}

		free(x);
		free(y);
		free(z);
	}
	else {

		for (int i = 0; i < discontinuityIndexes.size(); i++) {
			unsigned int subTrajectoryStopIndex = discontinuityIndexes[i];

			if (subTrajectoryStopIndex - subTrajectoryStartIndex < 3) {
				// Remove next discontinuity index
				if (i < discontinuityIndexes.size()-1) {
					discontinuityIndexes.erase(discontinuityIndexes.begin() + i + 1);
				}
				i--;
				// Go on without updating the startIndex, so it will be repeated
				continue;
			}

			x = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));
			y = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));
			z = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));

			int pos = 0;
			for (int k = subTrajectoryStartIndex; k < subTrajectoryStopIndex; k++) {
				x[pos] = t->status[k]->objectLocation.x;
				y[pos] = t->status[k]->objectLocation.y;
				z[pos] = t->status[k]->objectLocation.z;
				pos++;
			}

			if (pos < 3) {
				for (int q = 0; q < pos; q++) {
					globalZPos += pos*globalDeltaZ;
					x1.push_back(x[q]);
					y1.push_back(y[q]);
					z1.push_back(z[q]);
					cout << "Fitting3D[" << t->trajectoryID << "] storing " << pos << " points without interpolating" << endl;
					cout << "Fitting3D[" << t->trajectoryID << "] -----------------------------------" << endl;
				}
				return;
			}

			double zRangeEnd;
			double zRangeStart;

	#ifdef CFG_3D_MODEL_ASSUMPTION_DEPTH_VELOCITY_MONOTONIC
			getMechanicalStateDepthMinMax(t->status, subTrajectoryStartIndex, subTrajectoryStopIndex-1, &zRangeStart, &zRangeEnd);
	#else
			zRangeEnd = t->status[subTrajectoryStopIndex-1]->objectLocation.z;
			zRangeStart = t->status[subTrajectoryStartIndex]->objectLocation.z;
	#endif

			double deltaZ = (zRangeEnd - zRangeStart) / 100.0;

			cout << "Fitting3D[" << t->trajectoryID << "] Discontinuity: " << i << " zRangeStart: " << zRangeStart << ", zRangeEnd: " << zRangeEnd << ", deltaZ: " << deltaZ << endl;

			cout << "fitTrajectory3D: [Discontinuties]   stateTracking:[" << t->trajectoryID << "]   interp: z range [" << zRangeStart << ", " << zRangeEnd << "]" << endl;

			// Interpolate x behaviour along z
			DynamicModel3D_PolyInterpFitting(z, x, pos, &coeffs_poly_x);

			// Interpolate y behaviour along z
			DynamicModel3D_PolyInterpFitting(z, y, pos, &coeffs_poly_y);

			for (int s = 0; s < 100; s++) {
				double zPos;
				double poly_eval;

				// X behaviour
	#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
				zPos = globalZPos;
	#else
				zPos = zRangeStart + s*deltaZ;
	#endif

				poly_eval = coeffs_poly_x[0] + coeffs_poly_x[1]*zPos + coeffs_poly_x[2]*(zPos*zPos) + coeffs_poly_x[3]*(zPos*zPos*zPos);
				x1.push_back(poly_eval);

				// Y behaviour
				poly_eval = coeffs_poly_y[0] + coeffs_poly_y[1]*zPos + coeffs_poly_y[2]*(zPos*zPos) + coeffs_poly_y[3]*(zPos*zPos*zPos);
				y1.push_back(poly_eval);

				z1.push_back(zPos);

	#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
				globalZPos += globalDeltaZ;
	#endif
			}

			free(x);
			free(y);
			free(z);

			subTrajectoryStartIndex = subTrajectoryStopIndex;
		}

		// Last trajectory segment
		unsigned int subTrajectoryStopIndex = t->status.size();

		x = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));
		y = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));
		z = (double *)malloc((subTrajectoryStopIndex - subTrajectoryStartIndex) * sizeof(double));

		int pos = 0;
		for (int k = subTrajectoryStartIndex; k < subTrajectoryStopIndex; k++) {
			x[pos] = t->status[k]->objectLocation.x;
			y[pos] = t->status[k]->objectLocation.y;
			z[pos] = t->status[k]->objectLocation.z;
			pos++;
		}

		if (pos < 3) {
			for (int q = 0; q < pos; q++) {
				globalZPos += pos*globalDeltaZ;
				x1.push_back(x[q]);
				y1.push_back(y[q]);
				z1.push_back(z[q]);
				cout << "Fitting3D[" << t->trajectoryID << "] storing " << pos << " points without interpolating" << endl;
				cout << "Fitting3D[" << t->trajectoryID << "] -----------------------------------" << endl;
			}
			return;
		}

		double zRangeEnd;
		double zRangeStart;

#ifdef CFG_3D_MODEL_ASSUMPTION_DEPTH_VELOCITY_MONOTONIC
		getMechanicalStateDepthMinMax(t->status, subTrajectoryStartIndex, subTrajectoryStopIndex-1, &zRangeStart, &zRangeEnd);
#else
		zRangeEnd = t->status[subTrajectoryStopIndex-1]->objectLocation.z;
		zRangeStart = t->status[subTrajectoryStartIndex]->objectLocation.z;
#endif

		double deltaZ = (zRangeEnd - zRangeStart) / 100.0;

		cout << "Fitting3D[" << t->trajectoryID << "] Final section, zRangeStart: " << zRangeStart << ", zRangeEnd: " << zRangeEnd << ", deltaZ: " << deltaZ << endl;

		cout << "fitTrajectory3D: [Last trajectory section]   stateTracking:[" << t->trajectoryID << "]   interp: z range [" << zRangeStart << ", " << zRangeEnd << "]" << endl;

		// Interpolate y behaviour along z
		DynamicModel3D_PolyInterpFitting(z, x, pos, &coeffs_poly_x);

		// Interpolate z behaviour along z
		DynamicModel3D_PolyInterpFitting(z, y, pos, &coeffs_poly_y);

		for (int s = 0; s < 100; s++) {
			double zPos;
			double poly_eval;

			// X behaviour
#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
			zPos = globalZPos;
#else
			zPos = zRangeStart + s*deltaZ;
#endif

			poly_eval = coeffs_poly_x[0] + coeffs_poly_x[1]*zPos + coeffs_poly_x[2]*(zPos*zPos) + coeffs_poly_x[3]*(zPos*zPos*zPos);
			x1.push_back(poly_eval);

			// Y behaviour
			poly_eval = coeffs_poly_y[0] + coeffs_poly_y[1]*zPos + coeffs_poly_y[2]*(zPos*zPos) + coeffs_poly_y[3]*(zPos*zPos*zPos);
			y1.push_back(poly_eval);

			z1.push_back(zPos);

#ifdef CFG_3D_MODEL_USE_GLOBAL_TRAJECTORY_Z_VARIABLE
			globalZPos += globalDeltaZ;
#endif
		}

		free(x);
		free(y);
		free(z);
	}

	cout << "Fitting3D[" << t->trajectoryID << "] ---------------------------------------------------" << endl;
	storeInterp(t->trajectoryID, x1, y1, z1);

	free(coeffs_poly_x);
	free(coeffs_poly_y);
}

// Feed the 3D Dynamic Model with new data and recalculate the trajectories state
void DynamicModel3D_RecalcTrajectoryMechanicalState(Vector3D v, bool impacted, uint32_t instant, uint64_t trajectoryID)
{
	// Invalid z position
	if (v.z >= 10000.0) return;

	if (isnan(v.x) || isnan(v.y) || isnan(v.z))
		return;

	if (isinf(v.x) || isinf(v.y) || isinf(v.z))
		return;

	// First set the correct measurement unit scale
	/*
	v.x /= 1000.0;
	v.y /= 1000.0;
	v.z /= 1000.0;
	*/

	if (impacted) {
		printf("IMPACT: RECALC with impact\n");
	}

	// Search a corresponding existing tracker
	MechanicalStateTracking *tracking = searchMechanicalStateTracking(trajectoryID);

	// Tracker not found, create it new
	if (!tracking) {
		cout << "[Dynamic Model 3D]: Making new mechanical state tracking for trajectory ID " << trajectoryID << endl;

		// Kalman Filter called automatically the first time
		tracking = makeMechanicalStateTracking(makeMechanicalState(v, instant, impacted), trajectoryID);
		mechanicalStateTrackings.push_back(tracking);

		return;
	}

	// Make new mechanical state
	MechanicalState *current_state = makeMechanicalState(v, instant, impacted);

	// If there is a mechanical state jump, re-initialize the internal tracker's Kalman Filter to operate
	// On the observed 3D trajectory segment
	// Compute distance with the current_status and the last status already in the tracker
	// The current_status will be filtered and then inserted
	if (impacted) {// || computeMechanicalStatesDistance(tracking->status[tracking->status.size() - 1], current_state) >= CFG_3D_MODEL_FITTING_TRAJECTORY_JUMP_NORM) {
		printf("[Dynamic Model 3D]: Impact or Jump between (%.2f, %.2f, %.2f) and (%.2f, %.2f, %.2f)\n",
				tracking->status[tracking->status.size() - 1]->objectLocation.x,
				tracking->status[tracking->status.size() - 1]->objectLocation.y,
				tracking->status[tracking->status.size() - 1]->objectLocation.z,
				current_state->objectLocation.x,
				current_state->objectLocation.y,
				current_state->objectLocation.z);

		tracking->kalmanFilter->initParameters(current_state->objectLocation.x, current_state->objectLocation.y, current_state->objectLocation.z);
		tracking->filterCallCounter = 1;
	}

	cout << "[Dynamic Model 3D]: Trajectory:[" << trajectoryID << "] KalmanFilter: Pre-Filtering (" << current_state->objectLocation.x << ", " << current_state->objectLocation.y << ", " << current_state->objectLocation.z << ")" << endl;

	// Filter the mechanical state
#ifdef CFG_3D_MODEL_USE_KALMAN_FILTER
	double est_x, est_y, est_z;
	tracking->kalmanFilter->recalc(tracking->filterCallCounter, current_state->objectLocation.x, current_state->objectLocation.y, current_state->objectLocation.z, &est_x, &est_y, &est_z);

	current_state->objectLocation.x = est_x;//tracking->kalmanFilter->getModelX(tracking->filterCallCounter);
	current_state->objectLocation.y = est_y;//tracking->kalmanFilter->getModelY(tracking->filterCallCounter);
	current_state->objectLocation.z = est_z;//tracking->kalmanFilter->getModelZ(tracking->filterCallCounter);

	tracking->filterCallCounter++;
#endif

	cout << "[Dynamic Model 3D]: Trajectory:[" << trajectoryID << "] KalmanFilter: Filtered (" << current_state->objectLocation.x << ", " << current_state->objectLocation.y << ", " << current_state->objectLocation.z << ")" << endl;

	tracking->status.push_back(current_state);

	// If the tracker knows more than one mechanical state, recalculate model fitting
	if (tracking->status.size() > 1) {
		// Model fitting
		MechanicalState *latest_state = tracking->status[tracking->status.size() - 2];
		double dt = (double)(current_state->instant - latest_state->instant);

		current_state->velocityX = (current_state->objectLocation.x - latest_state->objectLocation.x) / dt;
		current_state->velocityY = (current_state->objectLocation.y - latest_state->objectLocation.y) / dt;
		current_state->velocityZ = (current_state->objectLocation.z - latest_state->objectLocation.z) / dt;

		cout 	<< "[Dynamic Model 3D]: Current state [" << trajectoryID << "] updated velocity: ["
				<< current_state->velocityX << ", "
				<< current_state->velocityY << ", "
				<< current_state->velocityZ << "]" << endl;

		// If the state is not the first, it has a velocity and computing the acceleration is possible

		// Set acceleration
		// This is enabled with OR 1 logical condition
		// Only the Y component has acceleration of -g (-9.81 = -0.00981 m/s^2)
		if (true || (latest_state != tracking->status[0])) {
			current_state->accelX = 0.0;//(current_state->velocityX - latest_state->velocityX) / dt;
			current_state->accelY = 0.00981;//(current_state->velocityY - latest_state->velocityY) / dt - 0.00981;
			current_state->accelZ = 0.0;//(current_state->velocityZ - latest_state->velocityZ) / dt;
		}

	}

	// Filtering and smoothing
	vector<int> discontinuityIndexes = getTrajectory3DDiscontinuityIndexes(tracking->status);

	// Fitting
	fitTrajectory3D(tracking, discontinuityIndexes);

	cout << "DISABLE: [Dynamic Model 3D] Storing current state" << endl;
	storeTracking(tracking, trajectoryID);
}

vector<Vector3D> DynamicModel3D_PredictMechanicalState(uint64_t trajectoryID, uint8_t timeLookAhead)
{
	vector<Vector3D> future_states;
	MechanicalStateTracking *t = searchMechanicalStateTracking(trajectoryID);

	if (t == NULL) {
		cout << "PredictMechanicalState: " << "[" << trajectoryID << "] Tracker not found" << endl;
		return future_states;
	}
	if (t->status.size() <= 2) {
		cout << "PredictMechanicalState: " << "[" << trajectoryID << "] Too few status information" << endl;
		return future_states;
	}

	MechanicalState *latest_state = t->status[t->status.size() - 1];

	cout 	<< "PredictMechanicalState: " << "[" << trajectoryID << "] Position = ("
			<< latest_state->objectLocation.x << ", "
			<< latest_state->objectLocation.y << ", "
			<< latest_state->objectLocation.z << ")" << endl;


	cout 	<< "PredictMechanicalState: " << "[" << trajectoryID << "] Velocity = ("
			<< latest_state->velocityX << ", "
			<< latest_state->velocityY << ", "
			<< latest_state->velocityZ << ")" << endl;

	cout 	<< "PredictMechanicalState: [" << trajectoryID << "] Acceleration = ("
			<< latest_state->accelX << ", "
			<< latest_state->accelY << ", "
			<< latest_state->accelZ << ")" << endl;

	for (int i = 1; i <= timeLookAhead; i++) {
		Vector3D fv;

		// X dimension
		if (latest_state->accelX == 0.0)
			fv.x = latest_state->objectLocation.x + (latest_state->velocityX * (double)i);
		else
			fv.x = latest_state->objectLocation.x + latest_state->velocityX * ((double)i);// + (double)(0.5 * latest_state->accelX * (double)(i*i));

		// Y dimension
		if (latest_state->accelY == 0.0)
			fv.y = latest_state->objectLocation.y + (latest_state->velocityY * (double)i);
		else
			fv.y = latest_state->objectLocation.y + latest_state->velocityY * ((double)i) + (double)(0.5 * latest_state->accelY * (double)(i*i));

		// Z dimension
		if (latest_state->accelX == 0.0)
			fv.z = latest_state->objectLocation.z + (latest_state->velocityZ * (double)i);
		else
			fv.z = latest_state->objectLocation.z + latest_state->velocityZ * ((double)i);// + (double)(0.5 * latest_state->accelZ * (double)(i*i));

		//fv.x *= 1000.0;
		//fv.y *= 1000.0;
		//fv.z *= 1000.0;

		cout 	<< "PredictMechanicalState: [" << trajectoryID << "] currPosition["
				<< latest_state->objectLocation.x << ", "
				<< latest_state->objectLocation.y << ", "
				<< latest_state->objectLocation.z << "]    predictedPosition["
				<< fv.x << ", " << fv.y << ", " << fv.z << "]" << endl;

		future_states.push_back(fv);
	}

	return future_states;
}

Vector3D DynamicModel3D_GPU_ComputeDepth(double Cx_R, double Cy_R, double Cx_L, double Cy_L)
{
	Mat_<cv::Vec3f> XYZ(1,1);   // Output point cloud
	Mat_<float> vec_tmp(4,1);

	float disp = Cx_L-Cx_R;

	vec_tmp(0)=Cx_L; vec_tmp(1)=Cy_L; vec_tmp(2)=disp; vec_tmp(3)=1;
	vec_tmp = Q*vec_tmp;
	vec_tmp /= vec_tmp(3);
	Vec3f point = XYZ.at<cv::Vec3f>(0, 0);

	point[0] = vec_tmp(0);
	point[1] = vec_tmp(1);
	point[2] = vec_tmp(2);

	Vector3D v;

	v.x = point[0];
	v.y = point[1];
	v.z = point[2];

	cout << "Q: Projection in 3D: " << point << endl;

	return v;
}

Vector3D DynamicModel3D_GPU_ComputeSingle3DPositionVector(double Cx_R, double Cy_R, double Cx_L, double Cy_L)
{
    Vector3D pVec3D;
    
    // Disparity

    printf("CR = (%lf, %lf)    CL = (%lf, %lf)\n", Cx_R, Cy_R, Cx_L, Cy_L);
    double disp = Cx_L - Cx_R;//sqrt((Cx_R - Cx_L)*(Cx_R - Cx_L) + (Cy_R - Cy_L)*(Cy_R - Cy_L));
    
    printf("Disparity ---> %lf\n", disp);

    Mat disparityImage(320, 240, CV_16S);
    
    disparityImage.at<int16_t>(Cy_L, Cx_L) = (int16_t)disp;

    GpuMat d_xyzw(320, 240, CV_32FC4);
    GpuMat d_disparityImage(disparityImage);
    
    printf("disparityImage contanis %d\n", disparityImage.at<int16_t>(Cy_L, Cx_L));
    cout << "Q: " << Q << endl;

    Mat Q1;
    get_Q().convertTo(Q1, CV_32F);

    cout << "Q1: " << Q1 << endl;

    gpu::reprojectImageTo3D(d_disparityImage, d_xyzw, Q);
    
    Mat h_xyzw(d_xyzw);
    
    Vec4f v = h_xyzw.at<Vec4f>(Cy_L, Cx_L);
    pVec3D.x = v.val[0];
    pVec3D.y = v.val[1];
    pVec3D.z = v.val[2];
    
    cout << "Q: Projection in 3D: " << v << endl;

    return pVec3D;
}
