/**
 * Example - Scene With Timeline
 * This example shows how to create a basic RGB+D scene
 * with a timeline to scrub through the video.
 *
 * It requiers ofxGameCamera https://github.com/Flightphase/ofxGameCamera 
 * and ofxTimeline https://github.com/Flightphase/ofxTimeline in addition to ofxRGBDepth
 *
 * James George 2012 
 * Released under the MIT License
 *
 * The RGBDToolkit has been developed with support from the STUDIO for Creative Inquiry and Eyebeam
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxGameCamera.h"
#include "ofxRGBDPlayer.h"
#include "ofxRGBDMeshBuilder.h"
#include "ofxRGBDScene.h"
#include "ofxGui.h"
#include "ofxTimeline.h"
#include "ofxTLCameraTrack.h"
#include "ParticleRenderer.h"

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
    
    void exit();
    
	void updateParticleSystem();
	void updatePerlinLuminosity();
	
	float luminosityChannel;
	
    ofxRGBDPlayer player;
    ofxRGBDMeshBuilder meshBuilder;
    ofxGameCamera cam;

    ofxPanel gui;
    ofxIntSlider simplify;
    ofxToggle drawWireframe;
	ofxToggle drawPointcloud;
	ofxToggle drawParticles;
	ofxToggle freezeParticles;
	ofxToggle supressPlay;
	
	ofxIntSlider numAmbientParticles;
	ofxIntSlider particleBoxWidth;
	ofxIntSlider particleBoxHeight;
	ofxIntSlider particleBoxDepth;
	ofxFloatSlider particleMaxDistance;
	ofxFloatSlider particleMaxSize;
	
	ofxFloatSlider particleFade;
	//START NODE STRUCTURES
	
	ParticleRenderer particleRenderer;
	
	ofxTimeline timeline;
	ofxTLCameraTrack camTrack;
	bool writeInfoXml;
	void drawGeometry();
	void bangFired(ofxTLBangEventArgs& bang);
	
	map<string, string> scenePaths;
	
	bool justLoadedNewScene;
	
    bool loadNewScene();
    bool loadDefaultScene();
    bool loadScene(string takeDirectory);
    ofFbo renderTarget;
	ofRectangle fboRect;
	
	ofShader cloudShader;
	ofShader ambientShader;

	ofImage saveImage;
	bool rendering;
	string renderFolder;
	int currentFrameNumber;
	ofImage textureReplacement;
	
	void loadShader();
	
	void generateAmbientParticles();
	ofVboMesh ambientParticles;
	
	void generateClusters();
	void generateSubClusters(ofVec3f center, int maxSubs, float maxRadius);
	ofVboMesh clusters;
	vector<ofVec3f> clusterCenters;

	ofVboMesh randomUnits;
};
