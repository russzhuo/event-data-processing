#include "eFast.h"
#include "process.h"

const int efast_inner_circle_size = 16;
const int efast_outer_circle_size = 20;

//radius=3
int efast_inner_circle[efast_inner_circle_size][2] =
{
	{0, 3}, { 1, 3 }, { 2, 2 }, { 3, 1 },
	{ 3, 0 }, { 3, -1 }, { 2, -2 }, { 1, -3 },
	{ 0, -3 }, { -1, -3 }, { -2, -2 }, { -3, -1 },
	{ -3, 0 }, { -3, 1 }, { -2, 2 }, { -1, 3 }
};

//radius=4
int efast_outer_circle[efast_outer_circle_size][2] =
{
	{0, 4}, { 1, 4 }, { 2, 3 }, { 3, 2 },
	{ 4, 1 }, { 4, 0 }, { 4, -1 }, { 3, -2 },
	{ 2, -3 }, { 1, -4 }, { 0, -4 }, { -1, -4 },
	{ -2, -3 }, { -3, -2 }, { -4, -1 }, { -4, 0 },
	{ -4, 1 }, { -3, 2 }, { -2, 3 }, { -1, 4 }
};

void eFast(int n_events) {

	vector<bool> is_corner_list;

	double SAE[2][HEIGHT][WIDTH] = { 0.0 };

	//int shape[] = { HEIGHT,WIDTH };
	//Mat	SAE = Mat::zeros(2, shape, CV_32FC1);


	ifstream file(EVENT_FILE);
	if (file.is_open()) {
		//some variables used to display progress
		int numerator = 0;
		int k = 0;
		int max_k = n_events / 100;

		int line_i = 0;
		string line;
		getline(file, line);

		auto start = high_resolution_clock::now();

		while (getline(file, line)) {
			//display progress
			if (++k == max_k) {
				cout << "..." << ++numerator << "/100" << "\r" << flush;
				k = 0;
			}
			//extract event from current line
			Event current = from_line(line);
			int y = current.y;
			int x = current.x;
			bool p = current.p;
			double t = current.t;

			//update SAE
			SAE[p][y][x] = t;

			//see if the current coordinates is too closed to border that can't form a proper circle
			if (is_border(x, y, 4)) {
				is_corner_list.push_back(false);
				continue;
			}

			bool found_segment = false;

			//search for segment of contiguouts pixels (arcs) along inner cicle
			for (int i = 0; i < efast_inner_circle_size; i++) {
				for (int segment_size = 3; segment_size <= 6; segment_size++) {
					if (SAE[p][y + efast_inner_circle[i][1]][x + efast_inner_circle[i][0]]
						< SAE[p][y + efast_inner_circle[(i - 1 + efast_inner_circle_size) % efast_inner_circle_size][1]][x + efast_inner_circle[(i - 1 + efast_inner_circle_size) % efast_inner_circle_size][0]]
						)
						continue;

					if (SAE[p][(y + efast_inner_circle[(i + segment_size - 1) % efast_inner_circle_size][1])][(x + efast_inner_circle[(i + segment_size - 1) % efast_inner_circle_size][0])]
						< SAE[p][(y + efast_inner_circle[(i + segment_size) % efast_inner_circle_size][1])][(x + efast_inner_circle[(i + segment_size) % efast_inner_circle_size][0])]
						)
						continue;

					double min_t = SAE[p][(y + efast_inner_circle[i][1])][(x + efast_inner_circle[i][0])];
					for (int j = 1; j < segment_size; j++) {
						double current_t = SAE[p][(y + efast_inner_circle[(i + j) % efast_inner_circle_size][1])][(x + efast_inner_circle[(i + j) % efast_inner_circle_size][0])];
						if (current_t < min_t) {
							min_t = current_t;
						}
					}

					bool did_break = false;
					for (int j = segment_size; j < efast_inner_circle_size; j++) {
						double current_t = SAE[p][(y + efast_inner_circle[(i + j) % efast_inner_circle_size][1])][(x + efast_inner_circle[(i + j) % efast_inner_circle_size][0])];

						if (current_t >= min_t) {
							did_break = true;
							break;
						}
					}

					if (!did_break) {
						found_segment = true;
						break;
					}
				}
			}

			//search for segment of contiguouts pixels (arcs) along outer cicle
			for (int i = 0; i < efast_outer_circle_size; i++) {
				for (int segment_size = 4; segment_size <= 8; segment_size++) {
					if (SAE[p][(y + efast_outer_circle[i][1])][(x + efast_outer_circle[i][0])]
						< SAE[p][(y + efast_outer_circle[(i - 1 + efast_outer_circle_size) % efast_outer_circle_size][1])][(x + efast_outer_circle[(i - 1 + efast_outer_circle_size) % efast_outer_circle_size][0])]
						)
						continue;

					if (SAE[p][(y + efast_outer_circle[(i + segment_size - 1) % efast_outer_circle_size][1])][(x + efast_outer_circle[(i + segment_size - 1) % efast_outer_circle_size][0])]
						< SAE[p][(y + efast_outer_circle[(i + segment_size) % efast_outer_circle_size][1])][(x + efast_outer_circle[(i + segment_size) % efast_outer_circle_size][0])]
						)
						continue;

					double min_t = SAE[p][(y + efast_outer_circle[i][1])][(x + efast_outer_circle[i][0])];
					for (int j = 1; j < segment_size; j++) {
						double current_t = SAE[p][(y + efast_outer_circle[(i + j) % efast_outer_circle_size][1])][(x + efast_outer_circle[(i + j) % efast_outer_circle_size][0])];
						if (current_t < min_t) {
							min_t = current_t;
						}
					}

					bool did_break = false;
					for (int j = segment_size; j < efast_outer_circle_size; j++) {
						double current_t = SAE[p][(y + efast_outer_circle[(i + j) % efast_outer_circle_size][1])][(x + efast_outer_circle[(i + j) % efast_outer_circle_size][0])];

						if (current_t >= min_t) {
							did_break = true;
							break;
						}
					}

					if (!did_break) {
						found_segment = true;
						break;
					}
				}
			}

			is_corner_list.push_back(found_segment ? true : false);

			if (line_i++ == 1000000) {
				auto stop = high_resolution_clock::now();

				auto duration = duration_cast<microseconds>(stop - start);

				cout << duration.count() << endl;
			}
		}
		cout << endl;

		file.close();

		save_result(is_corner_list, -1.1, detector);
		save_result_with_filtering(is_corner_list, -1.1, detector);
	}
	else {
		cout << "File not found: EVENT_FILE" << endl;
	}

}

bool is_efast_corner(double t, int x, int y, bool p) {
	bool segment_found = false;

	//search for segment of contiguouts pixels (arcs) along inner cicle
	for (int i = 0; i < efast_inner_circle_size; i++) {
		for (int segment_size = 3; segment_size <= 6; segment_size++) {

		}
	}

	//search for segment of contiguouts pixels (arcs) along outer cicle
	for (int i = 0; i < efast_outer_circle_size; i++) {
	}

	return false;
}

