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
} TrajectorySection;

class TrajectoryDescriptor {
public:
	TrajectoryDescriptor(int ID);
	virtual ~TrajectoryDescriptor();
	int getID();
	vector<TrajectorySection *> getTrajectorySections();
	void update(CubicPolynomialCoeff coeffs, double xFrom, double xTo);
	void update(HexaPolynomialCoeff coeffs, double xFrom, double xTo);

private:
	int ID;
	vector<TrajectorySection *> trajectorySections;
	void optimize();
};

#endif /* TRAJECTORYDESCRIPTOR_H_ */
