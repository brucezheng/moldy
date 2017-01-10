#include "ofApp.h"
#include <sstream>


//extern int height, width, num_threads;

//--------------------------------------------------------------
void ofApp::setup(){
	A_vals = std::vector<std::vector<double>>(h+2, std::vector<double>(w+2, 1));
	B_vals = std::vector<std::vector<double>>(h+2, std::vector<double>(w+2, 0));
	
	A_delta = std::vector<std::vector<double>>(h+2, std::vector<double>(w+2, 0));
	B_delta = std::vector<std::vector<double>>(h+2, std::vector<double>(w+2, 0));
	
	gettimeofday(&last_fup, NULL);
	randomPopulate(n_rand_pop);
	
	img.allocate(w, h, OF_IMAGE_COLOR);
	
	ofSetVerticalSync(false);
}

void ofApp::resetMatrix() {
	for(int i = 1; i < h+1; ++i) {
		for(int j = 1; j < w+1; ++j) {
			A_vals[i][j] = 1;
			B_vals[i][j] = 0;
			
			A_delta[i][j] = 0;
			B_delta[i][j] = 0;
		}
	}
}

void ofApp::place(int x, int y) {
		B_vals[x-1][y-1] = 1;
		B_vals[x+1][y-1] = 1;
		B_vals[x-1][y+1] = 1;
		B_vals[x+1][y+1] = 1;
}

void ofApp::randomPopulate(int num) {
	for (int i = 0; i < num; i++) {
		int rx = (int) rand() % (h-2) + 2;
		int ry = (int) rand() % (w-2) + 2;
		place(rx,ry);
	}
}

double avg2(std::vector<double> const& v) {
    int n = 0;
    double mean = 0.0;
    for (auto x : v) {
        double delta = x - mean;
        mean += delta/++n;
    }
    return mean;
}
  
void ofApp::quit() {
	printf("Avg framerate: %.2f\n",avg2(frame_rates));
	std::exit(0);
}
  
