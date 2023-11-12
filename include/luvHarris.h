#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <utility>

using namespace cv;
using namespace std;


void luvHarris(int n_threads, float threshold, int k_tos, int n_events);
void update_LUT(int tid, Mat& LUT, Mat& TOS, bool& is_running, mutex& mtx);
void update_TOS(Mat& TOS, int x, int y, int k_tos, int min_tos);
void luvHarris_accumulated(int n_threads, float threshold, int k_tos, int n_events);
