//
//  OpticalFlowRegionRecognizer.c
//  
//
//  Created by Victor Huerlimann on 11.12.15.
//
//

#include "OpticalFlowRegionRecognizer.h"
#include "../Configuration/configs.h"

// CUDA accelerated library
#include "../CUDA/gpu_contrastu8.h"

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace std;
using namespace cv;
using namespace cv::gpu;

OpticalFlowRegion *makeOpticalFlowRegion(unsigned int x_start, unsigned int x_end, unsigned int y_start, unsigned int y_end)
{
    OpticalFlowRegion *r = new OpticalFlowRegion();
    
    r->x0 = x_start;
    r->x1 = x_end;
    r->y0 = y_start;
    r->y1 = y_end;
    r->updated_at_row = 0;
    
    r->trueCenterFound = false;

    return r;
}

OpticalFlowRegion *cloneAndDelete(OpticalFlowRegion *r)
{
    OpticalFlowRegion *res = new OpticalFlowRegion();
    *res = *r;
    delete r;
    return res;
}

inline bool xSpanConnectedToRegion(unsigned int x0, unsigned int x1, OpticalFlowRegion *r)
{
    if (x1 < (r->x0 - 1) || x0 > (r->x1 + 1))
        return false;
    return true;
}

inline void updatePendingRegion(OpticalFlowRegion *r, unsigned int x0, unsigned int x1, unsigned int y)
{
    if (x0 < r->x0) r->x0 = x0;
    if (x1 > r->x1) r->x1 = x1;
    r->updated_at_row = y;
}

inline bool isFlowCorrect(Point2f u)
{
    return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}

inline bool valueInRange(int value, int min, int max)
{
    return (value >= min-CFG_OPTICAL_LAYER_REGION_OVERLAP_PIXEL_TOLERANCE) && (value <= max+CFG_OPTICAL_LAYER_REGION_OVERLAP_PIXEL_TOLERANCE);
}

bool regionOverlap(OpticalFlowRegion *A, OpticalFlowRegion *B)
{
    bool xOverlap = valueInRange(A->x0, B->x0, B->x1) || valueInRange(B->x0, A->x0, A->x1);
    bool yOverlap = valueInRange(A->y0, B->y0, B->y1) || valueInRange(B->y0, A->y0, A->y1);
    
    return xOverlap && yOverlap;
}

OpticalFlowRegion *mergeRegions(vector<OpticalFlowRegion *> regions)
{
    unsigned int xmin = regions[0]->x0;
    unsigned int xmax = regions[0]->x1;
    unsigned int ymin = regions[0]->y0;
    unsigned int ymax = regions[0]->y1;
    
    for (int i = 1; i < regions.size(); i++) {
        if (regions[i]->x0 < xmin) xmin = regions[i]->x0;
        if (regions[i]->x1 > xmax) xmax = regions[i]->x1;
        if (regions[i]->y0 < ymin) ymin = regions[i]->y0;
        if (regions[i]->y1 > ymax) ymax = regions[i]->y1;
        
        delete regions[i];
    }
    
    return makeOpticalFlowRegion(xmin, xmax, ymin, ymax);
}

