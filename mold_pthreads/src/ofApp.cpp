#include "ofApp.h"
#include <sstream>
#include <pthread.h>
#include <algorithm>

//extern int height, width, num_threads;

//--------------------------------------------------------------
void ofApp::setup(){
	A_vals = std::vector<std::vector<double>>(w, std::vector<double>(h, 1));
	B_vals = std::vector<std::vector<double>>(w, std::vector<double>(h, 0));
	
	A_delta = std::vector<std::vector<double>>(w, std::vector<double>(h, 0));
	B_delta = std::vector<std::vector<double>>(w, std::vector<double>(h, 0));
	
	/*
	*/
	
	gettimeofday(&last_fup, NULL);
	randomPopulate(10);
	
	img.allocate(w, h, OF_IMAGE_COLOR);
	
	ofSetFrameRate(320);
}

void ofApp::resetMatrix() {
	for(int i = 0; i < w; ++i) {
		for(int j = 0; j < h; ++j) {
			A_vals[i][j] = 1;
			B_vals[i][j] = 0;
			
			A_delta[i][j] = 0;
			B_delta[i][j] = 0;
		}
	}
}

double RandomDouble(double a, double b) {
    double random = ((double) rand()) / (double) RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}

void ofApp::randomPopulate(int num) {
	for (int i = 0; i < num; i++) {
		int rx = (int) rand() % (w-2) + 1;
		int ry = (int) rand() % (h-2) + 1;
		double c = RandomDouble(0,0.7);
		// Dot placement
		if (c < 0.1) { 
			B_vals[rx][ry] = 1;
		}
		// X placement
		else if (c < 0.2) {
			B_vals[rx][ry] = 1;
			B_vals[rx+1][ry+1] = 1;
			B_vals[rx-1][ry-1] = 1;
			B_vals[rx+1][ry-1] = 1;
			B_vals[rx-1][ry+1] = 1;
		}
		// / Placement
		else if (c < 0.3) {
			B_vals[rx][ry] = 1;
			B_vals[rx-1][ry+1] = 1;
			B_vals[rx+1][ry-1] = 1;
		}
		// \ Placement
		else if (c < 0.4) {
			B_vals[rx][ry] = 1;
			B_vals[rx-1][ry-1] = 1;
			B_vals[rx+1][ry+1] = 1;
		}
		// | placement
		else if (c < 0.5) {
			B_vals[rx][ry] = 1;
			B_vals[rx][ry-1] = 1;
			B_vals[rx][ry+1] = 1;
		}
		// - placement
		else if (c < 0.6) {
			B_vals[rx][ry] = 1;
			B_vals[rx-1][ry] = 1;
			B_vals[rx+1][ry] = 1;
		}
		// # placement
		else if (c < 0.7) {
			B_vals[rx-1][ry-1] = 1;
			B_vals[rx+1][ry-1] = 1;
			B_vals[rx-1][ry+1] = 1;
			B_vals[rx+1][ry+1] = 1;
		}
	}
}

double A_sum(int x, int y) {
    double sum = 0;
	if( (x-1 < 0) || (y-1 < 0) || 
		(x+1 > w-1) || (y+1 > h-1) )
		return 0;
	for(int i = -1; i <= 1; ++i) {
		for(int j = -1; j <= +1; ++j) {
			sum += A_vals[x+i][y+j] * convolution[i+1][j+1];
		}
	}
    return sum;
}

double B_sum(int x, int y) {
    double sum = 0;
	if( (x-1 < 0) || (y-1 < 0) || 
		(x+1 > w-1) || (y+1 > h-1) )
		return 0;
	for(int i = -1; i <= 1; ++i) {
		for(int j = -1; j <= +1; ++j) {
			sum += B_vals[x+i][y+j] * convolution[i+1][j+1];
		}
	}
    return sum;
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

struct thread_data{
   int start;
   int end;
};

void *update_lines(void *threadarg) {
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	for(int i = my_data->start; i < my_data->end; ++i) {
		for(int j = 0; j < h; ++j) {
			double a_ = A_vals[i][j];
			double b_ = B_vals[i][j];
			A_delta[i][j] = (da*A_sum(i, j)) - (a_*b_*b_) + (f*(1-a_));
			B_delta[i][j] = (db*B_sum(i, j)) + (a_*b_*b_) - ((k+f)*b_);
			A_vals[i][j] = a_ + A_delta[i][j];
			B_vals[i][j] = b_ + B_delta[i][j];
		}
	}
	pthread_exit(NULL);
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
		randomPopulate(10);
	}
	timeval now;
	gettimeofday(&now, NULL);
	int milliSeconds = (now.tv_sec - last_fup.tv_sec) * 1000 + (now.tv_usec - last_fup.tv_usec)/1000;
	if (f_refresh_rate != 0 && milliSeconds > f_refresh_rate) {
		frame_rates.push_back(int(ofGetFrameRate()));
		if(print_frame) {
			printf("%i\n", int(ofGetFrameRate()));
			fflush(stdout);
		}
		last_fup = now;
	}
	/*
	for(int i = 0; i < w; ++i) {
		for(int j = 0; j < h; ++j) {
			double a_ = A_vals[i][j];
			double b_ = B_vals[i][j];
			A_delta[i][j] = (da*sum(i, j, 'A')) - (a_*b_*b_) + (f*(1-a_));
			B_delta[i][j] = (db*sum(i, j, 'B')) + (a_*b_*b_) - ((k+f)*b_);
		}
	}
	*/
	int rc;
	
   pthread_t threads[num_threads];
	struct thread_data td[num_threads];
   pthread_attr_t attr;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i=0; i < num_threads; i++ ){
      //cout << "main() : creating thread, " << i << endl;
		td[i].start = w * i / num_threads;
		td[i].end = (i+1) * w / num_threads;
		rc = pthread_create(&threads[i], NULL, update_lines, (void *)&td[i]);
		
      if (rc){
        cout << "Error:unable to create thread," << rc << endl;
         std::exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
	
   for(int i=0; i < num_threads; i++ ){
      rc = pthread_join(threads[i], &status);
		
      if (rc){
         cout << "Error:unable to join," << rc << endl;
         std::exit(-1);
      }
   }
	
	/*
	for(int i = 0; i < w; ++i) {
		for(int j = 0; j < h; ++j) {
			A_vals[i][j] += A_delta[i][j];
			B_vals[i][j] += B_delta[i][j];
		}
	}
	*/
}


//--------------------------------------------------------------
void ofApp::draw(){
	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			//int scale = A_vals[i][j] * 255;
			double del_a = A_delta[i][j];
			int scale = 255 - std::min(int(del_a * 255 * 200), 255);
			//int scale = std::min(std::abs(int(del_a * 255 * 200)), 255);
			//int scale = std::max(0,std::min(std::abs(int(del_a * 255 * 200)), 255));
			//int scale = std::min(std::abs(int(del_a * 255 * 200)), 255);
			ofColor color = ofColor(scale, scale, scale);
			/*
			double a = A_vals[i][j];
			double b = B_vals[i][j];
			ofColor color = ofColor(std::min(a * 512,double(255)), 
				std::min((a + b) * 255, double(255)), 255);
				*/
			img.setColor(i, j, color);
		}
	}
	img.update();
	img.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'c') {
		resetMatrix();
		randomPopulate(10);
	}
	if(key == 'q') {
		quit();
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