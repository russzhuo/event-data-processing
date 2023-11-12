#include "luvHarris.h"
#include "process.h"

void update_LUT(int tid, Mat& LUT, Mat& TOS, bool& is_running, mutex& mtx) {
	//cout << "thread " << tid << " running......" << endl;
	int blockSize = 5;
	int kSize = 5;
	float k = 0.04;
	//static Mat blurred;

	while (is_running) {
		//GaussianBlur(TOS, blurred, Size(5,5),0,0);
		cornerHarris(TOS, LUT, blockSize, kSize, k);
	}

	//cout << "thread " << tid << " terminating......" << endl;
}


void update_TOS(Mat& TOS, int x, int y, int k_tos, int min_tos) {
	if (x > k_tos and x + k_tos < WIDTH and y > k_tos and y + k_tos < HEIGHT) {
		for (unsigned char xi = x - k_tos; xi <= x + k_tos; xi++) {
			for (unsigned char yi = y - k_tos; yi <= y + k_tos; yi++) {
				TOS.at<unsigned char>(yi, xi) = ((TOS.at<unsigned char>(yi, xi) - 1) > min_tos) ? (TOS.at<unsigned char>(yi, xi) - 1) : 0;
			}
		}

		TOS.at<unsigned char>(y, x) = 255;
	}
} 


void luvHarris(int n_threads, float threshold, int k_tos, int n_events) {

	vector<bool> is_corner_list;

	Mat LUT = Mat::zeros(HEIGHT, WIDTH, CV_32FC1);
	Mat TOS = Mat::zeros(HEIGHT, WIDTH, CV_8UC1);

	//atomic_bool is_running(true);
	bool is_running = true;

	mutex mtx;


	//create and start threads to update lookup table
	vector<thread> LUT_updater_threads;
	for (int i = 0; i < n_threads; ++i) {
		thread updater(update_LUT, i, ref(LUT), ref(TOS), ref(is_running), ref(mtx));
		LUT_updater_threads.push_back(move(updater));
	}

	int t_tos = 2 * (2 * k_tos + 1);
	int min_tos = 255 - t_tos;

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
			//for (int i=0;i<1000;i++){}
			//extract event from current line
			//Event event = from_line(line);

			int i_first_empty = line.find(' ');
			int i_second_empty = line.find(' ', i_first_empty + 1);
			int i_last_empty = line.rfind(' ');
			int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
			int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));

			//mtx.lock();
			update_TOS(TOS, x, y, k_tos, min_tos);
			//mtx.unlock();

			//debug, see if lookup table is updating or not.
			//if (LUT.at<float>(y, x) > 0.0001) cout << LUT.at<float>(y, x)<<endl;

			////see if the current event is a corner or not
			is_corner_list.push_back(LUT.at<float>(y, x) > threshold ? true : false);

			if (line_i++ == 1000000) { 
				auto stop = high_resolution_clock::now();

				auto duration = duration_cast<microseconds>(stop - start);

				cout << duration.count() << endl;
			}

		}

		file.close();

		//terminate updater threads
		is_running = false;

		//wait until all updater threads have terminated
		for (int i = 0; i < n_threads; ++i) {
			LUT_updater_threads[i].join();
		}

		//assert(is_corner_list.size() == n_events);
		cout << endl;
		cout << is_corner_list.size() << endl;
		
		save_result(is_corner_list, threshold, detector);
		save_result_with_filtering(is_corner_list, threshold, detector);

	}
	else {
		cout << "File not found: EVENT_FILE" << endl;
	}

}


void luvHarris_accumulated(int n_threads, float threshold, int k_tos, int n_events) {

	vector<bool> is_corner_list;

	Mat LUT = Mat::zeros(HEIGHT, WIDTH, CV_32FC1);
	Mat TOS = Mat::zeros(HEIGHT, WIDTH, CV_8UC1);

	//atomic_bool is_running(true);
	bool is_running = true;

	mutex mtx;


	//create and start threads to update lookup table
	vector<thread> LUT_updater_threads;
	for (int i = 0; i < n_threads; ++i) {
		thread updater(update_LUT, i, ref(LUT), ref(TOS), ref(is_running), ref(mtx));
		LUT_updater_threads.push_back(move(updater));
	}

	int t_tos = 2 * (2 * k_tos + 1);
	int min_tos = 255 - t_tos;
	
	
	ifstream file(EVENT_FILE);
	if (file.is_open()) {
		//some variables used to display progress 
		int numerator = 0;
		int k = 0;
		int max_k = n_events / 100;

		int line_i = 0;
		string line;
		getline(file, line);

		while (getline(file, line)) {

			//display progress
			if (++k == max_k) {
				cout << "..." << ++numerator << "/100" << "\r" << flush;
				k = 0;
			}
			//for (int i=0;i<1000;i++){}
			//extract event from current line
			int i_first_empty = line.find(' ');
			int i_second_empty = line.find(' ', i_first_empty + 1);
			int i_last_empty = line.rfind(' ');
			int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
			int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));

			//int i_first_empty = line.find(' ');
			//int i_second_empty = line.find(' ', i_first_empty + 1);
			//int i_last_empty = line.rfind(' ');
			//int x = stoi(line.substr(i_first_empty + 1, i_second_empty - i_first_empty));
			//int y = stoi(line.substr(i_second_empty + 1, i_last_empty - i_second_empty));

			//mtx.lock();
			update_TOS(TOS, x, y, k_tos, min_tos);
			//mtx.unlock();

			//debug, see if lookup table is updating or not.
			//if (LUT.at<float>(y, x) > 0.0001) cout << LUT.at<float>(y, x)<<endl;

			////see if the current event is a corner or not
			is_corner_list.push_back(LUT.at<float>(y, x) > threshold ? true : false);


		}

		file.close();

		//terminate updater threads
		is_running = false;

		//wait until all updater threads have terminated
		for (int i = 0; i < n_threads; ++i) {
			LUT_updater_threads[i].join();
		}

		//assert(is_corner_list.size() == n_events);
		cout << endl;
		cout << is_corner_list.size() << endl;

		save_result(is_corner_list, threshold, detector);
		save_result_with_filtering(is_corner_list, threshold, detector);

	}
	else {
		cout << "File not found: EVENT_FILE" << endl;
	}

}
