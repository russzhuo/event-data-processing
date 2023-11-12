#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <utility>
#include <vector>
#include "process.h"
#include <deque>
#include <queue>

using namespace std;
using namespace cv;

const int window_length = 7;
const int kernel_size = 5;
const int neighbor_size = window_length / 2;


void extract_patch(int x, int y, const bool binarized_surface[HEIGHT][WIDTH]);

void eHarris(double threshold, int n_events); 
void eHarris_accumulated(double threshold, int n_events);

void reset();
double compute_harris_score(int x, int y);
//void compute_gradient(const int_vector_2d& binarized_surface, int_vector_2d& gradient_dx, int_vector_2d& gradient_dy, int x, int y);
//void extract_patch(int x, int y, vector<vector<bool>>& binarized_surface, vector<vector<int>>& patch);
//void extract_patch(int x, int y, vector<vector<int>>& binarized_surface, vector<vector<int>>& patch);
//float compute_harris_score(int img_x, int img_y, bool p, vector<vector<int>>& binarized_surface_positive, vector<vector<int>>& binarized_surface_negative);
//void compute_queue(vector<Event>& queue);
//void eHarris();