/*
 * SlopeBehaviourAnalyzer.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#include "SlopeBehaviourAnalyzer.h"
#include "../Configuration/Configuration.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

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

// TODO: CUDA Local Minima!!
vector<int> SlopeBehaviourAnalyzer::computeLocalMinima(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo)
{
	vector<int> localMinima;

	double eps = 0.01;

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
		if ((fabsl(first_differential) <= eps)) { //&& (second_differential < eps)) {

			if (extremaType(coeffs, x, (double)trackedStates[indexFrom]->state.x, (double)trackedStates[indexTo]->state.x) == EXTREMA_TYPE_LOCAL_MIN) {
				localMinima.push_back(i);
			}

			// EXPERIMENTAL:
			// Accept only one local extrema (for real-time online tracking)
			return localMinima;
		}
	}

	return localMinima;
}

int SlopeBehaviourAnalyzer::searchNearestLocalMinimumDiscrete(HexaPolynomialCoeff coeffs, vector<TrackedState *> trackedStates, int indexFrom, int indexTo)
{
	static double lastValue;
	static double lastIndex;
	static double firstPeakValue;
	static bool firstPeakPassed = false;

	static bool descending = false;

	int confirmation = 0;

	for (int i = indexFrom; i <= indexTo; i++) {
		double x = (double)trackedStates[i]->state.x;
		double y = coeffs.c0 + coeffs.c1*x + coeffs.c2*x*x + coeffs.c3*x*x*x + coeffs.c4*x*x*x*x + coeffs.c5*x*x*x*x*x + coeffs.c6*x*x*x*x*x*x;

		if (i == indexFrom) {
			lastValue = y;
			continue;
		}

		if (descending && y > lastValue) {
			confirmation++;

			// TODO: parameterize this
			if (confirmation > 2) {
				return i;
			}
		}
		else {
			confirmation = 0;
		}

		// Hill-climbing
		if (y >= lastValue && !descending) {
			lastValue = y;
			continue;
		}

		// Starting descent
		if (y < lastValue && !descending) {
			descending = true;
			lastValue = y;
		}

		// Updating descending y
		else if (y < lastValue) {
			lastValue = y;
		}
	}

	return -1;
}

int SlopeBehaviourAnalyzer::extremaType(HexaPolynomialCoeff coeffs, double x_extrema, double x_from, double x_to)
{
	double neigh = Configuration::getInstance()->getInterpolationEngineParameters().extremaNeighbourhoodSize;

	double diff_from = x_extrema - x_from + 1;
	double diff_to = x_to - x_extrema + 1;

	double votes_min = 0.0;
	double votes_max = 0.0;

	if (diff_to < neigh || diff_from < neigh) {
		if (diff_from < diff_to)
			neigh = diff_from;
		else
			neigh = diff_to;
	}

	// Symmetrical check
	double x = x_extrema;
	double y_extrema = coeffs.c0 + coeffs.c1*x + coeffs.c2*x*x + coeffs.c3*x*x*x + coeffs.c4*x*x*x*x + coeffs.c5*x*x*x*x*x + coeffs.c6*x*x*x*x*x*x;
	double tot = neigh;

	for (double k = 1.0; k <= neigh; k += 1.0) {
		x = x_extrema - k;
		double yL = coeffs.c0 + coeffs.c1*x + coeffs.c2*x*x + coeffs.c3*x*x*x + coeffs.c4*x*x*x*x + coeffs.c5*x*x*x*x*x + coeffs.c6*x*x*x*x*x*x;
		x = x_extrema + k;
		double yR = coeffs.c0 + coeffs.c1*x + coeffs.c2*x*x + coeffs.c3*x*x*x + coeffs.c4*x*x*x*x + coeffs.c5*x*x*x*x*x + coeffs.c6*x*x*x*x*x*x;

		if (yL < y_extrema && yR < y_extrema) {
			votes_min += 1.0;
		}
		else if (yL > y_extrema && yR > y_extrema) {
			votes_max += 1.0;
		}
		else {
			tot -= 1.0; // Measurement failed
		}
	}

	double min_percent = votes_min/tot;
	double max_percent = votes_max/tot;

	if (min_percent >= max_percent) {
		return EXTREMA_TYPE_LOCAL_MIN;
	}
	else {
		return EXTREMA_TYPE_LOCAL_MAX;
	}
}
