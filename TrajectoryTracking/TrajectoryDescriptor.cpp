/*
 * TrajectoryDescriptor.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: sled
 */

#include "TrajectoryDescriptor.h"

TrajectoryDescriptor::TrajectoryDescriptor(int ID)
{
	this->ID = ID;
}

TrajectoryDescriptor::~TrajectoryDescriptor()
{
}

int TrajectoryDescriptor::getID()
{
	return ID;
}

vector<TrajectorySection *> TrajectoryDescriptor::getTrajectorySections()
{
	return trajectorySections;
}

void TrajectoryDescriptor::update(CubicPolynomialCoeff coeffs, double xFrom, double xTo)
{
	// Descriptor is empty
	if (!trajectorySections.size()) {
		TrajectorySection *s = new TrajectorySection();

		s->coeffs = coeffs;
		s->x_from = xFrom;
		s->x_to = xTo;

		trajectorySections.push_back(s);
	}

	// Update only last section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	lastSection->coeffs = coeffs;
	lastSection->x_from = xFrom;
	lastSection->x_to = xTo;

	// Optimize: the last trajectory section could be splitted into two more precisely
	// approximating polynomials
	optimize();
}

void TrajectoryDescriptor::update(HexaPolynomialCoeff coeffs, double xFrom, double xTo)
{
	// Descriptor is empty
	if (!trajectorySections.size()) {
		TrajectorySection *s = new TrajectorySection();

		s->hexa_coeffs = coeffs;
		s->x_from = xFrom;
		s->x_to = xTo;

		trajectorySections.push_back(s);
	}

	// Update only last section
	TrajectorySection *lastSection = trajectorySections[trajectorySections.size() - 1];
	lastSection->coeffs = coeffs;
	lastSection->x_from = xFrom;
	lastSection->x_to = xTo;

	// Optimize: the last trajectory section could be splitted into two more precisely
	// approximating polynomials
	optimize();
}

void TrajectoryDescriptor::optimize()
{

}
