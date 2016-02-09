
#include "PhysicalModel.h"
#include "../CUDA/gpu_physical_model.h"

vector<int> D_idx;
vector<MechanicalState *> S;

void aggregateCloseTrajectoryDiscontinuities(vector<int> trajectoryDiscontinuities)
{
	D_idx.clear();

	for (int k = 0; k < trajectoryDiscontinuities.size()-1; ) {
		D_idx.push_back(trajectoryDiscontinuities[k]);

		if (trajectoryDiscontinuities[k+1] - trajectoryDiscontinuities[k] < 3) {
			k += 2;
		}
		else {
			k++;
		}
	}
}

PhysicalModel *compute_physical_model(double v0_x, double v0_y, double v0_z, double x0, double y0, double z0, int N)
{
	PhysicalModel *physical_model = new PhysicalModel();

	physical_model->N = N;

	physical_model->x = (double *)malloc(N * sizeof(double));
	physical_model->y = (double *)malloc(N * sizeof(double));
	physical_model->z = (double *)malloc(N * sizeof(double));

	// Convert in standard measurement units [m]
	double x0_m = x0 / 1000.0;
	double y0_m = y0 / 1000.0;
	double z0_m = z0 / 1000.0;

	double v0_x_m = v0_x / 1000.0;
	double v0_y_m = v0_y / 1000.0;
	double v0_z_m = v0_z / 1000.0;

	physical_model->x[0] = x0_m;
	physical_model->y[0] = y0_m;
	physical_model->z[0] = z0_m;

	double g = 9.8; // g in mm/s^2
	double K = (6.0 * M_PI * AIR_FRICTION_COEFF_SPHERE * TENNIS_BALL_RADIUS_m) / TENNIS_BALL_MASS_Kg;

#ifdef CFG_USE_GPU
	gpu_physical_model_compute(g, K, v0_x_m, v0_y_m, v0_z_m, physical_model->x, physical_model->y, physical_model->z, N);
#endif

	return physical_model;
}

void PhysicalModel_Delete(PhysicalModel *physical_model)
{
	free(physical_model->x);
	free(physical_model->y);
	free(physical_model->z);
}

vector<PhysicalModel *> PhysicalModel_ComputeTrajectory(MechanicalStateTracking *trajectoryTracker, vector<int> trajectoryDiscontinuityIndexes)
{
	vector<PhysicalModel *> models;

	aggregateCloseTrajectoryDiscontinuities(trajectoryDiscontinuityIndexes);
	S = trajectoryTracker->status;

	for (int j = 0; j < D_idx.size(); j++) {
		double x0 = S[D_idx[j]]->objectLocation.x;
		double y0 = S[D_idx[j]]->objectLocation.y;
		double z0 = S[D_idx[j]]->objectLocation.z;

		double vx0 = S[D_idx[j]]->velocityX;
		double vy0 = S[D_idx[j]]->velocityY;
		double vz0 = S[D_idx[j]]->velocityZ;

		PhysicalModel *phymod = compute_physical_model(vx0, vy0, vz0, x0, y0, z0, 256);
		models.push_back(phymod);
	}

	return models;
}
