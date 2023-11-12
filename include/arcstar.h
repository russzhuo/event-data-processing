#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <utility>
#include "process.h"
bool is_arcstar_corner(double_vector_2d & sae_, int x, int y, double t, bool p);

void arcstar(int n_events);