#pragma once

#define _BSD_SOURCE

#include <sys/time.h>

#include "ofMain.h"
#include <vector>
#include <ctime>

extern double f, k, da, db; 
extern int w, h;
extern int f_num_refresh, f_refresh_rate, f_refresh_quit;
extern int n_rand_pop, n_frame_skip;
extern bool print_frame, synchronized;
extern int edge_handle;

class ofApp : public ofBaseApp{
	private:
		std::vector<std::vector<double>> A_vals, B_vals, A_delta, B_delta;
		ofImage img;
		int num_threads;
		double convolution[3][3] = {{0.05, 0.2, 0.05}, 
							   {0.2, -1, 0.2}, 
							   {0.05, 0.2, 0.05}};
		double A_sum(int x, int y);
		double B_sum(int x, int y);
		void randomPopulate(int num);
		void resetMatrix();
		void update_border();
		std::vector<double> frame_rates;
		timeval last_fup;
	public:
		void setup();
		void update();
		void draw();
		void quit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
};
