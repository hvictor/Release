#ifndef PHYSICALMODEL_H_
#define PHYSICALMODEL_H_

#include "DynamicModel3D.h"

#define TENNIS_BALL_RADIUS_m		3.43		// Avg. from Wikipedia
#define TENNIS_BALL_MASS_Kg			0.0578		// Avg. from Wikipedia
#define AIR_FRICTION_COEFF_SPHERE	0.47		// From Wikipedia

typedef struct
{
	int N;
	double *x;
	double *y;
	double *z;
} PhysicalModel;

#endif /* PHYSICALMODEL_H_ */