//--------------------------------------------------------------
void ofApp::update(){
	static int update_at = f_num_refresh;
	if (f_num_refresh != 0 && frame_rates.size() == update_at) {
		//printf("refresh %i %i\n", frame_rates.size(), update_at);
		if(frame_rates.size() == f_refresh_quit * f_num_refresh) {
			quit();
		}
		update_at += f_num_refresh;
		resetMatrix();
		randomPopulate(n_rand_pop);
	}
	timeval now;
	gettimeofday(&now, NULL);
	int milliSeconds = (now.tv_sec - last_fup.tv_sec) * 1000 + (now.tv_usec - last_fup.tv_usec)/1000;
	if (f_refresh_rate != 0 && milliSeconds > f_refresh_rate) {
		frame_rates.push_back(int(ofGetFrameRate()));
		if(print_frame) {
			if(n_frame_skip != 0) {
				printf("%i (%i)\n", int(ofGetFrameRate()), int(ofGetFrameRate()) * (n_frame_skip+1));
			} else {
			printf("%i\n", int(ofGetFrameRate()));
			}
			fflush(stdout);
		}
		last_fup = now;
	}
	
	for(int repeat = 0; repeat < n_frame_skip + 1; ++repeat) {
		if(synchronized) {
			// A_vals and B_vals updated at once
			for(int i = 1; i < h+1; ++i) {
				for(int j = 1; j < w+1; ++j) {
					/*
					double a_ = A_vals[i][j];
					double b_ = B_vals[i][j];
					A_delta[i][j] = (da*A_sum(i,j)) - (a_*b_*b_) + (f*(1-a_));
					B_delta[i][j] = (db*B_sum(i,j)) + (a_*b_*b_) - ((k+f)*b_);
					*/
					double a_ = A_vals[i][j];
					double b_ = B_vals[i][j];
					double A_sum = 	  0.05 * (A_vals[i-1][j-1] + A_vals[i-1][j+1] + A_vals[i+1][j-1] + A_vals[i+1][j+1]) 
									+ 0.2  * (A_vals[i-1][j]   + A_vals[i+1][j]   + A_vals[i][j+1]   + A_vals[i][j-1]) 
									- (a_);
					double B_sum = 	  0.05 * (B_vals[i-1][j-1] + B_vals[i-1][j+1] + B_vals[i+1][j-1] + B_vals[i+1][j+1]) 
									+ 0.2  * (B_vals[i-1][j]   + B_vals[i+1][j]   + B_vals[i][j+1]   + B_vals[i][j-1]) 
									- (b_);
					double abb_ = a_*b_*b_;
					
					A_delta[i][j] = (da*A_sum) - (abb_) + (f*(1-a_));
					B_delta[i][j] = (db*B_sum) + (abb_) - ((k+f)*b_);
				}
			}
			for(int i = 1; i < h+1; ++i) {
				for(int j = 1; j < w+1; ++j) {
					A_vals[i][j] += A_delta[i][j];
					B_vals[i][j] += B_delta[i][j];
				}
			}
		}
		else {
			// A_vals and B_vals updated continuously (technically incorrect!!!)
			for(int i = 1; i < h+1; ++i) {
				for(int j = 1; j < w+1; ++j) {
					/*
					double a_ = A_vals[i][j];
					double b_ = B_vals[i][j];
					A_vals[i][j] = a_ + (da*A_sum(i, j)) - (a_*b_*b_) + (f*(1-a_));
					B_vals[i][j] = b_ + (db*B_sum(i, j)) + (a_*b_*b_) - ((k+f)*b_);
					*/
					double a_ = A_vals[i][j];
					double b_ = B_vals[i][j];
					double A_sum = 	  0.05 * (A_vals[i-1][j-1] + A_vals[i-1][j+1] + A_vals[i+1][j-1] + A_vals[i+1][j+1]) 
									+ 0.2  * (A_vals[i-1][j]   + A_vals[i+1][j]   + A_vals[i][j+1]   + A_vals[i][j-1]) 
									- (a_);
					double B_sum = 	  0.05 * (B_vals[i-1][j-1] + B_vals[i-1][j+1] + B_vals[i+1][j-1] + B_vals[i+1][j+1]) 
									+ 0.2  * (B_vals[i-1][j]   + B_vals[i+1][j]   + B_vals[i][j+1]   + B_vals[i][j-1]) 
									- (b_);
					double abb_ = a_*b_*b_;
					
					A_vals[i][j] = a_ + (da*A_sum) - (abb_) + (f*(1-a_));
					B_vals[i][j] = b_ + (db*B_sum) + (abb_) - ((k+f)*b_);
				}
			}
		}
		update_border();
	}
}

