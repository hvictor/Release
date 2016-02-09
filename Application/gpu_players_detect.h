#ifndef GPU_PLAYERS_H
#define GPU_PLAYERS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace cv;

std::vector<Rect> detectPlayers(Mat& frame0);
void drawRectAroundPlayers(Mat& frame0, vector<cv::Rect> found);

#endif
