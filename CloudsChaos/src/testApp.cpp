#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofBackground(30);
    
    ofToggleFullscreen();
    
    renderTarget.allocate(1920, 1080, GL_RGBA, 4);
    
    renderer.setSimplification(2);
    //renderer.forceUndistortOff = true;
    
    cam.setup();
    cam.usemouse = true;
    cam.autosavePosition = true;
    cam.speed = 20;
    cam.setScale(1, -1, 1);
    cam.targetNode.setScale(1, -1, 1);
    cam.loadCameraPosition();
	cam.maximumY =  120;
	cam.minimumY = -120;
	cam.rollSpeed = 1;
    cam.loadCameraPosition();
    
    timeline.setup();
    timeline.setDurationInFrames(500);
    timeline.setOffset(ofVec2f(0,0));
    
    int absoluteMaxParticles = 100000;
    depthImages.setup();
    timeline.setPageName("Source");
    timeline.addElement("Depth", &depthImages);
    timeline.addKeyframes("Simplify", "zthreshold.xml", ofRange(0, 8 ), 2);
    timeline.addKeyframes("Z Threshold", "zthreshold.xml", ofRange(0, sqrtf(5000) ), sqrtf(5000));
    timeline.addKeyframes("Edge Snip", "edgesnip.xml", ofRange(0, sqrtf(2000) ), sqrtf(2000));
        
    timeline.addPage("Particles");
    timeline.addKeyframes("Max Particles", "maxParticles.xml", ofRange(1000, absoluteMaxParticles) );
    timeline.addKeyframes("Birthrate", "particleBirthrate.xml", ofRange(.1, sqrtf(10)) );
    timeline.addKeyframes("Lifespan", "particleLifespan.xml", ofRange(2, 300) );
    timeline.addKeyframes("Lifespan Variance", "particleLifespanVariance.xml", ofRange(0, 100) );
    timeline.addKeyframes("Drag Force", "particleDragFroce.xml", ofRange(0, 1.0), 0);
    
    timeline.addPage("Perlin");
    timeline.addKeyframes("Perlin Amplitude", "perlinAmplitude.xml", ofRange(1, sqrtf(200)) );
    timeline.addKeyframes("Perlin Density", "perlinDensity.xml", ofRange(100, sqrtf(2000)));
    timeline.addKeyframes("Perlin Speed", "perlinSpeed.xml", ofRange(0, sqrtf(5)), 0);
    
    timeline.addPage("Attractors");
    timeline.addKeyframes("Mesh Attract", "meshAttractScale.xml", ofRange(0, 2000));
    timeline.addKeyframes("Min Radius", "meshMinRadius.xml", ofRange(0, 6000));

    timeline.setCurrentPage(0);
    
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
    
    //setup forces
    perlinForce = new CloudInterludeForcePerlin();
    dragForce = new CloudInterludeForceDrag();
    meshForce = new CloudInterludeForceMeshAttractor();
    meshForce->mesh = &renderer.getMesh();
    
    for(int i = 0; i < 640*480; i++){
    	CloudInterludeParticleGenerator g;
        g.addForce(perlinForce);
        g.addForce(dragForce);
//        g.addForce(meshForce);
        emmiters.push_back(g);
    }
    
    for(int i = 0; i < absoluteMaxParticles; i++){
    	mesh.addVertex(ofVec3f(0,0,0));
        mesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
    }

//    generator.addForce(perlinForce);
//    generator.addForce(dragForce);
//    generator.addForce(meshForce);
//    
//    generator.position = ofVec3f(0,0,0);
//    generator.direction = ofVec3f(0,0,1);

}