void ofApp::update_border() {
	if(edge_handle == 0) {
		A_vals[0][0] = 1;
		A_vals[h+1][0] = 1;
		A_vals[0][w+1] = 1;
		A_vals[h+1][w+1] = 1;
		
		B_vals[0][0] = 0;
		B_vals[h+1][0] = 0;
		B_vals[0][w+1] = 0;
		B_vals[h+1][w+1] = 0;
		
		for(int i = 1; i < h+1; ++i) {
			A_vals[i][0] = 1; // bottom
			B_vals[i][0] = 0; 
			A_vals[i][w+1] = 1; // top
			B_vals[i][w+1] = 0;
		}
		for(int j = 1; j < w+1; ++j) {
			A_vals[0][j] = 1; // left
			B_vals[0][j] = 0; 
			A_vals[h+1][j] = 1; // right
			B_vals[h+1][j] = 0; 
		}
	}
	else if (edge_handle == 1) {
		A_vals[0][0] = A_vals[h][w];
		A_vals[h+1][0] = A_vals[1][w];
		A_vals[0][w+1] = A_vals[h][1];
		A_vals[h+1][w+1] = A_vals[1][1];
		
		B_vals[0][0] = B_vals[h][w];
		B_vals[h+1][0] = B_vals[1][w];
		B_vals[0][w+1] = B_vals[h][1];
		B_vals[h+1][w+1] = B_vals[1][1];
		
		for(int i = 1; i < h+1; ++i) {
			A_vals[i][0] = A_vals[i][w]; // bottom
			B_vals[i][0] = B_vals[i][w]; 
			A_vals[i][w+1] = A_vals[i][1]; // top
			B_vals[i][w+1] = B_vals[i][1]; 
		}
		for(int j = 1; j < w+1; ++j) {
			A_vals[0][j] = A_vals[h][j]; // left
			B_vals[0][j] = B_vals[h][j]; 
			A_vals[h+1][j] = A_vals[1][j]; // right
			B_vals[h+1][j] = B_vals[1][j]; 
		}
	}
	else {
		// corners!
		A_vals[0][0] = A_vals[1][1];
		A_vals[h+1][0] = A_vals[h][1];
		A_vals[0][w+1] = A_vals[1][w];
		A_vals[h+1][w+1] = A_vals[h][w];
		
		B_vals[0][0] = B_vals[1][1];
		B_vals[h+1][0] = B_vals[h][1];
		B_vals[0][w+1] = B_vals[1][w];
		B_vals[h+1][w+1] = B_vals[h][w];
		
		for(int i = 1; i < h+1; ++i) {
			A_vals[i][0] = A_vals[i][1]; // bottom
			B_vals[i][0] = B_vals[i][1]; 
			A_vals[i][w+1] = A_vals[i][w]; // top
			B_vals[i][w+1] = B_vals[i][w]; 
		}
		for(int j = 1; j < w+1; ++j) {
			A_vals[0][j] = A_vals[1][j]; // left
			B_vals[0][j] = B_vals[1][j]; 
			A_vals[h+1][j] = A_vals[h][j]; // right
			B_vals[h+1][j] = B_vals[h][j]; 
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	unsigned char* pix = img.getPixels().getData();
	for(int i = 1; i < h+1; i++) {
		for(int j = 1; j < w+1; j++) {
			double a = A_vals[i][j];
			
			/*
			double b = B_vals[i][j];
			
			b = 1-b;
			b = b*b;
			b = b*b;
			b = b*b;
			ofColor color = ofColor(b*255, b*a*255, a*255);
		
			double red = std::min(1.0,2.0*a);
			double green = std::min(1.0,a+b);
			ofColor color = ofColor(red*255, green*255, 255);
			
			*/
			double scale = a*255;
			ofColor color = ofColor(scale, scale, scale);
			*(pix++) = color.r;
			*(pix++) = color.g;
			*(pix++) = color.b;
		}
	}
	img.update();
	img.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'c') {
		resetMatrix();
		randomPopulate(n_rand_pop);
	}
	if(key == 'q') {
		quit();
	}
	if(key == 's') {
		synchronized = !synchronized;
		resetMatrix();
		randomPopulate(n_rand_pop);
		printf("synchronized: %i\n", synchronized);
		fflush(stdout);
	}	
	if(key == 'e') {
		edge_handle = (edge_handle + 1) % 3;
		resetMatrix();
		randomPopulate(n_rand_pop);
		const char* msg;
		switch(edge_handle) {
			case 0:  msg = "empty"; break;
			case 1:  msg = "extend"; break;
			case 2:  msg = "wrap-around"; break;
			default: msg = "????"; break;
		}
		printf("edge handling: %s\n", msg);
		fflush(stdout);
	}	
	if(key == '-') {
		n_frame_skip -= 1;
		if(n_frame_skip < 0) n_frame_skip = 0;
		printf("frames skipped per iteration: %i\n", n_frame_skip);
	}
	if(key == '=') {
		n_frame_skip += 1;
		printf("frames skipped per iteration: %i\n", n_frame_skip);
	}
	if(key == '0') {
		n_frame_skip = 0;
		printf("frames skipped per iteration: %i\n", n_frame_skip);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}