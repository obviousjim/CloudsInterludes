#pragma once

#include "ofMain.h"
#include "Node.h"
#include "ofxGui.h"
#include "ofxGameCamera.h"
#include "ofxTimeline.h"
#include "ofxTLCameraTrack.h"

class testApp : public ofBaseApp {
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void exit();
	
	bool rendering;
	int frameNum;
	ofxPanel gui;
	ofxIntSlider heroNodes;
	ofxIntSlider numIterations;
	ofxFloatSlider heroRadius;
	ofxFloatSlider heroRadiusVariance;
	ofxIntSlider numBranches;
	ofxFloatSlider minDistance;
	ofxFloatSlider distanceRange;
	ofxFloatSlider stepSize;
	ofxFloatSlider lineThickness;
	ofxFloatSlider minAttractRadius;
	ofxFloatSlider minRepelRadius;
	ofxFloatSlider minFuseRadius;
	
	ofxFloatSlider maxAttractForce;
	ofxFloatSlider maxRepelForce;
	
	ofxFloatSlider maxTraverseDistance;
	
	ofxIntSlider numSurvivingBranches;
	ofxIntSlider numPointsAtReplicate;
	ofxFloatSlider replicatePointDistance;
	ofxFloatSlider pointSize;
	ofxFloatSlider maxTraverseAngle;
	
	ofxTLCameraTrack camTrack;
	ofxGameCamera cam;
	ofFbo renderTarget;
	ofxTimeline timeline;
	ofRectangle fboRect;
	
	vector<Node*> nodes;
	vector<ofVec3f> fusePoints;
	ofVboMesh geometry;
	ofVboMesh points;
	ofVboMesh traversal;
	void traverse();
	void generate();
	
	string renderFolder;
};
