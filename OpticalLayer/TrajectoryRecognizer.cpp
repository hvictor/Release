
#include "TrajectoryRecognizer.h"
#include "OpticalFlowRegionRecognizer.h"
#include <math.h>
#include <iostream>

using namespace std;

extern double ax;
extern double ay;
extern int    filter_iterat;

TrajectoryRecognizer::TrajectoryRecognizer()
{
	_trajectorySequenceNumber = 0;
}

TrajectoryRecognizer::~TrajectoryRecognizer()
{

}

Trajectory *TrajectoryRecognizer::makeTrajectoryOfObject(MovingObject *mv_obj, uint16_t frameTime)
{
    if (mv_obj == NULL) return NULL;

    Trajectory *t_obj = new Trajectory();
    
    t_obj->trajectoryID = _trajectorySequenceNumber++;

    cout << "TrajectoryRecognizer::makeTrajectoryOfObject(): Sequence Number = " << _trajectorySequenceNumber << endl;

    t_obj->motionFlow.push_back(mv_obj);
    t_obj->velocityX = 0.0;
    t_obj->velocityY = 0.0;
    t_obj->accelX = 0.0;
    t_obj->accelY = 0.0;
    
    t_obj->predictedPosition.Cx = mv_obj->centerOfMotionX;
    t_obj->predictedPosition.Cy = mv_obj->centerOfMotionY;
    t_obj->predictedPosition.r  = mv_obj->radius * TRAJECTORY_OBJ_RELATED_FACTOR;
    
    t_obj->lastUpdateTime = frameTime;
    
    return t_obj;
}

// Get the quality indicator (smaller is better) for a moving object being on a trajectory
double TrajectoryRecognizer::qualityIndicatorForObjectOnTrajectory(MovingObject *mv_obj, Trajectory *traj)
{
    // EXPERIMENTAL: distance calculating object's radius
    // Compute the distance from the trajectory's predicted position
    traj->predictedPosition = CPU_TrajectoryRecognizerPredict(traj, 1);

    double dist_x = mv_obj->centerOfMotionX - traj->motionFlow[traj->motionFlow.size()-1]->centerOfMotionX;//traj->predictedPosition.Cx;   // Absolute value not important, will be squared
    double dist_y = mv_obj->centerOfMotionY - traj->motionFlow[traj->motionFlow.size()-1]->centerOfMotionY;//traj->predictedPosition.Cy;   // Absolute value not important, will be squared

    double dist = sqrt((dist_x * dist_x) + (dist_y * dist_y)) - mv_obj->radius;
    
    if (dist > traj->motionFlow[traj->motionFlow.size()-1]->radius*2.0)//traj->predictedPosition.r * 2.0)
        return -1.0;
    
    return 0.0001; //(dist / traj->predictedPosition.r);
}

