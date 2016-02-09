#ifndef ____DynamicModel3D__
#define ____DynamicModel3D__

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "DynamicModel3D_KalmanFilter.h"
#include "../Configuration/configs.h"
#include "../Common/data_types.h"

using namespace std;
using namespace cv;

typedef struct
{
	Vector3D objectLocation;
	uint32_t instant;

	double accelX, accelY, accelZ;
	double velocityX, velocityY, velocityZ;

	bool impacted;
} MechanicalState;

typedef struct
{
	uint64_t trajectoryID;
	vector<MechanicalState *> status;
	DynamicModel3D_KalmanFilter *kalmanFilter;
	int filterCallCounter;
} MechanicalStateTracking;

void DynamicModel3D_Init(double width, double height, double focalLengthCoeffX_R, double focalLengthCoeffX_L, double translationCoeffX);
Vector3D DynamicModel3D_GPU_ComputeSingle3DPositionVector(double Cx_R, double Cy_R, double Cx_L, double Cy_L);
Vector3D DynamicModel3D_GPU_ComputeDepth(double Cx_R, double Cy_R, double Cx_L, double Cy_L);

// Feed the 3D Dynamic Model with new data and recalculate the trajectories state
void DynamicModel3D_RecalcTrajectoryMechanicalState(Vector3D v, bool impacted, uint32_t instant, uint64_t trajectoryID);
vector<Vector3D> DynamicModel3D_PredictMechanicalState(uint64_t trajectoryID, uint8_t timeLookAhead);
void DynamicModel3D_SetLastObjectImpactState(uint64_t trajectoryID, bool impactStatus);

#endif /* defined(____DynamicModel3D__) */
