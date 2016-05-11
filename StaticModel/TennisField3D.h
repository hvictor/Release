/*
 * TennisField3D.h
 *
 *  Created on: Feb 4, 2016
 *      Author: sled
 */

#ifndef TENNISFIELD3D_H_
#define TENNISFIELD3D_H_

typedef struct
{
	// 3D coordinates
	double ground_h;

	double near_L_x, near_L_z;
	double near_R_x, near_R_z;
	double far_R_x, far_R_z;
	double far_L_x, far_L_z;

	double near_half_x, near_half_z;
	double far_half_x, far_half_z;

	// 2D coordinates
	Point2f near_L, near_R;
	Point2f far_L, far_R;

	Point2f near_half, far_half;
	bool trigger;

	// Score positions
	vector<Point2f> score_positions;
	int score;
	int nets;

} TennisField_3D;


#endif /* TENNISFIELD3D_H_ */
