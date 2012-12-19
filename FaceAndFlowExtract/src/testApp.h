#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"

class testApp : public ofBaseApp {
  public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
    
	ofVideoPlayer player;
	ofxFaceTracker tracker;
    
	ofxCv::FlowPyrLK pyrLk;

    ofFbo outputFbo;
    
};
