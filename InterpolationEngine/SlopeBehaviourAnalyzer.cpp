/*
 * SlopeBehaviourAnalyzer.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#include "SlopeBehaviourAnalyzer.h"

SlopeBehaviourAnalyzer::SlopeBehaviourAnalyzer()
{
}

SlopeBehaviourAnalyzer::~SlopeBehaviourAnalyzer()
{
}

SlopeBehaviourAnalyzer *SlopeBehaviourAnalyzer::getInstance()
{
	static SlopeBehaviourAnalyzer *instance = 0;

	if (!instance) {
		instance = new SlopeBehaviourAnalyzer();
	}

	return instance;
}

vector<int> SlopeBehaviourAnalyzer::computeLocalMinima(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo)
{
	vector<int> localMinima;

	double eps = 1E-3;

	// y		= C0 + C1x + C2*x^2 + C3*x^3 + C4*x^4 + C5*x^5 + C6*x^6
	// dy/dx	= C1 + 2*C2*x + 3*C3*x^2 + 4*C4*x^3 + 5*C5*x^4 + 6*C6*x^5
	// dy^2/dx	= 2*C2 + 6*C3*x + 12*C4*x^2 + 20*C5*x^3 + 30*C6*x^4

	// Local minima are points for which the first order differentiation is approximately 0,
	// and the second order differentiation is positive

	for (int i = indexFrom; i <= indexTo; i++) {
		double x = (double)trackedStates[i]->state.x;
		double first_differential = coeffs.c1 + 2*coeffs.c2*x + 3*coeffs.c3*x*x + 4*coeffs.c4*x*x*x + 5*coeffs.c5*x*x*x*x + 6*coeffs.c6*x*x*x*x*x;
		double second_differential = 2*coeffs.c2 + 6*coeffs.c3*x + 12*coeffs.c4*x*x + 20*coeffs.c5*x*x*x + 30*coeffs.c6*x*x*x*x;

		// Local minima
		printf("[index = %d] y' = %.4f, y'' = %.4f\n", i, first_differential, second_differential);
		if ((fabsl(first_differential) <= eps) && (second_differential > eps)) {
			printf("%d is a local minimum\n", i);
			localMinima.push_back(i);
		}
	}

	return localMinima;
}

