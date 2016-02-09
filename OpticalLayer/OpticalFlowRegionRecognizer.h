//
//  OpticalFlowRegionRecognizer.h
//  
//
//  Created by Victor Huerlimann on 11.12.15.
//
//

#ifndef _OpticalFlowRegionRecognizer_h
#define _OpticalFlowRegionRecognizer_h

#include <vector>
#include <iterator>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp>

#define REGION_MIN_X       10
#define REGION_MIN_Y       10
#define REGION_MAX_X	   50
#define REGION_MAX_Y	   50
#define OPTICAL_FLOW_MIN_MOTION 3.0

using namespace cv;

typedef struct
{
    unsigned int x0;
    unsigned int x1;
    unsigned int y0;
    unsigned int y1;
    
    unsigned int updated_at_row;

    bool trueCenterFound;

    unsigned int trueCenterX;
    unsigned int trueCenterY;
} OpticalFlowRegion;

typedef struct
{
	unsigned int xmin;
	unsigned int xmax;
	unsigned int ymin;
	unsigned int ymax;
} ObjectEstimationCandidate;

std::vector<OpticalFlowRegion *> CPU_recognizeOpticalFlowRegions(const Mat_<float>& flowx, const Mat_<float>& flowy);
// True Object Estimation
void recognizeTrueRegionCenter(OpticalFlowRegion *r, Mat immutableFrame);

#endif
