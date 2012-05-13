#pragma once

#include "ofMain.h"
#include "ofxGameCamera.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDMediaTake.h"
#include "ofxGui.h"

#include "ofxTimeline.h"
#include "ofxTLDepthImageSequence.h"
#include "ofxTLCameraTrack.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxGui.h"

#include "CloudInterludeParticleGenerator.h"
#include "CloudInterludeForcePerlin.h"
#include "CloudInterludeForceDrag.h"
#include "CloudInterludeForceMeshAttractor.h"

class testApp : public ofBaseApp
{
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
    
    void copyVertsToMesh();
    void debugDrawOrigins();
    void loadShaders();
    

    //gui elements
    ofxPanel panel;
    ofxToggle lockToTrackToggle;
    ofxToggle useShaderToggle;
    ofxButton reloadShaders;
    ofxToggle renderOutput;
    ofxButton setCompDirectory;
    ofxButton resetCamera;
    ofxButton clear;
    ofxButton useDofBlur;
    ofxToggle showType;
    ofxToggle freezeParticles;
    ofxToggle useColors;
    ofxButton loadPalette;
    ofxToggle drawPointcloud;
    ofxToggle applyMeshDistort;
    ofxToggle depthRangeMultiply;
    ofxButton setImplodePoint;
    ofxToggle drawImplodeDebug;
    ofVec3f implodePoint;
    
    ofxParameter<float> cameraSpeed;
    ofxSlider<float> cameraSpeedSlider;
    
    float luminosityChannel;
    float distortionChannel;
    bool saveCameraPoint;

    int framesSaved;
    bool createdFolder;
    string currentSaveFolder;

    ofxTimeline timeline;
    ofxTLCameraTrack track;
    ofxTLDepthImageSequence depthImages;

    ofFbo renderTarget;
    ofFbo focusChannel;
    ofFbo modelTarget;
    ofFbo blurBuffer;
    ofFbo fbo2;
    ofImage colorPalette;
    
    ofxGameCamera cam;
    ofxRGBDRenderer renderer;
    ofxRGBDMediaTake take;
    
    ofImage savingImage;
    
    ofRectangle fboRect;
    
    //forces
    vector<CloudInterludeParticleGenerator> emmiters;
    CloudInterludeForcePerlin* perlinForce;
    CloudInterludeForceDrag* dragForce;
    CloudInterludeForceMeshAttractor* meshForce;
    
    ofShader pointCloudDOF;
    ofShader dofRange;
    ofShader dofBlur;
    
    int totalParticles;
    
    ofVboMesh mesh;
};
