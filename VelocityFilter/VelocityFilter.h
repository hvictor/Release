/*
 * VelocityFilter.h
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#ifndef VELOCITYFILTER_H_
#define VELOCITYFILTER_H_

#include "../OpticalLayer/StatefulObjectFilter.h"
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

using namespace std;

class VelocityFilter {
public:
	VelocityFilter();
	virtual ~VelocityFilter();
	static void correctVelocityVectors(StateRelatedTable *statesTable);
};

#endif /* VELOCITYFILTER_H_ */
