#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {

    ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	//cam.initGrabber(640, 480);
	ofBackground(0);
    
    ofFileDialogResult r = ofSystemLoadDialog();
    if(r.bSuccess){
        if(player.loadMovie(r.getPath())){
            outputFbo.allocate(player.getWidth(), player.getHeight(), GL_RGB, 4);
            player.play();
        }
    }
    
	tracker.setup();
	tracker.setRescale(.5);
}

void testApp::update() {
    if(!player.isLoaded()) return;
    
	player.update();
	if(player.isFrameNew()) {
		tracker.update(toCv(player));
	}
}

void testApp::draw() {
    
    
    if(!player.isLoaded()) return;

	player.draw(0, 0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	
	ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
	ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
	ofPolyline faceOutline = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
    
	outputFbo.begin();
    ofClear(0,0,0,0);
    
    ofPushStyle();

    //ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    ofSetColor(0,0,255,255);
    ofBeginShape();
    ofVertices(faceOutline.getVertices());
    ofEndShape();
    
	ofSetColor(255,0,0,255);
    ofBeginShape();
    ofVertices(leftEye.getVertices());
    ofEndShape();
    
	ofSetColor(255,0,0,255);
    ofBeginShape();
    ofVertices(rightEye.getVertices());
    ofEndShape();

    ofPopStyle();
    
    outputFbo.end();
    
    
    outputFbo.getTextureReference().draw(0, 0);
    
    
	ofSetLineWidth(1);
	ofSetColor(255);
	//tracker.draw();
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
	}
}