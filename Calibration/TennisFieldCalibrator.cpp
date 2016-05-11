/*
 * TennisFieldCalibrator.cpp
 *
 *  Created on: Jan 29, 2016
 *      Author: sled
 */

#include "TennisFieldCalibrator.h"

void TennisFieldCalibrator::renderCalibrationWindow(Mat calibrationFrame, double h, double basis_min, double basis_maj, double h_displacement)
{
	Point center = Point(calibrationFrame.cols/2, calibrationFrame.rows/2);

	Point bottomLeft(center.x-(basis_maj/2), center.y+(h/2)+h_displacement);
	Point bottomRight(center.x+(basis_maj/2), center.y+(h/2)+h_displacement);
	Point topLeft(center.x-(basis_min/2), center.y-(h/2)+h_displacement);
	Point topRight(center.x+(basis_min/2), center.y-(h/2)+h_displacement);

	line(calibrationFrame, bottomLeft, bottomRight, Scalar(0, 255, 0), 2);
	line(calibrationFrame, bottomRight, topRight, Scalar(0, 255, 0), 2);
	line(calibrationFrame, topRight, topLeft, Scalar(0, 255, 0), 2);
	line(calibrationFrame, topLeft, bottomLeft, Scalar(0, 255, 0), 2);

	char buffer[300];
	sprintf(buffer, "PARAM: h=%g m=%g M=%g h_displacement=%g", h, basis_min, basis_maj, h_displacement);
	putText(calibrationFrame, buffer, cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1.0, cvScalar(50, 205, 50), 1, CV_AA);
}

void TennisFieldCalibrator::setPerimetralCones(PerimetralConeSet4 cone_set)
{
	cones = cone_set;
}

void TennisFieldCalibrator::renderCalibrationWindow(Mat calibrationFrame, CalibrationWindow *calibrationWindow)
{
	char buffer[300];
	sprintf(buffer, "CALIB WIN: bL(%g,%g) bR(%g,%g) tR(%g,%g) tL(%g,%g)",
			calibrationWindow->bottomLeft.x, calibrationWindow->bottomLeft.y,
			calibrationWindow->bottomRight.x, calibrationWindow->bottomRight.y,
			calibrationWindow->topRight.x, calibrationWindow->topRight.y,
			calibrationWindow->topLeft.x, calibrationWindow->topLeft.y);
	putText(calibrationFrame, buffer, cvPoint(10,30), FONT_HERSHEY_SIMPLEX, 1.0, cvScalar(50, 205, 50), 1, CV_AA);

	line(calibrationFrame, calibrationWindow->bottomLeft, calibrationWindow->bottomRight, Scalar(0, 255, 0), 2);
	line(calibrationFrame, calibrationWindow->bottomRight, calibrationWindow->topRight, Scalar(0, 255, 0), 2);
	line(calibrationFrame, calibrationWindow->topRight, calibrationWindow->topLeft, Scalar(0, 255, 0), 2);
	line(calibrationFrame, calibrationWindow->topLeft, calibrationWindow->bottomLeft, Scalar(0, 255, 0), 2);
}

void TennisFieldCalibrator::setCalibrationWindow(Point2f center, double h, double basis_min, double basis_maj, double h_displacement)
{
	calibrationWindow->bottomLeft = Point2f(center.x-basis_maj/2, center.y+(h/2)+h_displacement);
	calibrationWindow->bottomRight = Point2f(center.x+basis_maj/2, center.y+(h/2)+h_displacement);
	calibrationWindow->topLeft = Point2f(center.x-basis_min/2, center.y-(h/2)+h_displacement);
	calibrationWindow->topRight = Point2f(center.x+basis_min/2, center.y-(h/2)+h_displacement);
}

