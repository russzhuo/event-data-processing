#include "process.h"
#include "arcstar.h"

const int inner_circle_size = 16;
const int outer_circle_size = 20;
//
const int L_inner_min = 3;
const int L_inner_max = 6;
//
const int L_outer_min = 4;
const int L_outer_max = 8;

int kSmallCircle_[16][2] = { {0, 3}, {1, 3}, {2, 2}, {3, 1},
                            {3, 0}, {3, -1}, {2, -2}, {1, -3},
                            {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
                            {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3} };
int kLargeCircle_[20][2] = { {0, 4}, {1, 4}, {2, 3}, {3, 2},
                            {4, 1}, {4, 0}, {4, -1}, {3, -2},
                            {2, -3}, {1, -4}, {0, -4}, {-1, -4},
                            {-2, -3}, {-3, -2}, {-4, -1}, {-4, 0},
                            {-4, 1}, {-3, 2}, {-2, 3}, {-1, 4} };
//radius=3
int inner_circle[inner_circle_size][2] =
{
    {0, 3}, { 1, 3 }, { 2, 2 }, { 3, 1 },
    { 3, 0 }, { 3, -1 }, { 2, -2 }, { 1, -3 },
    { 0, -3 }, { -1, -3 }, { -2, -2 }, { -3, -1 },
    { -3, 0 }, { -3, 1 }, { -2, 2 }, { -1, 3 }
};

//radius=4
int outer_circle[outer_circle_size][2] =
{
    {0, 4}, { 1, 4 }, { 2, 3 }, { 3, 2 },
    { 4, 1 }, { 4, 0 }, { 4, -1 }, { 3, -2 },
    { 2, -3 }, { 1, -4 }, { 0, -4 }, { -1, -4 },
    { -2, -3 }, { -3, -2 }, { -4, -1 }, { -4, 0 },
    { -4, 1 }, { -3, 2 }, { -2, 3 }, { -1, 4 }
};

void arcstar(int n_events) {

    vector<bool> is_corner_list;

    vector<vector<double>> SAE_0(HEIGHT, vector<double>(WIDTH, 0.0));
    vector<vector<double>> SAE_1(HEIGHT, vector<double>(WIDTH, 0.0));

    //int shape[] = { HEIGHT,WIDTH };
    //Mat	SAE[p] = Mat::zeros(2, shape, CV_32FC1);

    
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

            //update surface
            if (p) {
                SAE_1[y][x] = t;
            }
            else {
                SAE_0[y][x] = t;
            }

            //see if the current coordinates is too closed to border that can't form a proper circle
            if (is_border(x, y, 4)) {
                is_corner_list.push_back(false);
                continue;
            }
            //cout << jj++ << endl;
            is_corner_list.push_back(is_arcstar_corner(p ? SAE_1 : SAE_0, x, y,t,p));

            if (line_i++ == 1000000) {
                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                cout << duration.count() << endl;
                return;
            }

        }

        cout << endl;

        file.close();

        save_result(is_corner_list, -1.1,detector);
        save_result_with_filtering(is_corner_list, -1.1,detector);

    }
    else {
        cout << "File not found: EVENT_FILE" << endl;
    }
}
bool is_arcstar_corner(double_vector_2d & sae_, int x, int y, double t, bool p) {
        const int kSmallCircleSize = 16;
        const int kLargeCircleSize = 20;
        const int kSmallMinThresh = 3;
        const int kSmallMaxThresh = 6;
        const int kLargeMinThresh = 4;
        const int kLargeMaxThresh = 8;
        
        bool is_arc_valid = false;
        // Small Circle exploration
        //std::cout << "SMALL CIRCLE EXPLORATION" << std::endl;
         //Initialize arc from newest element
        double segment_new_min_t = sae_[y + kSmallCircle_[0][1]][x + kSmallCircle_[0][0]];

        // Left and Right are equivalent to CW and CCW as in the paper
        int arc_right_idx = 0;
        int arc_left_idx;

        // Find newest
        for (int i = 1; i < kSmallCircleSize; i++) {
            const double t_temp = sae_[y + kSmallCircle_[i][1]][x + kSmallCircle_[i][0]];
            if (t_temp > segment_new_min_t) {
                segment_new_min_t = t_temp;
                arc_right_idx = i; // % End up in the maximum value
                //std::cout << "max right arc index is: " << arc_right_idx << std::endl;
            }
        }

        // Shift to the sides of the newest element;
        arc_left_idx = (arc_right_idx - 1 + kSmallCircleSize) % kSmallCircleSize;
        arc_right_idx = (arc_right_idx + 1) % kSmallCircleSize;
        double arc_left_value = sae_[y + kSmallCircle_[arc_left_idx][1]][x + kSmallCircle_[arc_left_idx][0]];
        double arc_right_value = sae_[y + kSmallCircle_[arc_right_idx][1]][x + kSmallCircle_[arc_right_idx][0]];
        double arc_left_min_t = arc_left_value;
        double arc_right_min_t = arc_right_value;

        // Expand
        // Initial expand does not require checking
        int iteration = 1; // The arc already contain the maximum
        for (; iteration < kSmallMinThresh; iteration++) {
            // Decide the most promising arc
            if (arc_right_value > arc_left_value) { // Right arc
                if (arc_right_min_t < segment_new_min_t) {
                    segment_new_min_t = arc_right_min_t;
                    //std::cout << "smallest timestamp of right segment: " << segment_new_min_t << std::endl;
                }
                // Expand arc
                arc_right_idx = (arc_right_idx + 1) % kSmallCircleSize;
                arc_right_value = sae_[y + kSmallCircle_[arc_right_idx][1]][x + kSmallCircle_[arc_right_idx][0]];
                if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
                    arc_right_min_t = arc_right_value;
                    //std::cout << "Updated smallest timestamp of right segment: " << arc_right_min_t << std::endl;
                }
            }
            else { // Left arc
                // Include arc in new segment
                if (arc_left_min_t < segment_new_min_t) {
                    segment_new_min_t = arc_left_min_t;
                    //std::cout << "smallest timestamp of left segment: " << segment_new_min_t << std::endl;
                }

                // Expand arc
                arc_left_idx = (arc_left_idx - 1 + kSmallCircleSize) % kSmallCircleSize;
                arc_left_value = sae_[y + kSmallCircle_[arc_left_idx][1]][x + kSmallCircle_[arc_left_idx][0]];
                if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
                    arc_left_min_t = arc_left_value;
                    //std::cout << "Updated smallest timestamp of right segment: " << arc_left_min_t << std::endl;
                }
            }
        }
        int newest_segment_size = kSmallMinThresh;

        // Further expand until completion of the circle
        for (; iteration < kSmallCircleSize; iteration++) {
            // Decide the most promising arc
            if (arc_right_value > arc_left_value) { // Right arc
                // Include arc in new segment
                if ((arc_right_value >= segment_new_min_t)) {
                    newest_segment_size = iteration + 1; // Check
                    if (arc_right_min_t < segment_new_min_t) {
                        segment_new_min_t = arc_right_min_t;
                    }
                }

                // Expand arc
                arc_right_idx = (arc_right_idx + 1) % kSmallCircleSize;
                arc_right_value = sae_[y + kSmallCircle_[arc_right_idx][1]][x + kSmallCircle_[arc_right_idx][0]];
                if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
                    arc_right_min_t = arc_right_value;
                }
            }
            else { // Left arc
                // Include arc in new segment
                if ((arc_left_value >= segment_new_min_t)) {
                    newest_segment_size = iteration + 1;
                    if (arc_left_min_t < segment_new_min_t) {
                        segment_new_min_t = arc_left_min_t;
                    }
                }

                // Expand arc
                arc_left_idx = (arc_left_idx - 1 + kSmallCircleSize) % kSmallCircleSize;
                arc_left_value = sae_[y + kSmallCircle_[arc_left_idx][1]][x + kSmallCircle_[arc_left_idx][0]];
                if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
                    arc_left_min_t = arc_left_value;
                }
            }
        }

        if (// Corners with newest segment of a minority of elements in the circle
            // These corners are equivalent to those in Mueggler et al. BMVC17
            (newest_segment_size <= kSmallMaxThresh) ||
            // Corners with newest segment of a majority of elements in the circle
            // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the large circle
            ((newest_segment_size >= (kSmallCircleSize - kSmallMaxThresh)) && (newest_segment_size <= (kSmallCircleSize - kSmallMinThresh)))) {
            is_arc_valid = true;
        }

        // Large Circle exploration
        if (is_arc_valid) {
            is_arc_valid = false;

            segment_new_min_t = sae_[y + kLargeCircle_[0][1]][x + kLargeCircle_[0][0]];
            arc_right_idx = 0;

            // Initialize in the newest element
            for (int i = 1; i < kLargeCircleSize; i++) {
                const double t_temp = sae_[y + kLargeCircle_[i][1]][x + kLargeCircle_[i][0]];
                if (t_temp > segment_new_min_t) {
                    segment_new_min_t = t_temp;
                    arc_right_idx = i; // % End up in the maximum value
                }
            }
            // Shift to the sides of the newest elements;
            arc_left_idx = (arc_right_idx - 1 + kLargeCircleSize) % kLargeCircleSize;
            arc_right_idx = (arc_right_idx + 1) % kLargeCircleSize;
            arc_left_value = sae_[y + kLargeCircle_[arc_left_idx][1]][x + kLargeCircle_[arc_left_idx][0]];
            arc_right_value = sae_[y + kLargeCircle_[arc_right_idx][1]][x + kLargeCircle_[arc_right_idx][0]];
            arc_left_min_t = arc_left_value;
            arc_right_min_t = arc_right_value;

            // Expand
            // Initial expand does not require checking
            iteration = 1;
            for (; iteration < kLargeMinThresh; iteration++) {
                // Decide the most promising arc
                if (arc_right_value > arc_left_value) { // Right arc
                    if (arc_right_min_t < segment_new_min_t) {
                        segment_new_min_t = arc_right_min_t;
                    }
                    // Expand arc
                    arc_right_idx = (arc_right_idx + 1) % kLargeCircleSize;
                    arc_right_value = sae_[y + kLargeCircle_[arc_right_idx][1]][x + kLargeCircle_[arc_right_idx][0]];
                    if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
                        arc_right_min_t = arc_right_value;
                    }
                }
                else { // Left arc
                    // Include arc in new segment
                    if (arc_left_min_t < segment_new_min_t) {
                        segment_new_min_t = arc_left_min_t;
                    }

                    // Expand arc
                    arc_left_idx = (arc_left_idx - 1 + kLargeCircleSize) % kLargeCircleSize;
                    arc_left_value = sae_[y + kLargeCircle_[arc_left_idx][1]][x + kLargeCircle_[arc_left_idx][0]];
                    if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
                        arc_left_min_t = arc_left_value;
                    }
                }
            }
            newest_segment_size = kLargeMinThresh;

            // Further expand until completion of the circle
            for (; iteration < kLargeCircleSize; iteration++) {
                // Decide the most promising arc
                if (arc_right_value > arc_left_value) { // Right arc
                    // Include arc in new segment
                    if ((arc_right_value >= segment_new_min_t)) {
                        newest_segment_size = iteration + 1;
                        if (arc_right_min_t < segment_new_min_t) {
                            segment_new_min_t = arc_right_min_t;
                        }
                    }

                    // Expand arc
                    arc_right_idx = (arc_right_idx + 1) % kLargeCircleSize;
                    arc_right_value = sae_[y + kLargeCircle_[arc_right_idx][1]][x + kLargeCircle_[arc_right_idx][0]];
                    if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
                        arc_right_min_t = arc_right_value;
                    }
                }
                else { // Left arc
                    // Include arc in new segment
                    if ((arc_left_value >= segment_new_min_t)) {
                        newest_segment_size = iteration + 1;
                        if (arc_left_min_t < segment_new_min_t) {
                            segment_new_min_t = arc_left_min_t;
                        }
                    }

                    // Expand arc
                    arc_left_idx = (arc_left_idx - 1 + kLargeCircleSize) % kLargeCircleSize;
                    arc_left_value = sae_[y + kLargeCircle_[arc_left_idx][1]][x + kLargeCircle_[arc_left_idx][0]];
                    if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
                        arc_left_min_t = arc_left_value;
                    }
                }
            }

            if (// Corners with newest segment of a minority of elements in the circle
                // These corners are equivalent to those in Mueggler et al. BMVC17
                (newest_segment_size <= kLargeMaxThresh) ||
                // Corners with newest segment of a majority of elements in the circle
                // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the small circle
                (newest_segment_size >= (kLargeCircleSize - kLargeMaxThresh) && (newest_segment_size <= (kLargeCircleSize - kLargeMinThresh)))) {
                is_arc_valid = true;
            }
        }

        return is_arc_valid;


}


