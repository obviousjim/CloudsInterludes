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
    int lastFrameFound;

    ofFbo outputFbo;
    void startRender();
    void renderOutlinesToFBO(ofPolyline& leftEye, ofPolyline& rightEye, ofPolyline& faceOutline);
    bool inFace;
    bool foundFirstFace;
    
    ofPolyline lastRightEye;
    ofPolyline lastLeftEye;
    ofPolyline lastFace;
    ofImage outputImage;
    void interpolatePolyLine(ofPolyline& a, ofPolyline& b, ofPolyline& out, float delta);
    void saveImage(string path, int frame);
};