//--------------------------------------------------------------
void testApp::update(){
    
    //VIEW
    cam.applyRotation = cam.applyTranslation = fboRect.inside(ofGetMouseX(),ofGetMouseY());
    fboRect = ofRectangle(timeline.getDrawRect().x, timeline.getDrawRect().height, 1920/2, 1080/2);
    
    
    dragForce->dragForce = powf( timeline.getKeyframeValue("Drag Force"), 2.0);
    
    perlinForce->amplitude = powf( timeline.getKeyframeValue("Perlin Amplitude"), 2.0);
    perlinForce->density = powf( timeline.getKeyframeValue("Perlin Density"), 2.0);
    perlinForce->speed = powf( timeline.getKeyframeValue("Perlin Speed"), 2.0);
    
    meshForce->minRadius = timeline.getKeyframeValue("Min Radius");
    meshForce->attractScale = timeline.getKeyframeValue("Mesh Attract");

    //GENERATOR
    float birthRate = powf(timeline.getKeyframeValue("Birthrate"), 2);
    float lifeSpan = timeline.getKeyframeValue("Lifespan");
    float lifeSpanVariance = timeline.getKeyframeValue("Lifespan Variance");
    int maxParticles = timeline.getKeyframeValue("Max Particles");
    
//    for(int i = 0; i < emitters.size(); i++){    
//    	emitters[i].enabled = false;
//    }

    totalParticles = 0;
    for(int i = 0; i < renderer.getTotalPoints(); i++){
        CloudInterludeParticleGenerator& g = emmiters[i];
        bool valid = renderer.isVertexValid(i);
        totalParticles += g.particles.size();
        if(valid){
            g.birthRate = birthRate; //disable invisible verts
            g.lifespan  = lifeSpan;
            g.lifespanVariance = lifeSpanVariance;
            g.position = renderer.getMesh().getVertex( renderer.vertexIndex(i) );
            g.remainingParticles = maxParticles - totalParticles;
        }
        else{
            g.birthRate = 0;
        }
        g.update();
    }
    
    //put the particles in the mesh;
    copyVertsToMesh();
        
    //RENDERER
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
//        renderer.drawWireFrame(false);
        ofPopStyle();
        
        //for(int i = 0; i < debugNodes.size(); i++) debugNodes[i].draw();
    }
    
    //RENDERER
    glPointSize(4);
    glPushMatrix();
    glScalef(1,-1,1);
    mesh.drawVertices();
    glPopMatrix();
    
    //debugDrawOrigins();
	
    /*
    glBegin(GL_POINTS);
    glPointSize(4);
    glColor3f(1.0, 0, 0);
    int totalValidVerts = 0;
    for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
    	if(renderer.isVertexValid(i) ){
            totalValidVerts++;
            ofVec3f vert = renderer.getMesh().getVertex( renderer.vertexIndex(i) );
        	glVertex3f(vert.x, vert.y, vert.z);
        }
    }
    glEnd();    
    cout << "valid verts " << totalValidVerts << endl;
    */
    
    cam.end();
    
    renderTarget.end();
    
    ofPushStyle();
    ofSetColor(0);
    ofRect(fboRect);
    ofPopStyle();

    renderTarget.getTextureReference().draw(fboRect); 
    
    ofDrawBitmapString("totalParticles " + ofToString(totalParticles ), fboRect.x+fboRect.width + 10, fboRect.y + 15);
    
    timeline.draw();
}

void testApp::debugDrawOrigins(){
    for(int i = 0; i < emmiters.size(); i++){
        for(int v = 0; v < emmiters[i].particles.size(); v++){
            ofLine(emmiters[i].particles[v].position,
                   emmiters[i].particles[v].origin);
        }	
    }
}

void testApp::copyVertsToMesh(){
    int meshIndex = 0;    
    vector<ofVec3f>& meshVertices = mesh.getVertices();
    vector<ofFloatColor>& meshColors = mesh.getColors();
    
    for(int i = 0; i < emmiters.size(); i++){

        for(int v = 0; v < emmiters[i].particles.size(); v++){
            meshVertices[meshIndex] = emmiters[i].particles[v].position;
            float color = emmiters[i].particles[v].energy / emmiters[i].particles[v].initialEnergy;
            meshColors[meshIndex] = ofFloatColor(color,color,color,color);
            meshIndex++;
//            if(meshIndex == meshVertices.size()){
//                return;
//            }
        }
    }
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