//const int inner_circle_size = 16;
//const int outer_circle_size = 20;
//
//const int L_inner_min = 3;
//const int L_inner_max = 6;
//
//const int L_outer_min = 4;
//const int L_outer_max = 8;
//
////radius=3
//int inner_circle[inner_circle_size][2] =
//{
//    {0, 3}, { 1, 3 }, { 2, 2 }, { 3, 1 },
//    { 3, 0 }, { 3, -1 }, { 2, -2 }, { 1, -3 },
//    { 0, -3 }, { -1, -3 }, { -2, -2 }, { -3, -1 },
//    { -3, 0 }, { -3, 1 }, { -2, 2 }, { -1, 3 }
//};
//
////radius=4
//int outer_circle[outer_circle_size][2] =
//{
//    {0, 4}, { 1, 4 }, { 2, 3 }, { 3, 2 },
//    { 4, 1 }, { 4, 0 }, { 4, -1 }, { 3, -2 },
//    { 2, -3 }, { 1, -4 }, { 0, -4 }, { -1, -4 },
//    { -2, -3 }, { -3, -2 }, { -4, -1 }, { -4, 0 },
//    { -4, 1 }, { -3, 2 }, { -2, 3 }, { -1, 4 }
//};
//
//void arcstar() {
//    const int n_events = 17962477;
//
//    vector<bool> is_corner_list;
//
//    float SAE[2][HEIGHT][WIDTH] = { 0.0 };
//
//    //int shape[] = { HEIGHT,WIDTH };
//    //Mat	SAE[p] = Mat::zeros(2, shape, CV_32FC1);
//
//
//    ifstream file(EVENT_FILE);
//    if (file.is_open()) {
//        //some variables used to display progress
//        int numerator = 0;
//        int k = 0;
//        int max_k = n_events / 100;
//
//        string line;
//        while (getline(file, line)) {
//            //display progress
//            if (++k == max_k) {
//                cout << "..." << ++numerator << "/100" << "\r" << flush;
//                k = 0;
//            }
//
//            //extract event from current line
//            Event current = from_line(line);
//            int y = current.y;
//            int x = current.x;
//            bool p = current.p;
//            double t = current.t;
//
//            //update surface
//            SAE[p][y][x] = t;
//
//            //see if the current coordinates is too closed to border that can't form a proper circle
//            if (is_border(x, y, 4)) {
//                is_corner_list.push_back(false);
//                continue;
//            }
//
//            bool is_arc_valid = false;
//            // Small Circle exploration
//            //std::cout << "SMALL CIRCLE EXPLORATION" << std::endl;
//            // Initialize arc from newest element
//            double segment_new_min_t = SAE[p][y + inner_circle[0][1]][x + inner_circle[0][0]];
//
//            // Left and Right are equivalent to CW and CCW as in the paper
//            int arc_right_idx = 0;
//            int arc_left_idx;
//            // Find newest
//            for (int i = 1; i < inner_circle_size; i++) {
//                const int t_temp = SAE[p][y + inner_circle[i][1]][x + inner_circle[i][0]];
//                if (t_temp > segment_new_min_t) {
//                    segment_new_min_t = t_temp;
//                    arc_right_idx = i; // % End up in the maximum value
//                    //std::cout << "max right arc index is: " << arc_right_idx << std::endl;
//                }
//            }
//            // Shift to the sides of the newest element;
//            arc_left_idx = (arc_right_idx - 1 + inner_circle_size) % inner_circle_size;
//            arc_right_idx = (arc_right_idx + 1) % inner_circle_size;
//            double arc_left_value = SAE[p][y + inner_circle[arc_left_idx][1]][x + inner_circle[arc_left_idx][0]];
//            double arc_right_value = SAE[p][y + inner_circle[arc_right_idx][1]][x + inner_circle[arc_right_idx][0]];
//            double arc_left_min_t = arc_left_value;
//            double arc_right_min_t = arc_right_value;
//
//            // Expand
//            // Initial expand does not require checking
//            int iteration = 1; // The arc already contain the maximum
//            for (; iteration < L_inner_min; iteration++) {
//                // Decide the most promising arc
//                if (arc_right_value > arc_left_value) { // Right arc
//                    if (arc_right_min_t < segment_new_min_t) {
//                        segment_new_min_t = arc_right_min_t;
//                        //std::cout << "smallest timestamp of right segment: " << segment_new_min_t << std::endl;
//                    }
//                    // Expand arc
//                    arc_right_idx = (arc_right_idx + 1) % inner_circle_size;
//                    arc_right_value = SAE[p][y + inner_circle[arc_right_idx][1]][x + inner_circle[arc_right_idx][0]];
//                    if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
//                        arc_right_min_t = arc_right_value;
//                        //std::cout << "Updated smallest timestamp of right segment: " << arc_right_min_t << std::endl;
//                    }
//                }
//                else { // Left arc
//                    // Include arc in new segment
//                    if (arc_left_min_t < segment_new_min_t) {
//                        segment_new_min_t = arc_left_min_t;
//                        //std::cout << "smallest timestamp of left segment: " << segment_new_min_t << std::endl;
//                    }
//
//                    // Expand arc
//                    arc_left_idx = (arc_left_idx - 1 + inner_circle_size) % inner_circle_size;
//                    arc_left_value = SAE[p][y + inner_circle[arc_left_idx][1]][x + inner_circle[arc_left_idx][0]];
//                    if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
//                        arc_left_min_t = arc_left_value;
//                        //std::cout << "Updated smallest timestamp of right segment: " << arc_left_min_t << std::endl;
//                    }
//                }
//            }
//            int newest_segment_size = L_inner_min;
//
//            // Further expand until completion of the circle
//            for (; iteration < inner_circle_size; iteration++) {
//                // Decide the most promising arc
//                if (arc_right_value > arc_left_value) { // Right arc
//                    // Include arc in new segment
//                    if ((arc_right_value >= segment_new_min_t)) {
//                        newest_segment_size = iteration + 1; // Check
//                        if (arc_right_min_t < segment_new_min_t) {
//                            segment_new_min_t = arc_right_min_t;
//                        }
//                    }
//
//                    // Expand arc
//                    arc_right_idx = (arc_right_idx + 1) % inner_circle_size;
//                    arc_right_value = SAE[p][y + inner_circle[arc_right_idx][1]][x + inner_circle[arc_right_idx][0]];
//                    if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
//                        arc_right_min_t = arc_right_value;
//                    }
//                }
//                else { // Left arc
//                    // Include arc in new segment
//                    if ((arc_left_value >= segment_new_min_t)) {
//                        newest_segment_size = iteration + 1;
//                        if (arc_left_min_t < segment_new_min_t) {
//                            segment_new_min_t = arc_left_min_t;
//                        }
//                    }
//
//                    // Expand arc
//                    arc_left_idx = (arc_left_idx - 1 + inner_circle_size) % inner_circle_size;
//                    arc_left_value = SAE[p][y + inner_circle[arc_left_idx][1]][x + inner_circle[arc_left_idx][0]];
//                    if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
//                        arc_left_min_t = arc_left_value;
//                    }
//                }
//            }
//
//            if (// Corners with newest segment of a minority of elements in the circle
//                // These corners are equivalent to those in Mueggler et al. BMVC17
//                (newest_segment_size <= L_inner_max) ||
//                // Corners with newest segment of a majority of elements in the circle
//                // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the large circle
//                ((newest_segment_size >= (inner_circle_size - L_inner_max)) && (newest_segment_size <= (inner_circle_size - L_inner_min)))) {
//                is_arc_valid = true;
//            }
//
//            // Large Circle exploration
//            if (is_arc_valid) {
//                is_arc_valid = false;
//
//                segment_new_min_t = SAE[p][y + outer_circle[0][1]][x + outer_circle[0][0]];
//                arc_right_idx = 0;
//
//                // Initialize in the newest element
//                for (int i = 1; i < outer_circle_size; i++) {
//                    const int t_temp = SAE[p][y + outer_circle[i][1]][x + outer_circle[i][0]];
//                    if (t_temp > segment_new_min_t) {
//                        segment_new_min_t = t_temp;
//                        arc_right_idx = i; // % End up in the maximum value
//                    }
//                }
//                // Shift to the sides of the newest elements;
//                arc_left_idx = (arc_right_idx - 1 + outer_circle_size) % outer_circle_size;
//                arc_right_idx = (arc_right_idx + 1) % outer_circle_size;
//                arc_left_value = SAE[p][y + outer_circle[arc_left_idx][1]][x + outer_circle[arc_left_idx][0]];
//                arc_right_value = SAE[p][y + outer_circle[arc_right_idx][1]][x + outer_circle[arc_right_idx][0]];
//                arc_left_min_t = arc_left_value;
//                arc_right_min_t = arc_right_value;
//
//                // Expand
//                // Initial expand does not require checking
//                iteration = 1;
//                for (; iteration < L_outer_min; iteration++) {
//                    // Decide the most promising arc
//                    if (arc_right_value > arc_left_value) { // Right arc
//                        if (arc_right_min_t < segment_new_min_t) {
//                            segment_new_min_t = arc_right_min_t;
//                        }
//                        // Expand arc
//                        arc_right_idx = (arc_right_idx + 1) % outer_circle_size;
//                        arc_right_value = SAE[p][y + outer_circle[arc_right_idx][1]][x + outer_circle[arc_right_idx][0]];
//                        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
//                            arc_right_min_t = arc_right_value;
//                        }
//                    }
//                    else { // Left arc
//                        // Include arc in new segment
//                        if (arc_left_min_t < segment_new_min_t) {
//                            segment_new_min_t = arc_left_min_t;
//                        }
//
//                        // Expand arc
//                        arc_left_idx = (arc_left_idx - 1 + outer_circle_size) % outer_circle_size;
//                        arc_left_value = SAE[p][y + outer_circle[arc_left_idx][1]][x + outer_circle[arc_left_idx][0]];
//                        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
//                            arc_left_min_t = arc_left_value;
//                        }
//                    }
//                }
//                newest_segment_size = L_outer_min;
//
//                // Further expand until completion of the circle
//                for (; iteration < outer_circle_size; iteration++) {
//                    // Decide the most promising arc
//                    if (arc_right_value > arc_left_value) { // Right arc
//                        // Include arc in new segment
//                        if ((arc_right_value >= segment_new_min_t)) {
//                            newest_segment_size = iteration + 1;
//                            if (arc_right_min_t < segment_new_min_t) {
//                                segment_new_min_t = arc_right_min_t;
//                            }
//                        }
//
//                        // Expand arc
//                        arc_right_idx = (arc_right_idx + 1) % outer_circle_size;
//                        arc_right_value = SAE[p][y + outer_circle[arc_right_idx][1]][x + outer_circle[arc_right_idx][0]];
//                        if (arc_right_value < arc_right_min_t) { // Update minimum of the arc
//                            arc_right_min_t = arc_right_value;
//                        }
//                    }
//                    else { // Left arc
//                        // Include arc in new segment
//                        if ((arc_left_value >= segment_new_min_t)) {
//                            newest_segment_size = iteration + 1;
//                            if (arc_left_min_t < segment_new_min_t) {
//                                segment_new_min_t = arc_left_min_t;
//                            }
//                        }
//
//                        // Expand arc
//                        arc_left_idx = (arc_left_idx - 1 + outer_circle_size) % outer_circle_size;
//                        arc_left_value = SAE[p][y + outer_circle[arc_left_idx][1]][x + outer_circle[arc_left_idx][0]];
//                        if (arc_left_value < arc_left_min_t) { // Update minimum of the arc
//                            arc_left_min_t = arc_left_value;
//                        }
//                    }
//                }
//
//                if (// Corners with newest segment of a minority of elements in the circle
//                    // These corners are equivalent to those in Mueggler et al. BMVC17
//                    (newest_segment_size <= L_outer_max) ||
//                    // Corners with newest segment of a majority of elements in the circle
//                    // This can be commented out to decrease noise at expenses of less repeatibility. If you do, DO NOT forget to comment the equilvent line in the small circle
//                    (newest_segment_size >= (outer_circle_size - L_outer_max) && (newest_segment_size <= (outer_circle_size - L_outer_min)))) {
//                    is_arc_valid = true;
//                }
//            }
//
//            is_corner_list.push_back(is_arc_valid);
//        }
//
//
//
//
//        file.close();
//
//        save_result(is_corner_list, -1.1,detector);
//        save_result_with_filtering(is_corner_list, -1.1, detector);
//
//    }
//    else {
//        cout << "File not found: EVENT_FILE" << endl;
//    }
//}
