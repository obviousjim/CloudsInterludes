#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofBackground(100);
    
    ofToggleFullscreen();
    
    renderTarget.allocate(1920, 1080, GL_RGBA, 4);
    
    renderer.setSimplification(2);
    renderer.forceUndistortOff = true;
    
    cam.setup();
    cam.usemouse = true;
    cam.loadCameraPosition();
    cam.autosavePosition = true;
    cam.speed = 20;
    cam.setScale(1, -1, 1);
    cam.targetNode.setScale(1, -1, 1);
    
    timeline.setup();
    timeline.setDurationInFrames(500);
    timeline.setOffset(ofVec2f(0,0));
    
    depthImages.setup();
    timeline.setPageName("Source");
    timeline.addElement("Depth", &depthImages);
    timeline.addKeyframes("Simplify", "zthreshold.xml", ofRange(0, 8 ), 2);
    timeline.addKeyframes("Z Threshold", "zthreshold.xml", ofRange(0, sqrtf(5000) ), sqrtf(5000));
    timeline.addKeyframes("Edge Snip", "edgesnip.xml", ofRange(0, sqrtf(2000) ), sqrtf(2000));
    
//    timeline.addKeyframes("X Rot", "xrot.xml", ofRange(0, 360 ), 0);
//    timeline.addKeyframes("Y Rot", "yrot.xml", ofRange(0, 360 ), 0);
//    timeline.addKeyframes("Z Rot", "zrot.xml", ofRange(0, 360 ), 0);
    
    ofxXmlSettings defaults;
    defaults.loadFile("defaults.xml");
    string defaultTake = defaults.getValue("take", "");
    if(defaultTake != "" && take.loadFromFolder(defaultTake)){

        renderer.setup(take.calibrationDirectory);
        depthImages.loadSequence(take.depthFolder);                
        
        renderer.setDepthImage(depthImages.currentDepthRaw);
        timeline.setDurationInFrames(depthImages.videoThumbs.size());
    }
    
    for(int i = 0; i < 50; i++){
        ofNode n;
        n.setPosition(ofRandom(-200,200),
                      ofRandom(-200,200),
                      ofRandom(-200,200) );
        debugNodes.push_back( n );
    }
}

//--------------------------------------------------------------
void testApp::update(){
    fboRect = ofRectangle(timeline.getDrawRect().x,
                          timeline.getDrawRect().height, 1920/2, 1080/2);
    
    cam.applyRotation = cam.applyTranslation = fboRect.inside(ofGetMouseX(),ofGetMouseY());
    
    bool needsUpdate = false;
    
    renderer.farClip = powf(timeline.getKeyframeValue("Z Threshold"), 2.0);
    renderer.edgeCull = powf(timeline.getKeyframeValue("Edge Snip"), 2.0);
    if(renderer.getSimplification() != (int)timeline.getKeyframeValue("Simplify")){
        renderer.setSimplification( (int)timeline.getKeyframeValue("Simplify") );
    }

    if(timeline.getUserChangedValue()){
        needsUpdate = true;
    }
    
    if(depthImages.isLoaded() && depthImages.isFrameNew()){
        renderer.setDepthImage(depthImages.currentDepthRaw);
        needsUpdate = true;
    }
    
    if(needsUpdate){
        renderer.update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    renderTarget.begin();
    ofClear(0);
    
    cam.begin(ofRectangle(0,0,1920,1080));
    ofDrawGrid();
    
    if(depthImages.isLoaded()){
        ofPushStyle();
        ofSetColor(255);
        renderer.drawWireFrame(false);
        ofPopStyle();
        
        //for(int i = 0; i < debugNodes.size(); i++) debugNodes[i].draw();
    }
    
    cam.end();
    renderTarget.end();
    renderTarget.getTextureReference().draw(fboRect); 
    
    timeline.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //Load a file
    if(key == 'D'){
        ofFileDialogResult d = ofSystemLoadDialog("Take", true);
        if(d.bSuccess && take.loadFromFolder(d.getPath())){
                
            renderer.setup(take.calibrationDirectory);
            depthImages.loadSequence(take.depthFolder);                
            
            renderer.setDepthImage(depthImages.currentDepthRaw);
            timeline.setDurationInFrames(depthImages.videoThumbs.size());
            
            ofxXmlSettings defaults;
            defaults.setValue("take", d.getPath());
            defaults.saveFile("defaults.xml");
        }
    }
    
    if(key == ' '){
        timeline.togglePlay();
    }   
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    
    if(key == 'R'){
        cam.reset();
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
    timeline.setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}