#include "eHarris.h"
#include "process.h"

//const int sobel_x[][kernel_size] ={
//	{1,2,0,-2,1},
//	{4,8,0,-8,-4},
//	{6,12,0,-12,-6},
//	{4,8,0,-8,-4},
//	{1,2,0,-2,-1}
//};
//
//const int sobel_y[][kernel_size]={
//	{1,4,6,4,1},
//	{2,8,12,8,2},
//	{0,0,0,0,0},
//	{-2,-8,-12,-8,-2},
//	{-1,-4,-6,-4,-1}
//};

const float sobel_x[][5] = {
	{-0.083333,	-0.166666,	0,	0.166666,	0.083333},
	{-0.333333,	-0.666666,	0,	0.666666,	0.333333},
	{	  -0.5,	       -1,	0,		   1,	     0.5},
	{-0.333333,	-0.666666,	0,	0.666666,	0.333333},
	{-0.083333,	-0.166666,	0,	0.166666,	0.083333}
};

const float sobel_y[][5] = {
	{-0.083333,	-0.333333,	  -0.5,	 -0.333333,	 -0.083333},
	{-0.166666,	-0.666666,	    -1,	 -0.666666,	 -0.166666},
	{		0,		   0,		0,		    0,		    0},
	{0.166666,	0.666666,	   1,	0.666666,	0.166666},
	{0.083333,	0.333333,   0.5,	0.333333,	0.083333}
};


double gaussian_window_3x3[][3] = {
	{1,2,1},
	{2,4,2},
	{1,2,1}
};

double gaussian_window_5x5[][5] = {
	{1,  4,  7,  4,  1},
	{4,  16, 26, 16, 4},
	{7,  26, 41, 26, 7},
	{4,  16, 26, 16, 4},
	{1,  4,  7,  4,  1}
};
//[[0.0029690167439505, 0.0133062098910137, 0.0219382312797146, 0.0133062098910137, 0.0029690167439505];
//[0.0133062098910137, 0.0596342954361801, 0.0983203313488458, 0.0596342954361801, 0.0133062098910137] ;
//[0.0219382312797146, 0.0983203313488458, 0.162102821637127, 0.0983203313488458, 0.0219382312797146] ;
//[0.0133062098910137, 0.0596342954361801, 0.0983203313488458, 0.0596342954361801, 0.0133062098910137] ;
//[0.0029690167439505, 0.0133062098910137, 0.0219382312797146, 0.0133062098910137, 0.0029690167439505] ]

double gaussian_window_7x7[][7] = {
	{0,	0,	1,	2,	1,	0,	0},
	{0,	3,	13,	22,	13,	3,	0},
	{1,	13,	59,	97,	59,	13,	1},
	{2,	22,	97,	159,97,	22,	2},
	{1,	13,	59,	97,	59,	13,	1},
	{0,	3,	13,	22,	13,	3,	0},
	{0,	0,	1,	2,	1,	0,	0},
};

bool is_normalized = false;
void normalize_gaussian_window_7x7(double gaussian_window[][7]) {
	if (is_normalized)
		return;

	int sum = 0;
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			sum += gaussian_window[y][x];
		}
	}

	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			gaussian_window[y][x] /= sum;
		}
	}

	is_normalized = true;
}

bool patch[window_length][window_length] = { false };

bool binarized_surface_positive[HEIGHT][WIDTH] = { false };
bool binarized_surface_negative[HEIGHT][WIDTH] = { false };

void reset() {

	for (int i = 0; i < window_length; i++)
		for (int j = 0; j < window_length; j++) {
			patch[i][j] = false;
		}

	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++) {
			binarized_surface_negative[i][j] = false;
			binarized_surface_positive[i][j] = false;
		}

}
void eHarris(double threshold, int n_events) {
	normalize_gaussian_window_7x7(gaussian_window_7x7);

	const int queue_size = 200;

	int head = queue_size - 1;
	int tail = 0;

	vector<bool> is_corner_list;

	ifstream file(EVENT_FILE);
	if (file.is_open()) {


		//some variables used to display progress 
		int numerator = 0;
		int k = 0;
		int max_k = n_events / 100;


		int max_queue_size = 1000;

		string line;
		getline(file, line);

		int line_i = 0;
		queue<Event> current_events;

		auto start = high_resolution_clock::now();

		while (getline(file, line)) {

			//display progress
			if (++k == max_k) {
				cout << "..." << ++numerator << "/100" << "\r" << flush;
				k = 0;
			}
			
			//extract event from current line
			Event current = from_line(line);


			//update queue 
			current_events.push(current);

			//update surface
			if (current.p)
				binarized_surface_positive[current.y][current.x] = true;
			else
				binarized_surface_negative[current.y][current.x] = true;

			//see if queue is filled up with N events 
			if (current_events.size() < max_queue_size) {
				is_corner_list.push_back(false);
				continue;
			}

			// see if it is too closed to the border
			if (is_border(current.x, current.y, neighbor_size)) {
				is_corner_list.push_back(false);
				continue;
			}

			// 
			////extract patch associated with current event position
			//int min_patch_x = current.x - neighbor_size;
			//int min_patch_y = current.y - neighbor_size;
			//int max_patch_x = current.x + neighbor_size;
			//int max_patch_y = current.y + neighbor_size;

			extract_patch(current.x, current.y, current.p ? binarized_surface_positive : binarized_surface_negative);

			//see if it is a corner 
			is_corner_list.push_back(compute_harris_score(current.x, current.y) > threshold ? true : false);

			//reset patch so that we can use it again in next iteration
			for (int yi = 0; yi < window_length; yi++)
				for (int xi = 0; xi < window_length; xi++)
					patch[yi][xi] = false;


			Event front = current_events.front();

			if (front.p)
				binarized_surface_positive[front.y][front.x] = false;
			else
				binarized_surface_negative[front.y][front.x] = false;

			current_events.pop();



			if (line_i++ == 1000000) {
				auto stop = high_resolution_clock::now();

				auto duration = duration_cast<microseconds>(stop - start);

				cout << duration.count() << endl;
				return;
			}
		}

		file.close();
		reset();
		//while (is_corner_list.size() < n_events) is_corner_list.push_back(false);

		save_result(is_corner_list, threshold, detector);
		save_result_with_filtering(is_corner_list, threshold, detector);

	}
	else {
		cout << "File not found: EVENT_FILE" << endl;
	}

}