MovingObject *TrajectoryRecognizer::convertOpticalFlowRegion2MovingObject(OpticalFlowRegion *r, uint16_t frameTime)
{
    if (!r) return 0;
    
    MovingObject *mv_obj = new MovingObject();

    // True object center
    double true_Cx = -1;
    double true_Cy = -1;

    double distx1, distx2;
    double disty1, disty2;
    double dist1, dist2;
    

    if (r->trueCenterFound) {
    	true_Cx = r->trueCenterX;
    	true_Cy = r->trueCenterY;

	distx1 = 50 - true_Cx;
	disty1 = 120 - true_Cy;
	dist1 = sqrt(distx1*distx1 + disty1*disty1);

	distx2 = 320-50 - true_Cx;
        disty2 = 120 - true_Cy;
        dist2 = sqrt(distx2*distx2 + disty2*disty2);
    }

    // Flow region rectangle to compute the center
    double mv_Cx = (double)r->x0 + ((double)r->x1 - (double)r->x0) / 2.0;
    double mv_Cy = (double)r->y0 + ((double)r->y1 - (double)r->y0) / 2.0;

    distx1 = 50 - mv_Cx;
    disty1 = 120 - mv_Cy;
    dist1 = sqrt(distx1*distx1 + disty1*disty1);

    distx2 = 320-50 - mv_Cx;
    disty2 = 120 - mv_Cy;
    dist2 = sqrt(distx2*distx2 + disty2*disty2);

    if (dist1 < 40 || dist2 < 40) {
	delete mv_obj;
	return NULL;
    }

    disty1 = 120-40 - mv_Cy;
    disty2 = 120-40 - mv_Cy;
    dist1 = sqrt(distx1*distx1 + disty1*disty1);
    dist2 = sqrt(distx2*distx2 + disty2*disty2);

    if (dist1 < 40 || dist2 < 40) {
        delete mv_obj;
        return NULL;
    }

    double mv_r  = sqrt(((double)r->x1 - mv_Cx) * ((double)r->x1 - mv_Cx) + ((double)r->y1 - mv_Cy) * ((double)r->y1 - mv_Cy));
    
    mv_obj->impacted 		= false;
    mv_obj->filtered		= false;
    mv_obj->trueObject		= r->trueCenterFound;

    // If the true object has been identified, use the true center of motion
    if (mv_obj->trueObject) {
    	mv_obj->centerOfMotionX = true_Cx;
    	mv_obj->centerOfMotionY = true_Cy;
    }
    // Approximation: could not identify true object, use the flow region's center as center of motion
    else {
    	mv_obj->centerOfMotionX = mv_Cx;
    	mv_obj->centerOfMotionY = mv_Cy;
    }

    mv_obj->radius          = mv_r;
    mv_obj->frameIndex      = frameTime;

    return mv_obj;
}

// Predict future trajectory behaviour
PredictedPosition TrajectoryRecognizer::CPU_TrajectoryRecognizerPredict(Trajectory *t_obj, uint16_t timeLookAhead)
{
    PredictedPosition p;
    MovingObject *lastObj = t_obj->motionFlow[t_obj->motionFlow.size() - 1];
    
    //printf("Predict(): lastObj: [%f, %f]       velocity: [%f %f]\n", lastObj->centerOfMotionX, lastObj->centerOfMotionY, t_obj->velocityX, t_obj->velocityY);

    if (t_obj->accelX == 0.0)
    	p.Cx = lastObj->centerOfMotionX + (t_obj->velocityX * (double)timeLookAhead);
    else
    	p.Cx = lastObj->centerOfMotionX + (t_obj->velocityX * (double)timeLookAhead) + (double)(timeLookAhead*timeLookAhead)/(2.0 * t_obj->accelX);
    
    if (t_obj->accelY == 0.0)
    	p.Cy = lastObj->centerOfMotionY + (t_obj->velocityY * (double)timeLookAhead);
    else
    	p.Cy = lastObj->centerOfMotionY + (t_obj->velocityY * (double)timeLookAhead) + (double)(timeLookAhead*timeLookAhead)/(2.0 * t_obj->accelY);

    // Acceptance radius of predicted position: the higher the velocity, the larger
    p.r = lastObj->radius * TRAJECTORY_OBJ_RELATED_FACTOR;

    return p;
}

vector<unsigned int> TrajectoryRecognizer::findSubTrajectories(Trajectory *t)
{
	vector<unsigned int> subTrajectories;

	for (unsigned int i = 0; i < t->motionFlow.size(); i++) {
		if (t->motionFlow[i]->impacted) {
			subTrajectories.push_back(i);
		}
	}

	return subTrajectories;
}