void TennisFieldCalibrator::regulateVisualCalibrationWindow(Mat calibrationFrame)
{
	// Define initial scanning shape
	double h = 190;
	double basis_min = 100;
	double basis_maj = 200;
	double h_displacement = 0.0;

	int counter = 0;
	char fn[300];

	while (h < calibrationFrame.rows * 0.8) {
		// TODO disable disk write
		sprintf(fn, "/tmp/calib-out-%04d.png", counter++);
		Mat frameCopy = calibrationFrame.clone();
		renderCalibrationWindow(frameCopy, h, basis_min, basis_maj, h_displacement);
		imwrite(fn, frameCopy);
		h += 10;
		//frameCopy.deallocate();
	}

	while (basis_min < calibrationFrame.cols * 0.4) {
		// TODO disable disk write
		sprintf(fn, "/tmp/calib-out-%04d.png", counter++);
		Mat frameCopy = calibrationFrame.clone();
		renderCalibrationWindow(frameCopy, h, basis_min, basis_maj, h_displacement);
		imwrite(fn, frameCopy);
		basis_min += 10;
		//frameCopy.deallocate();
	}

	while (basis_maj < calibrationFrame.cols * 0.8) {
		// TODO disable disk write
		sprintf(fn, "/tmp/calib-out-%04d.png", counter++);
		Mat frameCopy = calibrationFrame.clone();
		renderCalibrationWindow(frameCopy, h, basis_min, basis_maj, h_displacement);
		imwrite(fn, frameCopy);
		basis_maj += 10;
		//frameCopy.deallocate();
	}

	while ((calibrationFrame.rows/2-h/2) + h_displacement > 0) {
		Mat frameCopy = calibrationFrame.clone();
		h_displacement -= 10;
		renderCalibrationWindow(frameCopy, h, basis_min, basis_maj, h_displacement);
		sprintf(fn, "/tmp/calib-out-%04d.png", counter++);
		imwrite(fn, frameCopy);
		//frameCopy.deallocate();
	}

	h_displacement = 0;

	while ((calibrationFrame.rows/2+h/2) + h_displacement < calibrationFrame.rows) {
		Mat frameCopy = calibrationFrame.clone();
		h_displacement += 10;
		renderCalibrationWindow(frameCopy, h, basis_min, basis_maj, h_displacement);
		sprintf(fn, "/tmp/calib-out-%04d.png", counter++);
		imwrite(fn, frameCopy);
	}
}

double dist(Point p0, Point p1)
{
	double deltax = (double)(p1.x - p0.x);
	double deltay = (double)(p1.y - p0.y);

	return sqrt(deltax*deltax + deltay*deltay);
}

// Find extreme point in a set of points
// Not minimizing a variable means maximizing it
Point TennisFieldCalibrator::findClosestPoint(vector<Point> pts, Point p)
{
	Point result = pts[0];
	double last_dist = dist(pts[0], p);

	for (int i = 1; i < pts.size(); i++) {
		if (dist(pts[i], p) < last_dist) {
			last_dist = dist(pts[i], p);
			result = pts[i];
		}
	}

	return result;
}

TennisFieldDelimiter *TennisFieldCalibrator::computeConeDelimitedStaticModel(vector<Point> intersPts)
{
	TennisFieldDelimiter *retval = new TennisFieldDelimiter();
	Point2f bottomLeft, bottomRight;
	Point2f topLeft, topRight;

	double h = cones.vertex_bottomLeft.y - cones.vertex_topLeft.y;
	double centerX = cones.vertex_topLeft.x + (cones.vertex_topRight.x - cones.vertex_topLeft.x)/2;

	///
	/// Top Left
	///
	// Define Calibration Window Quadrant
	// Line between topLeft and bottomLeft
	double m = (cones.vertex_topLeft.y - cones.vertex_bottomLeft.y) / (cones.vertex_topLeft.x - cones.vertex_bottomLeft.x);
	double q = cones.vertex_topLeft.y - m * cones.vertex_topLeft.x;

	// Find x for y = h/2
	// y = mx + q ---> x = (y - q)/m
	double y = cones.vertex_topLeft.y + h/2;
	double x = (y - q)/m;
	topLeft = cones.vertex_topLeft;
	topRight = Point2f(centerX, topLeft.y);
	bottomLeft = Point2f(x, y);
	bottomRight = Point2f(centerX, y);

	CalibrationWindow *w1 = new CalibrationWindow();

	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;

	// Filter points with current sub-window
	vector<Point> filtered = w1->filterPoints(intersPts);
	retval->topLeft = findClosestPoint(filtered, cones.vertex_topLeft);

	///
	/// Bottom left
	///
	topLeft = bottomLeft;
	bottomLeft = cones.vertex_bottomLeft;
	topRight = topRight = Point2f(centerX, topLeft.y);
	bottomRight = Point2f(centerX, bottomLeft.y);
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->bottomLeft = findClosestPoint(filtered, cones.vertex_bottomLeft);

	//
	// Bottom right
	//
	m = (cones.vertex_topRight.y - cones.vertex_bottomRight.y) / (cones.vertex_topRight.x - cones.vertex_bottomRight.x);
	q = cones.vertex_topRight.y - m * cones.vertex_topRight.x;
	y = cones.vertex_topRight.y + h/2;
	x = (y - q)/m;
	topLeft = topRight;
	bottomLeft = bottomRight;
	bottomRight = cones.vertex_bottomRight;
	topRight = Point2f(x, y);
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->bottomRight = findClosestPoint(filtered, cones.vertex_bottomRight);

	//
	// Top right
	//
	bottomLeft = topLeft;
	topLeft = Point2f(centerX, cones.vertex_topLeft.y);
	bottomRight = topRight;
	topRight = cones.vertex_topRight;
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->topRight = findClosestPoint(filtered, cones.vertex_topRight);

	delete w1;

	return retval;
}

