#include "ofMain.h"
#include "ofApp.h"

#include <map>
#include <fstream>

int w, h;
double f, k, da, db;

int f_num_refresh, f_refresh_rate, f_refresh_quit;
bool print_frame, synchronized;
int edge_handle;
int n_rand_pop, n_frame_skip;

//========================================================================
int main(){
	std::map<std::string,float> params;
	std::string p;
	float v;
	
	std::ifstream infile("../params.txt");
	while (infile >> p >> v) {
		params[p] = v;
	}
	infile.close();
	
	f = params["fill_rate"] ? params["fill_rate"] : 0.0545;
	k = params["kill_rate"] ? params["kill_rate"] : 0.0615;
	
	da = params["da"] ? params["da"] : 1;
	db = params["db"] ? params["db"] : 0.5;
	
	w = params["width"] ? int(params["width"]) : 300;
	h = params["height"] ? int(params["height"]) : 300;
	
	f_num_refresh = params["frame_num_refresh"] ? int(params["frame_num_refresh"]) : 0;
	f_refresh_rate = params["frame_refresh_rate"] ? int(params["frame_refresh_rate"]) : 0;
	f_refresh_quit = params["quit_after_n_refresh"] ? int(params["quit_after_n_refresh"]) : 0;
	
	print_frame = params["print_frame_rate"] ? bool(params["print_frame_rate"]) : false;
	
	n_rand_pop = params["n_rand_pop"] ? int(params["n_rand_pop"]) : 10;
	n_frame_skip = params["n_frame_skip"] ? int(params["n_frame_skip"]) : 0;
	
	synchronized = params["synchronized"] ? bool(params["synchronized"]) : false;
	edge_handle = params["edge_handle"] ? int(params["edge_handle"]) : 10;
	//width = height = 300;
/*
	num_threads = 0;
	height = width = 300;
	if (argc >= 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}
	*/
	ofSetupOpenGL(w, h, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp() );

}
