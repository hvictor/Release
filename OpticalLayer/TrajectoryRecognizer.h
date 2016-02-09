#ifndef ____TrajectoryRecognizer__
#define ____TrajectoryRecognizer__

#include "OpticalFlowRegionRecognizer.h"

#include <stdio.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdint.h>

using namespace std;

#define FPS                                1
#define TRAJECTORY_OBJ_RELATED_FACTOR      1.5
#define TRAJECTORY_IDLE_LIFETIME           6

typedef struct
{
    double Cx;
    double Cy;
    double r;
} PredictedPosition;

typedef struct
{
    double    centerOfMotionX;
    double    centerOfMotionY;
    double    radius;
    
    bool 	  impacted;
    bool	  filtered;
    bool	  trueObject;

    uint16_t    frameIndex;

    uint64_t	trajectoryID;
} MovingObject;

typedef struct
{
    vector<MovingObject *> motionFlow;
    
    double velocityX;
    double velocityY;
    
    double accelX;
    double accelY;

    PredictedPosition predictedPosition;
    
    uint16_t lastUpdateTime;

    MovingObject lastRawObject;

    uint64_t trajectoryID;
} Trajectory;

class TrajectoryRecognizer
{
public:
	TrajectoryRecognizer();
	virtual ~TrajectoryRecognizer();

	// Feed the recognizer
	vector<Trajectory *> CPU_TrajectoryRecognizerFeed(uint16_t frameTime, vector<OpticalFlowRegion *> detectedMotionRegions);

	// Filter all active trajectories
	void CPU_TrajectoryRecognizerFilter();

	// Predict future trajectory behaviour
	PredictedPosition CPU_TrajectoryRecognizerPredict(Trajectory *t_obj, uint16_t timeLookAhead);
	Trajectory *getTrajectoryByID(uint64_t trajectoryID);
private:
	vector<Trajectory *> trajectories;
	Trajectory *makeTrajectoryOfObject(MovingObject *mv_obj, uint16_t frameTime);
	double qualityIndicatorForObjectOnTrajectory(MovingObject *mv_obj, Trajectory *traj);
	MovingObject *convertOpticalFlowRegion2MovingObject(OpticalFlowRegion *r, uint16_t frameTime);
	vector<unsigned int> findSubTrajectories(Trajectory *t);
	void filterTrajectory(Trajectory *t);
	bool trackedObjectImpacted(double old_velocityX, double old_velocityY, double velocityX, double velocityY);
	void updateTrajectory(Trajectory *traj, MovingObject *mv_obj, uint16_t frameTime);

	uint64_t _trajectorySequenceNumber;
};


#endif /* defined(____TrajectoryRecognizer__) */