TennisFieldDelimiter *TennisFieldCalibrator::computeTennisFieldDelimiter(Mat calibrationFrame, vector<Point> intersPts, CalibrationWindow *calibWnd)
{
	TennisFieldDelimiter *retval = new TennisFieldDelimiter();
	Point2f bottomLeft, bottomRight;
	Point2f topLeft, topRight;

	double h = calibWnd->bottomLeft.y - calibWnd->topLeft.y;
	double centerX = calibWnd->topLeft.x + (calibWnd->topRight.x - calibWnd->topLeft.x)/2;

	///
	/// Top Left
	///
	// Define Calibration Window Quadrant
	// Line between topLeft and bottomLeft
	double m = (calibWnd->topLeft.y - calibWnd->bottomLeft.y) / (calibWnd->topLeft.x - calibWnd->bottomLeft.x);
	double q = calibWnd->topLeft.y - m * calibWnd->topLeft.x;

	// Find x for y = h/2
	// y = mx + q ---> x = (y - q)/m
	double y = calibWnd->topLeft.y + h/2;
	double x = (y - q)/m;
	topLeft = calibWnd->topLeft;
	topRight = Point2f(centerX, topLeft.y);
	bottomLeft = Point2f(x, y);
	bottomRight = Point2f(centerX, y);

	CalibrationWindow *w1 = new CalibrationWindow();

	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;

	// Filter points with current sub-window
	vector<Point> filtered = w1->filterPoints(intersPts);
	retval->topLeft = findClosestPoint(filtered, calibWnd->topLeft);

	///
	/// Bottom left
	///
	topLeft = bottomLeft;
	bottomLeft = calibWnd->bottomLeft;
	topRight = topRight = Point2f(centerX, topLeft.y);
	bottomRight = Point2f(centerX, bottomLeft.y);
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;
	renderCalibrationWindow(calibrationFrame, w1);

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->bottomLeft = findClosestPoint(filtered, calibWnd->bottomLeft);

	//
	// Bottom right
	//
	m = (calibWnd->topRight.y - calibWnd->bottomRight.y) / (calibWnd->topRight.x - calibWnd->bottomRight.x);
	q = calibWnd->topRight.y - m * calibWnd->topRight.x;
	y = calibWnd->topRight.y + h/2;
	x = (y - q)/m;
	topLeft = topRight;
	bottomLeft = bottomRight;
	bottomRight = calibWnd->bottomRight;
	topRight = Point2f(x, y);
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;
	renderCalibrationWindow(calibrationFrame, w1);

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->bottomRight = findClosestPoint(filtered, calibWnd->bottomRight);

	//
	// Top right
	//
	bottomLeft = topLeft;
	topLeft = Point2f(centerX, calibWnd->topLeft.y);
	bottomRight = topRight;
	topRight = calibWnd->topRight;
	w1->topLeft = topLeft;
	w1->topRight = topRight;
	w1->bottomLeft = bottomLeft;
	w1->bottomRight = bottomRight;
	renderCalibrationWindow(calibrationFrame, w1);

	// Filter points with current sub-window
	filtered = w1->filterPoints(intersPts);
	retval->topRight = findClosestPoint(filtered, calibWnd->topRight);

	delete w1;

	return retval;
}

