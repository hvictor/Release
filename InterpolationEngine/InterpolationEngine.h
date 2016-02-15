/*
 * InterpolationEngine.h
 *
 *  Created on: Feb 13, 2016
 *      Author: sled
 */

#ifndef INTERPOLATIONENGINE_H_
#define INTERPOLATIONENGINE_H_

#include "../OpticalLayer/StatefulObjectFilter.h"

// Macros
#define COEFF(i) (gsl_vector_get(c,(i)))
#define COVAR(i,j) (gsl_matrix_get(cov,(i),(j)))

// y = c0 + c1*x + c2*x^2 + c3*x^3
typedef struct
{
	double c0;
	double c1;
	double c2;
	double c3;
	double c4;
	double c5;
	double c6;
} HexaPolynomialCoeff;

// y = c0 + c1*x + c2*x^2 + c3*x^3
typedef struct
{
	double c0;
	double c1;
	double c2;
	double c3;
} CubicPolynomialCoeff;

// y = c0 + c1*x + c2*x^2
typedef struct
{
	double c0;
	double c1;
	double c2;
} QuadraticPolynomialCoeff;

class InterpolationEngine {
public:
	InterpolationEngine();
	virtual ~InterpolationEngine();
	CubicPolynomialCoeff interpolateCubicPolynomial(vector<TrackedState *> trajectoryStates);
	QuadraticPolynomialCoeff interpolateQuadraticPolynomial(vector<TrackedState *> trajectoryStates);
	HexaPolynomialCoeff interpolateHexaPolynomial(vector<TrackedState *> trajectoryStates);
};

#endif /* INTERPOLATIONENGINE_H_ */
