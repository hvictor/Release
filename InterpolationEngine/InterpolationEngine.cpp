/*
 * InterpolationEngine.cpp
 *
 *  Created on: Feb 13, 2016
 *      Author: sled
 */

#include "InterpolationEngine.h"
#include <gsl/gsl_multifit.h>

InterpolationEngine::InterpolationEngine()
{

}

InterpolationEngine::~InterpolationEngine()
{

}

CubicPolynomialCoeff InterpolationEngine::interpolateCubicPolynomial(vector<TrackedState *> trajectoryStates)
{
	CubicPolynomialCoeff coeffs;

	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	double chisq;

	int n = trajectoryStates.size();

	X = gsl_matrix_alloc(n, 4);
	y = gsl_vector_alloc(n);

	c = gsl_vector_alloc (4);
	cov = gsl_matrix_alloc (4, 4);

	double x0 = (double)trajectoryStates[0]->state.x;
	double xN = (double)trajectoryStates[trajectoryStates.size() - 1]->state.x;

	bool reverse = false;

	if (x0 > xN) {
		reverse = true;
	}

	if (!reverse) {
		for (int u = 0; u < n; u++) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, u, 0, 1.0);
			gsl_matrix_set(X, u, 1, xi);
			gsl_matrix_set(X, u, 2, xi * xi);
			gsl_matrix_set(X, u, 3, xi * xi * xi);

			gsl_vector_set(y, u, yi);
		}
	}
	else {
		for (int u = n-1; u <= 0; u--) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, u, 0, 1.0);
			gsl_matrix_set(X, u, 1, xi);
			gsl_matrix_set(X, u, 2, xi * xi);
			gsl_matrix_set(X, u, 3, xi * xi * xi);

			gsl_vector_set(y, u, yi);
		}
	}

	gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 4);

	gsl_multifit_linear(X, y, c, cov, &chisq, work);
	gsl_multifit_linear_free(work);

	coeffs.c0 = COEFF(0);
	coeffs.c1 = COEFF(1);
	coeffs.c2 = COEFF(2);
	coeffs.c3 = COEFF(3);

	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);

	return coeffs;
}

QuadraticPolynomialCoeff InterpolationEngine::interpolateQuadraticPolynomial(vector<TrackedState *> trajectoryStates)
{
	QuadraticPolynomialCoeff coeffs;
	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	double chisq;

	int n = trajectoryStates.size();

	X = gsl_matrix_alloc(n, 3);
	y = gsl_vector_alloc(n);

	c = gsl_vector_alloc (3);
	cov = gsl_matrix_alloc (3, 3);

	for (int u = 0; u < n; u++) {
		TrackedState *s = trajectoryStates[u];
		double xi = (double)s->state.x;
		double yi = (double)s->state.y;

		gsl_matrix_set(X, u, 0, 1.0);
		gsl_matrix_set(X, u, 1, xi);
		gsl_matrix_set(X, u, 2, xi * xi);

		gsl_vector_set(y, u, yi);
	}

	gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 3);

	gsl_multifit_linear(X, y, c, cov, &chisq, work);
	gsl_multifit_linear_free(work);

	coeffs.c0 = COEFF(0);
	coeffs.c1 = COEFF(1);
	coeffs.c2 = COEFF(2);

	return coeffs;
}