// Filter the trajectory: make it smoother filtering noise out
void TrajectoryRecognizer::filterTrajectory(Trajectory *t)
{
	if (t->motionFlow.size() <= 2)
		return;

	// Find sub-trajectories, i.e. trajectory sections not affected by any impact (sudden trajectory variation)
	vector<unsigned int> subTrajectories = findSubTrajectories(t);

	for (int f = 0; f < filter_iterat; f++) {
		unsigned int subTrajectoryStartIndex = 0;

		if (subTrajectories.size() == 0) {
			unsigned int subTrajectoryStopIndex = t->motionFlow.size();

			// Smooth the trajectory section between [subTrajectoryStartIndex : subTrajectoryStopIndex]
			MovingObject *startObject = t->motionFlow[subTrajectoryStartIndex];
			MovingObject *endObject = t->motionFlow[subTrajectoryStopIndex];

			double coarseDeltaX = endObject->centerOfMotionX - startObject->centerOfMotionX;
			double coarseDeltaY = endObject->centerOfMotionY - startObject->centerOfMotionY;


			// Quadratic curve fitting using low-pass filter
			for (unsigned int k = subTrajectoryStartIndex + 1; k < subTrajectoryStopIndex; k++) {
				if (!t->motionFlow[k]->filtered) {
					t->motionFlow[k]->centerOfMotionX = ax * t->motionFlow[k]->centerOfMotionX + (1.0 - ax) * t->motionFlow[k - 1]->centerOfMotionX;
					t->motionFlow[k]->centerOfMotionY = ay * t->motionFlow[k]->centerOfMotionY + (1.0 - ay) * t->motionFlow[k - 1]->centerOfMotionY;
				}
				if (f == filter_iterat - 1) t->motionFlow[k]->filtered = true;
			}

		}
		else {
			for (unsigned int i = 0; i < subTrajectories.size(); i++) {
				unsigned int subTrajectoryStopIndex = subTrajectories[i];

				// Smooth the trajectory section between [subTrajectoryStartIndex : subTrajectoryStopIndex]
				MovingObject *startObject = t->motionFlow[subTrajectoryStartIndex];
				MovingObject *endObject = t->motionFlow[subTrajectoryStopIndex];

				double coarseDeltaX = endObject->centerOfMotionX - startObject->centerOfMotionX;
				double coarseDeltaY = endObject->centerOfMotionY - startObject->centerOfMotionY;

				// Quadratic curve fitting using low-pass filter
				for (unsigned int k = subTrajectoryStartIndex + 1; k < subTrajectoryStopIndex; k++) {
					if (!t->motionFlow[k]->filtered) {
						t->motionFlow[k]->centerOfMotionX = ax * t->motionFlow[k]->centerOfMotionX + (1.0 - ax) * t->motionFlow[k - 1]->centerOfMotionX;
						t->motionFlow[k]->centerOfMotionY = ay * t->motionFlow[k]->centerOfMotionY + (1.0 - ay) * t->motionFlow[k - 1]->centerOfMotionY;
					}

					if (f == filter_iterat - 1) t->motionFlow[k]->filtered = true;
				}

				subTrajectoryStartIndex = subTrajectoryStopIndex;
			}

			// Process last trajectory section
			for (unsigned int k = subTrajectoryStartIndex + 1; k < t->motionFlow.size(); k++) {
				if (!t->motionFlow[k]->filtered) {
					t->motionFlow[k]->centerOfMotionX = ax * t->motionFlow[k]->centerOfMotionX + (1.0 - ax) * t->motionFlow[k - 1]->centerOfMotionX;
					t->motionFlow[k]->centerOfMotionY = ay * t->motionFlow[k]->centerOfMotionY + (1.0 - ay) * t->motionFlow[k - 1]->centerOfMotionY;
				}

				if (f == filter_iterat - 1) t->motionFlow[k]->filtered = true;
			}

		}
	}

	MovingObject *lastObj = t->motionFlow[t->motionFlow.size() - 1];
	MovingObject *preLastObj = t->motionFlow[t->motionFlow.size() - 2];
	t->velocityX = (lastObj->centerOfMotionX - preLastObj->centerOfMotionX) / ((lastObj->frameIndex - preLastObj->frameIndex) / (double)FPS);
	t->velocityY = (lastObj->centerOfMotionY - preLastObj->centerOfMotionY) / ((lastObj->frameIndex - preLastObj->frameIndex) / (double)FPS);
}

