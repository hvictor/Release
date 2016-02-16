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

InterpolationEngine *InterpolationEngine::getInstance()
{
	static InterpolationEngine *instance = 0;

	if (!instance) {
		instance = new InterpolationEngine();
	}

	return instance;
}

HexaPolynomialCoeff InterpolationEngine::interpolateHexaPolynomial(vector<TrackedState *> trajectoryStates, int indexFrom, int indexTo)
{
	printf("InterpolationEngine::interpolateHexaPolynomial :: interp from %d to %d\n", indexFrom, indexTo);
	HexaPolynomialCoeff coeffs;

	if (indexTo - indexFrom + 1 < 7) {
		coeffs.c0 = 0;
		coeffs.c1 = 0;
		coeffs.c2 = 0;
		coeffs.c3 = 0;
		coeffs.c4 = 0;
		coeffs.c5 = 0;
		coeffs.c6 = 0;

		return coeffs;
	}

	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	double chisq;

	int n = indexTo - indexFrom + 1;//trajectoryStates.size();

	X = gsl_matrix_alloc(n, 7);
	y = gsl_vector_alloc(n);

	c = gsl_vector_alloc (7);
	cov = gsl_matrix_alloc (7, 7);

	double x0 = (double)trajectoryStates[0]->state.x;
	double xN = (double)trajectoryStates[trajectoryStates.size() - 1]->state.x;

	bool reverse = false;

	if (x0 > xN) {
		reverse = true;
	}

	if (!reverse) {
		int i = 0;
		for (int u = indexFrom; u <= indexTo; u++) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, i, 0, 1.0);
			gsl_matrix_set(X, i, 1, xi);
			gsl_matrix_set(X, i, 2, xi * xi);
			gsl_matrix_set(X, i, 3, xi * xi * xi);
			gsl_matrix_set(X, i, 4, xi * xi * xi * xi);
			gsl_matrix_set(X, i, 5, xi * xi * xi * xi * xi);
			gsl_matrix_set(X, i, 6, xi * xi * xi * xi * xi * xi);

			gsl_vector_set(y, i, yi);

			i++;
		}
	}
	else {
		int i = 0;
		for (int u = indexTo; u <= indexFrom; u--) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, i, 0, 1.0);
			gsl_matrix_set(X, i, 1, xi);
			gsl_matrix_set(X, i, 2, xi * xi);
			gsl_matrix_set(X, i, 3, xi * xi * xi);
			gsl_matrix_set(X, i, 4, xi * xi * xi * xi);
			gsl_matrix_set(X, i, 5, xi * xi * xi * xi * xi);
			gsl_matrix_set(X, i, 6, xi * xi * xi * xi * xi * xi);

			gsl_vector_set(y, i, yi);

			i++;
		}
	}

	gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, 7);

	gsl_multifit_linear(X, y, c, cov, &chisq, work);
	gsl_multifit_linear_free(work);

	coeffs.c0 = COEFF(0);
	coeffs.c1 = COEFF(1);
	coeffs.c2 = COEFF(2);
	coeffs.c3 = COEFF(3);
	coeffs.c4 = COEFF(4);
	coeffs.c5 = COEFF(5);
	coeffs.c6 = COEFF(6);

	gsl_matrix_free (X);
	gsl_vector_free (y);
	gsl_vector_free (w);
	gsl_vector_free (c);
	gsl_matrix_free (cov);

	return coeffs;
}

CubicPolynomialCoeff InterpolationEngine::interpolateCubicPolynomial(vector<TrackedState *> trajectoryStates, int indexFrom, int indexTo)
{
	CubicPolynomialCoeff coeffs;
	printf("InterpolationEngine::interpolateCubicPolynomial :: interp from %d to %d\n", indexFrom, indexTo);

	if (indexTo - indexFrom + 1 < 4) {
		coeffs.c0 = 0;
		coeffs.c1 = 0;
		coeffs.c2 = 0;
		coeffs.c3 = 0;

		return coeffs;
	}

	gsl_matrix *X, *cov;
	gsl_vector *y, *w, *c;
	double chisq;

	int n = indexTo - indexFrom + 1;//trajectoryStates.size();

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
		int i = 0;
		for (int u = indexFrom; u <= indexTo; u++) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, i, 0, 1.0);
			gsl_matrix_set(X, i, 1, xi);
			gsl_matrix_set(X, i, 2, xi * xi);
			gsl_matrix_set(X, i, 3, xi * xi * xi);

			gsl_vector_set(y, i, yi);

			i++;
		}
	}
	else {
		int i = 0;
		for (int u = indexTo; u <= indexFrom; u--) {
			TrackedState *s = trajectoryStates[u];
			double xi = (double)s->state.x;
			double yi = (double)s->state.y;

			gsl_matrix_set(X, i, 0, 1.0);
			gsl_matrix_set(X, i, 1, xi);
			gsl_matrix_set(X, i, 2, xi * xi);
			gsl_matrix_set(X, i, 3, xi * xi * xi);

			gsl_vector_set(y, i, yi);

			i++;
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



