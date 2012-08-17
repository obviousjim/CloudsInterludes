
#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofSetFrameRate(30);
    p.loadMovie("/Users/focus/Desktop/Clouds_Eyebeam5.10.12_LowRes.mov");
	p.setFrame(4000);
    p.play();

	subtitles.setup("Clouds_20minSub_FINAL.srt","sazanami-gothic.ttf", 12);
    ftglFontTest.loadFont("sazanami-gothic.ttf", 30);
}

//--------------------------------------------------------------
void testApp::update(){
	p.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	p.draw(0,0, 1280, 720);
    subtitles.setTimeInSeconds(p.getPosition()*p.getDuration());
    subtitles.drawToScreen(ofGetWidth()/2, ofGetHeight()/2);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}