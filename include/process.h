#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <utility>
#include <vector>

using namespace std::chrono;
using namespace std;
using namespace cv;

using int_vector_2d = vector<vector<int>>;
using bool_vector_2d = vector<vector<bool>>;
using float_vector_2d = vector<vector<float>>;
using double_vector_2d = vector<vector<double>>;

const int HEIGHT = 180;
const int WIDTH = 240;

const string dataset = "dynamic_6dof";
const string detector = "efast";

//EVENT_FILE_PATH = r"C:\projects\event_data_processing\data\%s\%s.txt" % (dataset_name, dataset_name)
//FRAME_FOLDER_PATH = r"C:\projects\event_data_processing\data\%s\reconstruction" % dataset_name
const int numb_lines_boxes_6dof = 133083127;
const int numb_lines_shapes_6dof = 17960658;
const int numb_lines_poster_6dof = 133462769;
const int numb_lines_dynamic_6dof = 57132658;

const string EVENT_FILE = "C:/projects/event_data_processing/data/"+dataset+"/" +dataset+".txt";
const string RESULT_FILE = "C:/projects/event_data_processing/data/" + dataset + "/" + "result_" + detector + ".txt";
const string GROUND_TRUTH_FILE = "C:/projects/event_data_processing/data/" + dataset + "/" + "gt_5_5_is_corner.txt";
const string IS_NOISE_FILE = "C:/projects/event_data_processing/data/" + dataset + "/" + "is_noise.txt";

const string FILTERED_GROUND_TRUTH = "C:/projects/event_data_processing/data/reconstruction_shapes_6dof/filtered_ground_truth.txt";
const string FILTERED_RESULT_FILE = "C:/projects/event_data_processing/data/reconstruction_shapes_6dof/filtered_result_blockSize=5_ksize=5_k=0.04.txt";
const string FILTERED = "C:/projects/event_data_processing/data/reconstruction_shapes_6dof/filtered.txt";
const string IS_CORNER_LIST_FILE = "C:/projects/event_data_processing/data/reconstruction_shapes_6dof/is_corner_list.txt";

struct Event {
	double t;
	int x;
	int y;
	bool p;
};

struct Event_without_t {
	int x;
	int y;
	bool p;
};

struct Event_without_t_and_p {
	int x;
	int y;
};

bool is_border(int x, int y, int min);

Event from_line(string line);
Event_without_t from_line_without_t(string line);
Event_without_t_and_p from_line_without_t_and_p(string line);
void save_result(vector<bool>& is_corner_list, double threshold, string detector_name);
void save_result_with_filtering(vector<bool>& is_corner_list, double threshold, string detector_name);

//void gaussian_window(vector<vector<double>>& kernel);
void print_vector(vector<vector<int>> grid);