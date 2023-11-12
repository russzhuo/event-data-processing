#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <utility>

using namespace std;
using namespace cv;


void eFast(int n_events);
bool is_efast_corner(double t, int x, int y, bool p);