TennisFieldDelimiter *TennisFieldCalibrator::calibrate_8UC4(uint8_t *u8data, int width, int height, bool *status)
{
	Mat calibrationFrame(Size(width, height), CV_8UC4, u8data);
	GpuMat d_calibrationFrame;

	// Upload calibration frame data on GPU device's memory
	printf("TennisFieldCalibrator :: calibrate(): converting RGB frame to grayscale...\n");
	GpuMat d_calibrationFrameRGB(calibrationFrame);
	gpu::cvtColor(d_calibrationFrameRGB, d_calibrationFrame, CV_RGBA2GRAY);
	printf("TennisFieldCalibrator :: calibrate(): conversion OK\n");

	// Detect lines
	printf("TennisFieldCalibrator :: calibrate(): running CUDA segment detector...\n");
	vector<Line> detectedLines = gpuLineDetector->detect(d_calibrationFrame);
	printf("TennisFieldCalibrator :: calibrate(): detected %d segments\n", detectedLines.size());

	// Process detected lines to obtain field lines
	// Processing segments is only marginally required when Barrel / Cushion distortions
	// prevent the line detector to detect straight continuous field lines, even after having detected
	// edges with the CUDA Canny algorithm
	// This processing does not perform a complex probabilistic analysis, simply check colinearity and
	// endpoints distance in order to provide an indicator about  if some segments are observations of the same line
	printf("TennisFieldCalibrator :: calibrate(): running coarse segment processor...\n");
	vector<Line> fieldLines = detectedLines;// coarseSegmentProcessor->process(detectedLines);

	// Get all intersection points
	intersDetector = new IntersectionPointsDetector(cones);
	vector<Point> inters = intersDetector->computeIntersectionPoints(fieldLines);

	if (fieldLines.size() < 4) {
		*status = false;
		return 0;
	}

	// Find field delimiting points
	TennisFieldDelimiter *tennisFieldDelimiter = computeConeDelimitedStaticModel(inters);
	for (int i = 0; i < inters.size(); i++) {
		circle(calibrationFrame, inters[i], 6, Scalar(255, 200, 0));
	}

	delete intersDetector;

	*status = true;

	return tennisFieldDelimiter;
}

TennisFieldDelimiter *TennisFieldCalibrator::calibrate(Mat calibrationFrame)
{
	GpuMat d_calibrationFrame;

	// If the calibration frame is not yet monochrome, convert it
	if (calibrationFrame.channels() > 1) {
		// Upload calibration frame data on GPU device's memory
		printf("TennisFieldCalibrator :: calibrate(): converting RGB frame to grayscale...\n");
		GpuMat d_calibrationFrameRGB(calibrationFrame);
		gpu::cvtColor(d_calibrationFrameRGB, d_calibrationFrame, CV_RGB2GRAY);
		printf("TennisFieldCalibrator :: calibrate(): conversion OK\n");
	}
	else {
		d_calibrationFrame.upload(calibrationFrame);
	}

	// Detect lines
	printf("TennisFieldCalibrator :: calibrate(): running CUDA segment detector...\n");
	vector<Line> detectedLines = gpuLineDetector->detect(d_calibrationFrame);
	printf("TennisFieldCalibrator :: calibrate(): detected %d segments\n", detectedLines.size());

	// Process detected lines to obtain field lines
	// Processing segments is only marginally required when Barrel / Cushion distortions
	// prevent the line detector to detect straight continuous field lines, even after having detected
	// edges with the CUDA Canny algorithm
	// This processing does not perform a complex probabilistic analysis, simply check colinearity and
	// endpoints distance in order to provide an indicator about  if some segments are observations of the same line
	printf("TennisFieldCalibrator :: calibrate(): running coarse segment processor...\n");
	vector<Line> fieldLines = detectedLines;// coarseSegmentProcessor->process(detectedLines);

	// Get all intersection points
	intersDetector = new IntersectionPointsDetector(calibrationWindow);
	vector<Point> inters = intersDetector->computeIntersectionPoints(fieldLines);

	// Find field delimiting points
	TennisFieldDelimiter *tennisFieldDelimiter = computeTennisFieldDelimiter(calibrationFrame, inters, calibrationWindow);
	for (int i = 0; i < inters.size(); i++) {
		circle(calibrationFrame, inters[i], 6, Scalar(0, 255, 255));
	}

	delete intersDetector;
	return tennisFieldDelimiter;
}

GPULineDetector *TennisFieldCalibrator::getCUDALinesDetector()
{
	return gpuLineDetector;
}

TennisFieldCalibrator::TennisFieldCalibrator() {
	gpuLineDetector = new GPULineDetector();
	coarseSegmentProcessor = new CoarseSegmentProcessor();
	intersDetector = 0;
	calibrationWindow = new CalibrationWindow();
}

TennisFieldCalibrator::~TennisFieldCalibrator() {
	delete gpuLineDetector;
	delete coarseSegmentProcessor;
	delete intersDetector;
	delete calibrationWindow;
}