bool TrajectoryRecognizer::trackedObjectImpacted(double old_velocityX, double old_velocityY, double velocityX, double velocityY)
{

	double inner_prod = old_velocityX * velocityX + old_velocityY * velocityY;
	double norm_old = sqrt(old_velocityX*old_velocityX + old_velocityY*old_velocityY);
	double norm = sqrt(velocityX*velocityX + velocityY*velocityY);

	double cos_val = inner_prod / (norm * norm_old);
	double angle = acos(cos_val) * 180.0 / M_PI;
	//printf("[Angle] %f deg\n", angle);

	if ((angle > 90.0 && velocityY < 0.0) || ((old_velocityY * velocityY < 0.0) && velocityY < 0.0))
		return true;

	return false;
}

void TrajectoryRecognizer::updateTrajectory(Trajectory *traj, MovingObject *mv_obj, uint16_t frameTime)
{
    if (traj->lastUpdateTime == frameTime) {
    	printf("Trajectory already updated at this time (t = %d).\n", frameTime);
    	return;
    }

    if (traj->motionFlow.size() > 2) {
    	PredictedPosition p = CPU_TrajectoryRecognizerPredict(traj, 1);
    	printf("[Object] Real: %f, %f     Expected: %f, %f\n", mv_obj->centerOfMotionX, mv_obj->centerOfMotionY, p.Cx, p.Cy);
    }

    traj->lastRawObject = *mv_obj;
    traj->lastRawObject.trajectoryID = traj->trajectoryID;

    MovingObject *lastObj = traj->motionFlow[traj->motionFlow.size() - 1];
    
    // Update velocities: v = dx/dt
    double old_velocityX = traj->velocityX;
    double old_velocityY = traj->velocityY;

    traj->velocityX = (mv_obj->centerOfMotionX - lastObj->centerOfMotionX) / ((frameTime - traj->lastUpdateTime) / (double)FPS);
    traj->velocityY = (mv_obj->centerOfMotionY - lastObj->centerOfMotionY) / ((frameTime - traj->lastUpdateTime) / (double)FPS);
    
    // Check if velocity vector changed over a certain threshold: detect object impact with a surface
    lastObj->impacted = trackedObjectImpacted(old_velocityX, old_velocityY, traj->velocityX, traj->velocityY);

    traj->accelX = (traj->velocityX - old_velocityX) / ((frameTime - traj->lastUpdateTime) / (double)FPS);
    traj->accelY = (traj->velocityY - old_velocityY) / ((frameTime - traj->lastUpdateTime) / (double)FPS);

    // EXPERIMENTAL: marked OK
    traj->lastUpdateTime = frameTime;
    
    traj->motionFlow.push_back(mv_obj);
}

Trajectory *TrajectoryRecognizer::getTrajectoryByID(uint64_t trajectoryID)
{
	for (vector<Trajectory *>::iterator traj = trajectories.begin(); traj != trajectories.end(); ++traj) {
		if ((*traj)->trajectoryID == trajectoryID) {
			return *traj;
		}
	}

	return 0;
}

// Filter all active trajectories
void TrajectoryRecognizer::CPU_TrajectoryRecognizerFilter()
{
	for (vector<Trajectory *>::iterator traj = trajectories.begin(); traj != trajectories.end(); ++traj) {
		filterTrajectory(*traj);
	}
}

