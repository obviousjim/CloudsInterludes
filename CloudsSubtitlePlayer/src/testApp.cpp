

#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    
    ofxXmlSettings settings;
    subtitles.loadFont("AxisStd-Regular.otf", 30);
    if(settings.loadFile("localsettings.xml")){
	    p.loadMovie(settings.getValue("video", ""), OF_QTKIT_DECODE_TEXTURE_ONLY);
        p.setLoopState(OF_LOOP_NORMAL);
        p.play();
        subtitles.load(settings.getValue("titles", ""));
        p.play();
    }

}

//--------------------------------------------------------------
void testApp::update(){
    if(p.isLoaded()){
        if(ofGetMousePressed()){
            p.setPosition(1.0*ofGetMouseX()/ofGetWidth());
        }
		p.update();
        subtitles.setTimeInSeconds(p.getPosition()*p.getDuration());
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if(p.isLoaded()){
        ofRectangle drawRect(0,0, p.getWidth(), p.getHeight());
        ofRectangle screenRect(0,0,ofGetWidth(), ofGetHeight());
        drawRect = screenRect.scaleIntoMe(drawRect);
        
		p.draw(drawRect.x,drawRect.y,drawRect.width,drawRect.height);
        
        ofPoint drawPoint = ofPoint(ofGetWidth()/2, ofGetHeight() * .8);
        
        ofSetColor(0);
        subtitles.draw(drawPoint+ofPoint(1.5,1.5));
        ofSetColor(255);
        subtitles.draw(drawPoint);
    }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ' && p.isLoaded()){
		if(p.isPlaying()){
            p.stop();
        }
        else{
            p.play();
        }
    }
    if(key == 'f'){
        ofToggleFullscreen();
    }
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
    
    for(int i = 0; i < dragInfo.files.size(); i++){
        string filename = dragInfo.files[i];
        string extension = ofToLower( ofFilePath::getFileExt(filename) );
		if (extension == "mov" || extension == "mp4"){
            p.loadMovie(filename, OF_QTKIT_DECODE_TEXTURE_ONLY);
            p.play();
            p.setLoopState(OF_LOOP_NORMAL);
            ofxXmlSettings settings;
            settings.loadFile("localsettings.xml");
            settings.setValue("video", filename);
            settings.saveFile();
        }
        else if( extension == "srt"){
            subtitles.load(filename);
            ofxXmlSettings settings;
            settings.loadFile("localsettings.xml");
            settings.setValue("titles", filename);
            settings.saveFile();
        }
    }
}