#include "process.h"


Event from_line(string line) {
	//extract t, x, y, p from current line
	int i_first_empty = line.find(' ');
	int i_second_empty = line.find(' ', i_first_empty + 1);
	int i_last_empty = line.rfind(' ');

	double t = stod(line.substr(0, i_first_empty));
	int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
	int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));
	bool p = line[line.size() - 1] == '1' ? true : false;
	
	return { t, x, y, p };
}

//int get_numb_line_of(string dataset) {
//	if(detector=='eharris')
//		return numb_lines_boxes_6dof
//}

Event_without_t from_line_without_t(string line) {
	//extract t, x, y, p from current line
	int i_first_empty = line.find(' ');
	int i_second_empty = line.find(' ', i_first_empty + 1);
	int i_last_empty = line.rfind(' ');

	//double t = stod(line.substr(0, i_first_empty));
	int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
	int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));
	bool p = line[line.size() - 1] == '1' ? true : false;

	return { x, y, p };
}


Event_without_t_and_p from_line_without_t_and_p(string line) {
	//extract t, x, y, p from current line
	int i_first_empty = line.find(' ');
	int i_second_empty = line.find(' ', i_first_empty + 1);
	int i_last_empty = line.rfind(' ');

	//double t = stod(line.substr(0, i_first_empty));
	int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
	int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));
	//bool p = line[line.size() - 1] == '1' ? true : false;

	return { x, y };
}



bool is_border(int x, int y, int min) {
	return x < min || x >= WIDTH-min || y < min || y >= HEIGHT-min;
}

void save_result_with_filtering(vector<bool>& is_corner_list, double threshold, string detector_name) {

	//ofstream is_corner_list_file(IS_CORNER_LIST_FILE, ios::out);
	//if (is_corner_list_file.is_open()) {
	//	for (auto is_corner : is_corner_list) {
	//		is_corner_list_file << is_corner ? '1' : '0' << '\n';
	//	}
	//}

	ifstream ground_truth(GROUND_TRUTH_FILE);
	int true_positive = 0;
	int false_positive = 0;
	int false_negative = 0;
	int true_negative = 0;

	ifstream is_noise(IS_NOISE_FILE);

	if (ground_truth.is_open()) {
		//compare ground truth and predicted label  
		string line;
		string noise;
		int i = 0;
		while (getline(ground_truth, line)) {
			bool is_true_corner = (line[0] == '1') ? true : false;
			bool is_predict_corner = is_corner_list[i++];

			getline(is_noise, noise);
			if (noise == "1") continue;

			if (is_predict_corner and is_true_corner) ++true_positive;
			else if (!is_predict_corner and !is_true_corner) ++true_negative;
			else if (is_predict_corner and !is_true_corner) ++false_positive;
			else if (!is_predict_corner and is_true_corner) ++false_negative;
		}

		ground_truth.close();
	}
	else {
		cout << "File not found: GROUND_TRUTH_FILE" << endl;
	}

	ofstream result(RESULT_FILE, ios::app);
	if (result.is_open()) {
		//result << current_time << endl;
		result << "======================================================" << endl;
		result << "with filtering" << endl;
		result << "detector: " << detector_name << endl;
		result << "threshold: " << threshold << endl;
		//result << "number of predict corner: " << numb_predict_corner << endl;
		//result << "number of true corner: " << numb_true_corner << endl;
		//result << "true positive: " << true_positive << endl;
		//result << "true negative : " << true_negative << endl;
		//result << "false positive: " << false_positive << endl;
		//result << "false negative: " << false_negative << endl;
		result << "precision: " << true_positive / ((double)true_positive + false_positive) << endl;
		result << "recall: " << true_positive / ((double)true_positive + false_negative) << endl;
		result << "======================================================" << endl;

		result.close();
	}
	else {
		cout << "File not found: RESULT_FILE" << endl;
	}

}

void save_result(vector<bool>& is_corner_list, double threshold,string detector_name) {

	//ofstream is_corner_list_file(IS_CORNER_LIST_FILE, ios::out);
	//if (is_corner_list_file.is_open()) {
	//	for (auto is_corner : is_corner_list) {
	//		is_corner_list_file << is_corner ? '1' : '0' << '\n';
	//	}
	//}

	ifstream ground_truth(GROUND_TRUTH_FILE);
	int true_positive = 0;
	int false_positive = 0;
	int false_negative = 0;
	int true_negative = 0;


	if (ground_truth.is_open()) {
		//compare ground truth and predicted label  
		string line;
		string noise;
		int i = 0;
		while (getline(ground_truth, line)) {
			bool is_true_corner = (line[0] == '1') ? true : false;
			bool is_predict_corner = is_corner_list[i++];


			if (is_predict_corner and is_true_corner) ++true_positive;
			else if (!is_predict_corner and !is_true_corner) ++true_negative;
			else if (is_predict_corner and !is_true_corner) ++false_positive;
			else if (!is_predict_corner and is_true_corner) ++false_negative;
		}

		ground_truth.close();
	}
	else {
		cout << "File not found: GROUND_TRUTH_FILE" << endl;
	}

	ofstream result(RESULT_FILE, ios::app);
	if (result.is_open()) {
		//result << current_time << endl;
		result << "======================================================" << endl;
		result << "without filtering" << endl;
		result << "detector: " << detector_name << endl;
		result << "threshold: " << threshold << endl;
		//result << "number of predict corner: " << numb_predict_corner << endl;
		//result << "number of true corner: " << numb_true_corner << endl;
		//result << "true positive: " << true_positive << endl;
		//result << "true negative : " << true_negative << endl;
		//result << "false positive: " << false_positive << endl;
		//result << "false negative: " << false_negative << endl;
		result << "precision: " << true_positive / ((double)true_positive + false_positive) << endl;
		result << "recall: " << true_positive / ((double)true_positive + false_negative) << endl;
		result << "======================================================" << endl;

		result.close();
	}
	else {
		cout << "File not found: RESULT_FILE" << endl;
	}
	
}

//void gaussian_window(vector<vector<double>>& kernel) {
//	double sigma = 1.0;
//	double r, s = 2.0 * sigma * sigma;
//	double sum = 0.0;
//	
//	int kernel_size=kernel
//	for (int x = -2; x <= 2; x++) {
//
//	}
//}

//https://www.geeksforgeeks.org/gaussian-filter-generation-c/


void print_vector(vector<vector<int>> grid) {
	for (int i = 0; i < grid.size(); i++) {
		for (int j = 0; j < grid[i].size(); j++) {
			cout << "[" << grid[i][j] << "]";
		}
		// Add a new line after every row
		cout << endl;
	}

}