vector<OpticalFlowRegion *> CPU_recognizeOpticalFlowRegions(const Mat_<float>& flowx, const Mat_<float>& flowy)
{
    unsigned int region_min_x;
    unsigned int region_max_x;
    unsigned int region_min_y;
    unsigned int region_max_y;

    vector<OpticalFlowRegion *> recognized;
    vector<OpticalFlowRegion *> pending;

    for (int y = 0; y < flowx.rows; ++y)
    {
        unsigned int rmin = 0;
        unsigned int rmax = 0;
        bool region_open_x = false;
        
        // Cycle pending regions: the ones that have not been updated in the last iteration are done
        if (y > 0) {
            for (vector<OpticalFlowRegion *>::iterator it = pending.begin(); it != pending.end(); ) {
                OpticalFlowRegion *ptr = *it;
                if (ptr->updated_at_row < (y - 1)) {
                    //printf("[y = %d] pending region [%d, %d] at y=%d is ready!\n", y, ptr->x0, ptr->x1, ptr->updated_at_row);
                    OpticalFlowRegion *tmp = new OpticalFlowRegion();
                    *tmp = *ptr;
                    tmp->y1 = tmp->updated_at_row;
                    it = pending.erase(it);
                    
                    // Filter out regions with unacceptable size
                    if (abs(tmp->x1 - tmp->x0) > REGION_MIN_X && abs(tmp->y1 - tmp->y0) > REGION_MIN_Y && 
			abs(tmp->x1 - tmp->x0) < REGION_MAX_X && abs(tmp->y1 - tmp->y0) < REGION_MAX_Y)
                        recognized.push_back(tmp);
                }
                
                 if (it != pending.end()) it++;
            }
        }
        
        // In the current row, open new encountered regions and close pending regions that have not been updated
        for (int x = 0; x < flowx.cols; ++x)
        {
            Point2f u(flowx(y, x), flowy(y, x));
            
            if (isFlowCorrect(u)) {
                
                // The point has movement within the range, update markers in the x direction
                if (sqrt(u.x * u.x + u.y * u.y) >= OPTICAL_FLOW_MIN_MOTION) {
                    if (!region_open_x) {
                        region_open_x = true;
                        rmin = x;
                        //printf("[y = %d] started scanning at x=%d\n", y, rmin);
                    }
                    // If it is the last pixel of this row, close an eventually open region
                    else if (x == flowx.cols-1) {
                        //printf("[y = %d] reached the last x=%d\n", y, x);
                        region_open_x = false;
                        rmax = x;
                        
                        bool updated = false;
                        // Check pending regions for updating
                        for (vector<OpticalFlowRegion *>::iterator it = pending.begin(); it != pending.end(); ++it) {
                            // If the current x span is connected to a pending region, update it with this information
                            OpticalFlowRegion *pendingRegion = *it;
                            if (xSpanConnectedToRegion(rmin, rmax, pendingRegion)) {
                                updatePendingRegion(pendingRegion, rmin, rmax, y);
                                updated = true;
                            }
                        }
                        
                        // If no pending regions can be updated with this information, create a new pending region, to be then finalized in the y direction
                        if (!updated) {
                            OpticalFlowRegion *r = makeOpticalFlowRegion(rmin, rmax, y, y);
                            r->updated_at_row = y; // Forced for copying
                            pending.push_back(r);
                        }
                    }
                }
                // The point has no acceptable movement, delimit an eventually open region
                else {
                    if (region_open_x) {
                        //printf("[y = %d] insufficient flow detected at x=%d\n", y, x);
                        region_open_x = false;
                        rmax = x;
                        
                        bool updated = false;
                        // Check pending regions for updating
                        for (vector<OpticalFlowRegion *>::iterator it = pending.begin(); it != pending.end(); ++it) {
                            // If the current x span is connected to a pending region, update it with this information
                            OpticalFlowRegion *pendingRegion = *it;
                            if (xSpanConnectedToRegion(rmin, rmax, pendingRegion)) {
                                //printf("[y = %d] updating region [%d, %d] with current rmin=%d, rmax=%d\n", y, pendingRegion->x0, pendingRegion->x1, rmin, rmax);
                                updatePendingRegion(pendingRegion, rmin, rmax, y);
                                //printf("[y = %d] updated: [%d, %d]\n", y, pendingRegion->x0, pendingRegion->x1);
                                
                                updated = true;
                            }
                        }
                        
                        // If no pending regions can be updated with this information, create a new pending region, to be then finalized in the y direction
                        if (!updated) {
                            OpticalFlowRegion *r = makeOpticalFlowRegion(rmin, rmax, y, y);
                            r->updated_at_row = y; // Forced for copying
                            pending.push_back(r);
                            //printf("[y = %d] new pending region [%d, %d] (closed for independency)\n", y, rmin, rmax);
                        }
                    }
                }
            }
        }
    }
    
    vector<OpticalFlowRegion *> working = recognized;
    
    while (true) {
        
        bool merged_some_regions = false;
        
        for (vector<OpticalFlowRegion *>::iterator it = working.begin(); it != working.end(); ) {
            OpticalFlowRegion *i = *it;
            vector<OpticalFlowRegion *> contacts;
            
            // Building contacting regions list
            for (vector<OpticalFlowRegion *>::iterator jt = it+1; jt != working.end(); ) {
                OpticalFlowRegion *j = *jt;
                
                if (regionOverlap(i, j)) {
                    contacts.push_back(j);
                    jt = working.erase(jt);
                }
                
                if (jt != working.end()) jt++;
            }
            
            // Merge region i with all contacting regions, and replace this new region in the original list
            if (contacts.size() > 0) {
                contacts.push_back(i);
                OpticalFlowRegion *m = mergeRegions(contacts);
                // Here only m exists: memory of i and <contacts> has been freed
                it = working.erase(it);
                working.push_back(m);
                merged_some_regions = true;
            }
            
            if (it != working.end()) it++;
        }
        
        if (!merged_some_regions) break;
    
    }

    return working;
}

