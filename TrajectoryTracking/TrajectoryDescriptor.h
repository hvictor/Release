/*
 * TrajectoryDescriptor.h
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#ifndef TRAJECTORYDESCRIPTOR_H_
#define TRAJECTORYDESCRIPTOR_H_

#include "../Common/opencv_headers.h"
#include "../InterpolationEngine/InterpolationEngine.h"

typedef struct
{
	CubicPolynomialCoeff coeffs;
	HexaPolynomialCoeff hexa_coeffs;
	double x_from;
	double x_to;
	int index_from;
	int index_to;
	bool optimized;
} TrajectorySection;

class TrajectoryDescriptor {
public:
	TrajectoryDescriptor(int ID);
	virtual ~TrajectoryDescriptor();
	int getID();
	vector<TrajectorySection *> getTrajectorySections();
	void update(CubicPolynomialCoeff coeffs, double xFrom, double xTo, vector<TrackedState *> trackedStates, bool reverse);
	void update(HexaPolynomialCoeff coeffs, double xFrom, double xTo, int indexFrom, int indexTo, vector<TrackedState *> trackedStates, bool reverse);

private:
	int ID;
	vector<TrajectorySection *> trajectorySections;
	void optimize(vector<TrackedState *> trackedStates);
	void approxTrajectorySectionCubic(TrajectorySection *s, vector<TrackedState *> trackedStates);
};

#endif /* TRAJECTORYDESCRIPTOR_H_ */