void eHarris_accumulated(double threshold, int n_events) {
	normalize_gaussian_window_7x7(gaussian_window_7x7);

	const int queue_size = 200;

	int head = queue_size - 1;
	int tail = 0;

	vector<bool> is_corner_list;

	ifstream file(EVENT_FILE);
	if (file.is_open()) {


		//some variables used to display progress 
		int numerator = 0;
		int k = 0;
		int max_k = n_events / 100;


		int max_queue_size = 1000;

		string line;
		getline(file, line);

		int line_i = 0;
		queue<Event_without_t> current_events;

		while (getline(file, line)) {

			//display progress
			if (++k == max_k) {
				cout << "..." << ++numerator << "/100" << "\r" << flush;
				k = 0;
			}

			//extract event from current line
			Event_without_t current = from_line_without_t(line);


			//update queue 
			current_events.push(current);

			//update surface
			if (current.p)
				binarized_surface_positive[current.y][current.x] = true;
			else
				binarized_surface_negative[current.y][current.x] = true;

			//see if queue is filled up with N events 
			if (current_events.size() < max_queue_size) {
				is_corner_list.push_back(false);
				continue;
			}

			// see if it is too closed to the border
			if (is_border(current.x, current.y, neighbor_size)) {
				is_corner_list.push_back(false);
				continue;
			}

			////extract patch associated with current event position
			//int min_patch_x = current.x - neighbor_size;
			//int min_patch_y = current.y - neighbor_size;
			//int max_patch_x = current.x + neighbor_size;
			//int max_patch_y = current.y + neighbor_size;

			extract_patch(current.x, current.y, current.p ? binarized_surface_positive : binarized_surface_negative);

			//see if it is a corner 
			is_corner_list.push_back(compute_harris_score(current.x, current.y) > threshold ? true : false);

			//reset patch so that we can use it again in next iteration
			for (int yi = 0; yi < window_length; yi++)
				for (int xi = 0; xi < window_length; xi++)
					patch[yi][xi] = false;


			Event_without_t front = current_events.front();

			if (front.p)
				binarized_surface_positive[front.y][front.x] = false;
			else
				binarized_surface_negative[front.y][front.x] = false;

			current_events.pop();
		}

		file.close();
		reset();
		//while (is_corner_list.size() < n_events) is_corner_list.push_back(false);

		save_result(is_corner_list, threshold, detector);
		save_result_with_filtering(is_corner_list, threshold, detector);

	}
	else {
		cout << "File not found: EVENT_FILE" << endl;
	}

}

void extract_patch(int x, int y, const bool binarized_surface[HEIGHT][WIDTH]) {
	for (int i = 0, yi = y - neighbor_size; i < window_length; i++, yi++) {
		for (int j = 0, xi = x - neighbor_size; j < window_length; j++, xi++) {
			patch[i][j] = binarized_surface[yi][xi];
		}
	}

}

double compute_harris_score(int x, int y) {
	double gradient_dy[window_length][window_length] = { 0. };
	double gradient_dx[window_length][window_length] = { 0. };

	//int offset_y = y - window_length / 2 - kernel_size / 2;
	//int offset_x = x - window_length / 2 - kernel_size / 2;

	//compute gradient for local patch
	for (int yi = 0; yi < window_length; yi++) {
		for (int xi = 0; xi < window_length; xi++) {
			for (int kernel_yi = 0, patch_yi = yi - kernel_size / 2; kernel_yi < kernel_size; kernel_yi++, patch_yi++) {
				if (patch_yi < 0 || patch_yi >= window_length) continue;

				for (int kernel_xi = 0, patch_xi = xi - kernel_size / 2; kernel_xi < kernel_size; kernel_xi++, patch_xi++) {
					if (patch_xi < 0 || patch_xi >= window_length) continue;

					gradient_dx[yi][xi] += patch[patch_yi][patch_xi] * sobel_x[kernel_yi][kernel_xi];
					gradient_dy[yi][xi] += patch[patch_yi][patch_xi] * sobel_y[kernel_yi][kernel_xi];

				}
			}
		}
	}

	// M(ei) = sum([ a, b],
	//			   [ c, d]), where b == c
	double a = 0., b = 0., d = 0.;
	for (int yi = 0; yi < window_length; yi++)
	{
		for (int xi = 0; xi < window_length; xi++)
		{
			a += gradient_dx[yi][xi] * gradient_dx[yi][xi] * gaussian_window_7x7[yi][xi];
			b += gradient_dx[yi][xi] * gradient_dy[yi][xi] * gaussian_window_7x7[yi][xi];
			d += gradient_dy[yi][xi] * gradient_dy[yi][xi] * gaussian_window_7x7[yi][xi];
		}
	}

	//R(ei) = det (M) - k * trace ^2 (M), where k = 0.04 
	return a * d - b * b - 0.04 * (a + d) * (a + d);

}



