/*
 * FlowProcessor.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: sled
 */

#include "FlowProcessor.h"
#include "OpticalFlowRegionRecognizer.h"
#include <time.h>
#include "pyrlk_optical_flow.h"
#include "../CUDA/gpu_flow_recogu8_binary.h"

inline bool isFlowCorrect(Point2f u)
{
    return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}

vector<FlowObject> FlowProcessor_ProcessSparseFlow(Mat frame0, Mat frame1, vector<Rect> discardedImageAreas)
{
	return pyrlk_compute(frame0, frame1, discardedImageAreas);
}

vector<FlowObject> FlowProcessor_ProcessDenseFlow(const Mat_<float>& flowx, const Mat_<float>& flowy)
{
	vector<FlowObject> flowObjects;

	Mat binary(Size(FRAME_WIDTH, FRAME_HEIGHT), CV_8UC1);

	//printf("GPU recogu8 binary: start\n");
	//gpu_flow_recogu8_binary((float *)flowx.data, (float *)flowy.data, binary.data, OPTICAL_FLOW_MIN_MOTION, FRAME_WIDTH * FRAME_HEIGHT);
	//printf("GPU recogu8 binary: end\n");

	for (int y = 0; y < flowx.rows; ++y) {
		for (int x = 0; x < flowx.cols; ++x) {
			Point2f u(flowx(y, x), flowy(y, x));
			if (isFlowCorrect(u)) {
				// The point has movement within the range, update markers in the x direction
				if (sqrt(u.x * u.x + u.y * u.y) >= OPTICAL_FLOW_MIN_MOTION) {
					binary.at<uint8_t>(y, x) = 1;
				}
				else {
					binary.at<uint8_t>(y, x) = 0;
				}
			}
		}
	}

	vector< vector <Point> > contours; // Vector for storing contour
	vector< Vec4i > hierarchy;
	int largest_contour_index = 0;
	int largest_area = 0;

	findContours(binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	for(int i = 0; i< contours.size(); i++) // iterate through each contour.
	{
		Rect R = boundingRect(contours[i]);

		if (R.width < REGION_MIN_X || R.height < REGION_MIN_Y || R.width > REGION_MAX_X || R.height > REGION_MAX_Y)
			continue;

		FlowObject fw_obj;
		fw_obj.rect = R;

		flowObjects.push_back(fw_obj);

		double a= contourArea(contours[i],false);  //  Find the area of contour
		if(a>largest_area){
			largest_area=a;
			largest_contour_index=i;                //Store the index of largest contour
		}
	}

	vector<OpticalFlowRegion *> flowRegions = CPU_recognizeOpticalFlowRegions(flowx, flowy);

	for (int s = 0; s < flowRegions.size(); s++) {
		FlowObject fw_obj;
		fw_obj.x = flowRegions[s]->x0 + (flowRegions[s]->x1-flowRegions[s]->x0)/2;
		fw_obj.y = flowRegions[s]->y0 + (flowRegions[s]->y1-flowRegions[s]->y0)/2;

		fw_obj.displacement_x = flowx.at<float>(fw_obj.y, fw_obj.x) * 10.0;
		fw_obj.displacement_y = flowy.at<float>(fw_obj.y, fw_obj.x) * 10.0;


		// Estimate brightness threshold with Montecarlo method
		srand(time(0));

		double sumx = 0;
		double sumy = 0;
		int iter = 1000;
		double div = 0.0;

		for (int i = 0; i < iter; i++) {
			int rx = flowRegions[s]->x0 + (rand() % ((flowRegions[s]->x1-flowRegions[s]->x0) + 1));
			int ry = flowRegions[s]->y0 + (rand() % ((flowRegions[s]->y1-flowRegions[s]->y0) + 1));

			if (rx < OPTICAL_FLOW_MIN_MOTION || ry < OPTICAL_FLOW_MIN_MOTION)
				continue;

			sumx += flowx.at<float>(ry, rx);
			sumy += flowy.at<float>(ry, rx);

			div += 1.0;
		}

		fw_obj.displacement_x = (sumx / div) * 20.0;
		fw_obj.displacement_y = (sumy / div) * 20.0;

		flowObjects.push_back(fw_obj);
	}

	return flowObjects;
}