// Raise the contrast for the given pixel value
Vec3b contrastOpticalFlowRegion(Vec3b v, uint8_t threshold, uint8_t deltaValue)
{
	Vec3b valCopy;
	
	for (int i = 0; i < 3; i++) {
		valCopy.val[i] = v.val[i];

		if (valCopy.val[i] >= threshold) {
			if (valCopy.val[i] + deltaValue >= 255) {
				valCopy.val[i] = 255;
			}
			else {
				valCopy.val[i] += deltaValue;
			}
		}
		else {
			if (valCopy.val[i] - deltaValue <= 0) {
				valCopy.val[i] = 0;
			}
			else {
				valCopy.val[i] -= deltaValue;
			}
		}
	}

	return valCopy;
}

// Extend Optical Flow Region
void extendOpticalFlowRegion(OpticalFlowRegion *r, int extensionDeltaX, int extensionDeltaY, int xMax, int yMax)
{
	int x0, x1, y0, y1;

	x0 = r->x0;
	x1 = r->x1;
	y0 = r->y0;
	y1 = r->y1;

	printf("extendOpticalFlowRegion(): r: x=[%d-%d] y=[%d-%d]     extensionDeltaX = %d, extensionDeltaY = %d\n", x0, x1, y0, y1, extensionDeltaX, extensionDeltaY);
	// Extend X to the left
	x0 -= extensionDeltaX;
	x1 += extensionDeltaX;

	y0 -= extensionDeltaY;
	y1 += extensionDeltaY;

	if (x0 < 0) 	x0 = 0;
	if (x0 > xMax) 	x0 = xMax;
	if (x1 < 0)		x1 = 0;
	if (x1 > xMax)	x1 = xMax;

	if (y0 < 0) 	y0 = 0;
	if (y0 > yMax) 	y0 = yMax;
	if (y1 < 0)		y1 = 0;
	if (y1 > yMax)	y1 = yMax;

	r->x0 = x0;
	r->x1 = x1;
	r->y0 = y0;
	r->y1 = y1;

	printf("extendOpticalFlowRegion(): output: x=[%d, %d] y=[%d, %d]\n", r->x0, r->x1, r->y0, r->y1);
}