vector<Trajectory *> TrajectoryRecognizer::CPU_TrajectoryRecognizerFeed(uint16_t frameTime, vector<OpticalFlowRegion *> detectedMotionRegions)
{
    // No trajectories being computed: initialize different trajectories
    if (!trajectories.size()) {
        for (vector<OpticalFlowRegion *>::iterator it = detectedMotionRegions.begin(); it != detectedMotionRegions.end(); ++it) {
	    Trajectory *traj_ptr = makeTrajectoryOfObject(convertOpticalFlowRegion2MovingObject(*it, frameTime), frameTime);
	    if (traj_ptr != NULL)
	            trajectories.push_back(traj_ptr);
        }
        
        return trajectories;
    }
    
    // Update predicted position for all present trajectories using the current dt (extrapolated based on frameTime)
    for (vector<Trajectory *>::iterator traj = trajectories.begin(); traj != trajectories.end(); ) {
        Trajectory *t_obj = *traj;

	/* Original version *
	t_obj->predictedPosition.Cx = t_obj->motionFlow[t_obj->motionFlow.size()-1]->centerOfMotionX + t_obj->velocityX * (frameTime - t_obj->lastUpdateTime);
        t_obj->predictedPosition.Cy = t_obj->motionFlow[t_obj->motionFlow.size()-1]->centerOfMotionY + t_obj->velocityY * (frameTime - t_obj->lastUpdateTime);
	 */
        if (t_obj->motionFlow.size() > 1) {
        	printf("delta t = %d\n", frameTime - t_obj->lastUpdateTime);
        	PredictedPosition p = CPU_TrajectoryRecognizerPredict(t_obj, frameTime - t_obj->lastUpdateTime);
        	t_obj->predictedPosition = p;
        }

        if ((frameTime - t_obj->lastUpdateTime) <= TRAJECTORY_IDLE_LIFETIME) {
        	//printf("Updating time of trajectory\n");
        	//t_obj->lastUpdateTime = frameTime;
        }
        else {
        	printf("Suppressing trajectory\n");
        	delete t_obj;
        	traj = trajectories.erase(traj);
        	printf("Suppressed.\n");
        }

        if (traj != trajectories.end()) traj++;
    }

    // Some trajectories already exist: recalc them based on previous model information
    // Check first which moving objects belong to which trajectory. Objects not belonging to any trajectory originate a new trajectory.
    for (vector<OpticalFlowRegion *>::iterator it = detectedMotionRegions.begin(); it != detectedMotionRegions.end(); ++it) {
        printf("Recalc trajectories for the new object\n");
        MovingObject *mv_obj = convertOpticalFlowRegion2MovingObject(*it, frameTime);

	if (!mv_obj) continue;

        // Find to which trajectories the object is best suitable
        // The quality indicator is the ratio dist(Object, Trajectory.predictedPosition.center) / Trajectory.predictedPosition.radius
        Trajectory *most_likely_trajectory = 0;
        double best_quality_indicator = 0.0;
        
        printf("Searching best trajectory for this object\n");
        
        for (vector<Trajectory *>::iterator traj = trajectories.begin(); traj != trajectories.end(); ++traj) {
            Trajectory *t_obj = *traj;
            
            // Get quality indicator for the observed object with the observed trajectory
            double quality_indicator = qualityIndicatorForObjectOnTrajectory(mv_obj, t_obj);
            
            // If this object has a unacceptable quality indicator with this trajectory, continue
            if (quality_indicator < 0.0) {
                continue;
            }
            
            // If the quality indicator is acceptable, but this object is not yet assigned to any trajectory, just assign to this one and continue
            if (!most_likely_trajectory) {
                most_likely_trajectory = t_obj;
                best_quality_indicator = quality_indicator;
            }
            // If for this object some other trajectory is assigned, compare the quality indicators (smaller is better) and update if this is better
            else if (quality_indicator < best_quality_indicator) {
                most_likely_trajectory = t_obj;
                best_quality_indicator = quality_indicator;
            }
        }
        
        // Here, if the object has not been assigned to any trajectory, it means it is independent, and it originates a new trajectory
        if (!most_likely_trajectory) {
            printf("No trajectory compabile\n");
	    Trajectory *traj_obj = makeTrajectoryOfObject(convertOpticalFlowRegion2MovingObject(*it, frameTime), frameTime);
	    if (traj_obj != NULL)
	            trajectories.push_back(traj_obj);
        }
        // If the best trajectory for this object has been found, update it with its information
        else {
            printf("Found the best compatible trajectory\n");
            updateTrajectory(most_likely_trajectory, mv_obj, frameTime);
        }
    }
    
    CPU_TrajectoryRecognizerFilter();

    return trajectories;
}




