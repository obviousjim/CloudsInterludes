#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {

    ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	//cam.initGrabber(640, 480);
	ofBackground(0);
    
    inFace = false;
    lastFrameFound = 0;
    foundFirstFace = false;
    
    /*
    ofFileDialogResult r = ofSystemLoadDialog();
    if(r.bSuccess){
        if(player.loadMovie(r.getPath())){
            outputFbo.allocate(player.getWidth(), player.getHeight(), GL_RGB, 4);
            player.play();
        }
    }
    */

	tracker.setup();
	tracker.setRescale(.5);
    
//    startRender();
}

void testApp::update() {
    if(!player.isLoaded()) return;
    
//	player.update();
//	if(player.isFrameNew()) {
//		foundFace = tracker.update(toCv(player));
//	}
}

void testApp::draw() {
    
    
    if(!player.isLoaded()) return;

	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);	
	ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
	ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
	ofPolyline faceOutline = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
    
    
    renderOutlinesToFBO(leftEye, rightEye, faceOutline);

    
    cout << " face outline " << faceOutline.getVertices().size() << " right eye " << rightEye.getVertices().size() << endl;
    
    ofDisableAlphaBlending();
    outputFbo.getTextureReference().draw(0, 0);
    
    
	ofSetLineWidth(1);
	ofSetColor(255);
	//tracker.draw();
}

void testApp::startRender(){
    string videoPath = "/Volumes/IMPOSTER/BruceSterlingAustin/TAKE_07_08_14_37_24/color/MVI_1158.MOV";
    int startFrame = 12000;
    int endFrame = 13500;
    string outputPath = "/Users/focus/Documents/RGBDExperimentalBin/NormalOutput/BruceFace2";
    if(player.loadMovie(videoPath)){
        player.setFrame(startFrame);
        outputFbo.allocate(player.getWidth(), player.getHeight(), GL_RGB);
        outputImage.setUseTexture(false);
        outputImage.allocate(player.getWidth(), player.getHeight(), OF_IMAGE_COLOR);
        
        int currentFrame = player.getCurrentFrame();
        while(currentFrame < endFrame){
            player.update();
            bool foundFace = tracker.update(toCv(player));
            
            if(foundFace){
                ofPolyline leftEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
                ofPolyline rightEye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);
                ofPolyline faceOutline = tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
                
                if(!inFace && foundFirstFace){
                    ofPolyline interpLeftEye;
                    ofPolyline interpRightEye;
                    ofPolyline interpFace;
                    for(int i = lastFrameFound+1; i < currentFrame; i++){
                        float delta = ofMap(i, lastFrameFound, currentFrame, 0, 1.0);
                        
                        interpolatePolyLine(lastLeftEye, leftEye, interpLeftEye, delta);
                        interpolatePolyLine(lastRightEye, rightEye, interpRightEye, delta);
                        interpolatePolyLine(lastFace, faceOutline, interpFace, delta);
                        
                        renderOutlinesToFBO(interpLeftEye, interpRightEye, interpFace);
                        saveImage(outputPath, i);
                    }
                }
                
                foundFirstFace = true;
                
                lastLeftEye = leftEye;
                lastRightEye = rightEye;
                lastFace = faceOutline;
                
                renderOutlinesToFBO(leftEye, rightEye, faceOutline);
                //save FBO image
                saveImage(outputPath, currentFrame);
                
                lastFrameFound = currentFrame;
            }
            
            inFace = foundFace;
            player.nextFrame();
            currentFrame = player.getCurrentFrame();
        }
    }
}

void testApp::interpolatePolyLine(ofPolyline& a, ofPolyline& b, ofPolyline& out, float delta){
    if(a.getVertices().size() != b.getVertices().size()){
        ofLogError("Polylines did not match in size");
        return;
    }
    
    out.clear();
    
    for(int i = 0; i < a.getVertices().size(); i++){
        out.addVertex( a.getVertices()[i].getInterpolated(b.getVertices()[i], delta) );
    }
}

void testApp::saveImage(string path, int frame){
    char filename[1024];
    sprintf(filename, "%s/faceframe_%05d.png", path.c_str(), frame);
    outputFbo.readToPixels(outputImage.getPixelsRef());
    outputImage.saveImage(filename);
}

void testApp::renderOutlinesToFBO(ofPolyline& leftEye, ofPolyline& rightEye, ofPolyline& faceOutline){
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
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
	}
    
    if(key == 'E'){
        startRender();
    }
}
