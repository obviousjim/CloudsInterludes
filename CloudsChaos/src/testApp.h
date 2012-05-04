#pragma once

#include "ofMain.h"
#include "ofxGameCamera.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDMediaTake.h"

#include "ofxTimeline.h"
#include "ofxTLDepthImageSequence.h"

class testApp : public ofBaseApp{
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
    
    ofxTimeline timeline;
    
    ofFbo renderTarget;
    ofxGameCamera cam;
    ofxRGBDRenderer renderer;
    ofxRGBDMediaTake take;
    ofxTLDepthImageSequence depthImages;
    vector<ofNode> debugNodes;
    
    ofRectangle fboRect;
};