// True Object Estimation
void recognizeTrueRegionCenter(OpticalFlowRegion *r, Mat immutableFrame)
{
	Mat frame = immutableFrame.clone();

	int extensionDeltaX = 0.0;
	int extensionDeltaY = 0.0;

	extensionDeltaX = (r->x1 - r->x0)/4;
	extensionDeltaY = (r->y1 - r->y0)/4;

	// Search boundaries
	int x0 = (int)r->x0;
	int y0 = (int)r->y0;
	int x1 = (int)r->x1;
	int y1 = (int)r->y1;

	// Brightness thresholds
	uint8_t thresh, thresh_min, thresh_max;
	thresh_min = 255;
	thresh_max = 0;
	
	// Currently detected True Object boundaries
	bool firstObjectDetection = true;
	int largetstObjectX0 = 0;
	int largetstObjectX1 = 0;
	int largetstObjectY0 = 0;
	int largetstObjectY1 = 0;

	for (int k = 0; k < CFG_OPTICAL_LAYER_REGION_EXTENSION_RETRIES; k++) {

		bool detection_open = false;

		int xmin = -1;
		int ymin = -1;
		int xmax = -1;
		int ymax = -1;

		srand(time(0));

		Mat roi = frame(Range(r->y0, r->y1), Range(r->x0, r->x1));

		// Below the retry-threshold, apply Contrasting and Dilation
		// This has as effect to brighten and broaden the object
		if (k < CFG_OPTICAL_LAYER_CONTRAST_DILATE_RETRIES) {

			// Ignore eventually red region marker
			int xIntervalSize = (int)(r->x1 - r->x0) - 2;
			int yIntervalSize = (int)(r->y1 - r->y0) - 2;

			// Estimate brightness threshold with Montecarlo method
			for (int i = 0; i < CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_MONTECARLO_ITERAT; i++) {
				int rx = (r->x0 + 1) + (rand() % (xIntervalSize + 1));
				int ry = (r->y0 + 1) + (rand() % (yIntervalSize + 1));

				Vec3b v = frame.at<Vec3b>(Point(rx, ry));

				if (v.val[0] > thresh_max) thresh_max = v.val[0];
				if (v.val[0] < thresh_min) thresh_min = v.val[0];
			}

			thresh = thresh_max; //(thresh_max - thresh_min) / 2;

			// Contrast Region
#ifndef CFG_USE_GPU
			for (int y = r->y0; y <= r->y1; y++) {
				for (int x = r->x0; x <= r->x1; x++) {
					Vec3b value;

					// Local Contrast
					Vec3b readValue = frame.at<Vec3b>(Point(x, y));
					value = contrastOpticalFlowRegion(readValue, thresh, 30);
					frame.at<Vec3b>(Point(x, y)) = value;
				}
			}
#else
			Mat frameRegionRef = frame(Range(r->y0, r->y1), Range(r->x0, r->x1));
			unsigned int N = frameRegionRef.rows * frameRegionRef.cols;
			size_t sz_mem8u = N * sizeof(unsigned char);
			unsigned char *u8data	= (unsigned char *)frameRegionRef.data;
			unsigned char *u8res  	= gpu_contrastu8(u8data, u8res, , thresh, 30)
			memcpy((void *)frameRegionRef.data, (void *)u8res, sz_mem8u);
#endif

			// GPU dilation kernel
			Mat kernel = getStructuringElement(
					MORPH_ELLIPSE,
					Size(2 * CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_MORPH_DILAT_KERNEL_SIZE + 1, 2 * CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_MORPH_DILAT_KERNEL_SIZE + 1),
					Point(CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_MORPH_DILAT_KERNEL_SIZE, CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_MORPH_DILAT_KERNEL_SIZE));

			// Upload data to device memory
			GpuMat d_roi(roi);
			GpuMat d_dst;

			// Compute dilation kernel on GPU device
			gpu::dilate(d_roi, d_dst, kernel);

			// Download dilation results from device memory to host memory
			d_dst.download(roi);
		}

		// Analyze the region
		int xminPrevRow;
		int xmaxPrevRow;

		vector<ObjectEstimationCandidate> objEstimat;

		for (int y = y0; y <= y1; y++) {

			bool detectedValuesOverThresholdInRow = false;

			for (int x = x0; x <= x1; x++) {

				Vec3b value = frame.at<Vec3b>(Point(x, y));

				if (value.val[0] >= thresh)
				{
					detectedValuesOverThresholdInRow = true;

					if (!detection_open) {
						detection_open = true;
						xmin = x;
						xmax = x;
						ymin = y;
						ymax = y;
						continue;
					}

					if (x < xmin) {
						xmin = x;
					}
					if (x > xmax) {
						xmax = x;
					}
					if (y < ymin) {
						ymin = y;
					}
					if (y > ymax) {
						ymax = y;
					}
				}
			}

			// If no value over threshold was detected in the current row, close the open detection
			if (!detectedValuesOverThresholdInRow) {
				detection_open = false;

				// Store the detected object for future comparison
				if (ymin > y0) {
					ObjectEstimationCandidate obj_est;
					obj_est.xmin = xmin;
					obj_est.xmax = xmax;
					obj_est.ymin = ymin;
					obj_est.ymax = ymax;
					objEstimat.push_back(obj_est);
				}

				xmin = -1;
				ymin = -1;
				xmax = -1;
				ymax = -1;
			}

			// If a [xmin - xmax] range is available on this row, but does not x-overlap, close the open detection and start a new one
			else {
				// Not overlapping
				if (	(xmax < xminPrevRow - CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_OVERLAP_PIXEL_TOLERANCE) ||
						(xmin > xmaxPrevRow + CFG_OPTICAL_LAYER_OBJECT_ESTIMAT_OVERLAP_PIXEL_TOLERANCE))
				{
					detection_open = false;

					if (ymin > y0) {
						ObjectEstimationCandidate obj_est;
						obj_est.xmin = xmin;
						obj_est.xmax = xmax;
						obj_est.ymin = ymin;
						obj_est.ymax = ymax;
						objEstimat.push_back(obj_est);
					}
				}
			}

			xminPrevRow = xmin;
			xmaxPrevRow = xmax;
		}

		for (int w = 0; w < objEstimat.size(); w++) {

			int xmin = objEstimat[w].xmin;
			int xmax = objEstimat[w].xmax;
			int ymin = objEstimat[w].ymin;
			int ymax = objEstimat[w].ymax;

			if ((xmin > -1 && xmax > -1) &&
					(ymin > -1 && ymax > -1) &&

					(xmax - xmin) >= CFG_OPTICAL_LAYER_TRUEOBJECT_PIXEL_WIDTH_MIN &&
					(ymax - ymin) >= CFG_OPTICAL_LAYER_TRUEOBJECT_PIXEL_HEIGHT_MIN)
			{
				double eccentricity = fabsl((ymax-ymin)/(xmax-xmin) - 1.0);

				// Eventually update largest detected object
				if (	(firstObjectDetection) ||
						( ((xmax - xmin) * (ymax-ymin) >  (largetstObjectX1 - largetstObjectX0) * (largetstObjectY1 - largetstObjectY0)) &&
						  (eccentricity <= CFG_OPTICAL_LAYER_TRUEOBJECT_ECCENTRICITY_EPS)
						)
				) 
				{
					firstObjectDetection = false;
					// Current object is larger than the previously detected one: updating
					largetstObjectX0 = xmin;
					largetstObjectX1 = xmax;
					largetstObjectY0 = ymin;
					largetstObjectY1 = ymax;
				}

				r->trueCenterX = largetstObjectX0 + (largetstObjectX1 - largetstObjectX0)/2;
				r->trueCenterY = largetstObjectY0 + (largetstObjectY1 - largetstObjectY0)/2;
				r->trueCenterFound = true;
				printf("x=[%d, %d] y=[%d, %d]     true=(%d, %d)   value over %d, xWidth: %d yWidth: %d\n", xmin, xmax, ymin, ymax, r->trueCenterX, r->trueCenterY, thresh, xmax-xmin, ymax-ymin);
			}

		}

	}
	
	rectangle(frame, Point(largetstObjectX0, largetstObjectY0), Point(largetstObjectX1, largetstObjectY1), Scalar(255, 0, 0), 1);
}
