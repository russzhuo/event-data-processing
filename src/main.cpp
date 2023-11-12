//#include "arcstar.h"
#include "eHarris.h"
#include "arcstar.h"
#include "eFast.h"
#include "luvHarris.h"
#include "process.h"
#include <iostream>

int main(){
	//int n_events = 17962477;
	int n_events = numb_lines_dynamic_6dof;
	//float threshold = 30.0;

	int n_threads= 6;
	int k_tos = 3;	

	//eHarris(2.3, n_events);
	//luvHarris(n_threads,5, k_tos,n_events);
	//arcstar(n_events);
	eFast(n_events);






	//arcstar(n_events);
	//eHarris_accumulated(0.5, n_events);	
	//eHarris_accumulated(1, n_events);

	//eHarris_accumulated(12, n_events);
	//eHarris_accumulated(16,n_events);

	//luvHarris(n_threads, -5, k_tos, n_events);
	//luvHarris(n_threads, 5, k_tos, n_events);
	//luvHarris(n_threads, 10, k_tos, n_events);
	//luvHarris(n_threads, 15, k_tos, n_events);
	//luvHarris(n_threads, 20, k_tos, n_events);
	//luvHarris(n_threads, 27, k_tos, n_events);
	//luvHarris(n_threads, 30, k_tos, n_events);

	//luvHarris_accumulated(n_threads, 15, k_tos, n_events);
	//luvHarris_accumulated(n_threads, 60, k_tos, n_events);
	//luvHarris_accumulated(n_threads, 80, k_tos, n_events);

	//for (float threshold = -0.2; threshold < 2.25; threshold += 0.17) {
	//	luvHarris_accumulated(n_threads, threshold, k_tos, n_events);
	//}
	//luvHarris(n_threads, 50.0, k_tos, n_events);
	//luvHarris(n_threads, 60.0, k_tos, n_events);
	//luvHarris(n_threads, 70.0, k_tos, n_events);	
	//luvHarris(n_threads, 85.0, k_tos, n_events);

	//luvHarris(n_threads, -15.0, k_tos, n_events);
	//luvHarris(n_threads, 12.0, k_tos, n_events);
	//luvHarris(n_threads, 20.0, k_tos, n_events);
	//luvHarris(n_threads, 27.0, k_tos, n_events);
	//luvHarris(n_threads, 32.0, k_tos, n_events);
	//luvHarris(n_threads, 42.0, k_tos, n_events);

	//eHarris(-1.0);
	//eHarris(-5.0);
	//eHarris(-10.0);
	//for (float threshold = 1.77; threshold < 3; threshold += 0.25) {
	//	eHarris(threshold);
	//}
	//luvHarris(n_threads, 53.0, k_tos);
	//arcstar();
	//arcstar();
	//luvHarris(n_threads, 48.5, k_tos);
	//luvHarris(n_threads, 49.5, k_tos);
	//luvHarris(n_threads, 48, k_tos);

	//for (float threshold = -30.0; threshold < 135.0; threshold += 5.0) {
	//	luvHarris(n_threads, threshold, k_tos);
	//}

	//eHarris_v2(0.2);
	//eHarris_v2(0.3);
	//eHarris_v2(0.4);
	//eHarris_v2(0.5);
	//eHarris_v2(2.0);	
	//eHarris_v2(1.5);
	//eHarris_v2(3.0);
	//eHarris_v2(4.0);
	//eHarris_v2(5.0);
	//eHarris(-0.01);
	//eHarris(0.035);
	//eHarris(0.036);
	//double gradient_dy[window_length][window_length] = { 0 };
	//double gradient_dx[window_length][window_length] = { 0 };




}

