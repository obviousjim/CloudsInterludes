#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
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
    
    useShader = false;
    
    int absoluteMaxParticles = 40000;
    depthImages.setup();
    timeline.setPageName("Source");
    timeline.addElement("Depth", &depthImages);
    timeline.addKeyframes("Simplify", "zthreshold.xml", ofRange(0, 8 ), 2);
    timeline.addKeyframes("Z Threshold", "zthreshold.xml", ofRange(0, sqrtf(5000) ), sqrtf(5000));
    timeline.addKeyframes("Edge Snip", "edgesnip.xml", ofRange(0, sqrtf(2000) ), sqrtf(2000));


    timeline.addPage("Particles");
    timeline.addKeyframes("Max Particles", "maxParticles.xml", ofRange(10, absoluteMaxParticles) );
    timeline.addKeyframes("Birthrate", "particleBirthrate.xml", ofRange(.001, .01) );
    timeline.addKeyframes("Lifespan", "particleLifespan.xml", ofRange(2, 300) );
    timeline.addKeyframes("Lifespan Variance", "particleLifespanVariance.xml", ofRange(0, 100) );
    timeline.addKeyframes("Drag Force", "particleDragFroce.xml", ofRange(0, 1.0), 0);
    
    timeline.addPage("DOF");
	timeline.addKeyframes("Focal Distance", "dofFocalDistance.xml", ofRange(0, 2000));
    timeline.addKeyframes("Focal Range", "dofFocalRange.xml", ofRange(0, 500));
    timeline.addKeyframes("Min Point Size", "dofMinPoint.xml", ofRange(1, 5));
    timeline.addKeyframes("Max Point Size", "dofMaxPoint.xml", ofRange(10, 45));
    
    timeline.addPage("Perlin");
    timeline.addKeyframes("Perlin Amplitude", "perlinAmplitude.xml", ofRange(0, sqrtf(200)) );
    timeline.addKeyframes("Perlin Density", "perlinDensity.xml", ofRange(0, sqrtf(2000)));
    timeline.addKeyframes("Perlin Speed", "perlinSpeed.xml", ofRange(0, sqrtf(5)), 0);
    
    timeline.addPage("Attractors");
    timeline.addKeyframes("Mesh Attract", "meshAttractScale.xml", ofRange(0, 2000));
    timeline.addKeyframes("Min Radius", "meshMinRadius.xml", ofRange(0, 6000));
    timeline.setCurrentPage(0);
    
//    timeline.addPage("Luminosity");
//	timeline.addKeyframes("Lumin Mid", "Lumin Mid", <#ofRange valueRange#>)
    
    ofxXmlSettings defaults;
    defaults.loadFile("defaults.xml");
    string defaultTake = defaults.getValue("take", "");
    if(defaultTake != "" && take.loadFromFolder(defaultTake)){

        renderer.setup(take.calibrationDirectory);
        depthImages.loadSequence(take.depthFolder);                
        
        renderer.setDepthImage(depthImages.currentDepthRaw);
        timeline.setDurationInFrames(depthImages.videoThumbs.size());
        
//        alignment.loadPairingFile(take.pairingsFile);
        
//        movie.loadMovie(take.lowResVideoPath);
//        movie.setUseTexture(false);
//        movie.play();
//        movie.setSpeed(0);
        
//        playerElement.setVideoPlayer(movie, take.videoThumbsPath);
//		timeline.addElement("Video", &playerElement);
//        timeline.setDurationInFrames(movie.getDuration());
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
    dragForce   = new CloudInterludeForceDrag();
    meshForce   = new CloudInterludeForceMeshAttractor();
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

    loadShaders();
}

//--------------------------------------------------------------
void testApp::update(){
    
    if(!take.valid()){
        return;
    }
    
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
    float birthRate = timeline.getKeyframeValue("Birthrate");
    float lifeSpan  = timeline.getKeyframeValue("Lifespan");
    float lifeSpanVariance = timeline.getKeyframeValue("Lifespan Variance");
    int maxParticles = timeline.getKeyframeValue("Max Particles");
    
//    for(int i = 0; i < emitters.size(); i++){    
//    	emitters[i].enabled = false;
//    }

    totalParticles = 0;
    for(int i = 0; i < emmiters.size(); i++){
    	emmiters[i].birthRate = 0;
        totalParticles += emmiters[i].particles.size();
    }

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
    }
    
    for(int i = 0; i < emmiters.size(); i++){
    	emmiters[i].update();
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
    
//    movie.update();
    if(take.valid() && depthImages.isFrameNew()){
        renderer.setDepthImage(depthImages.currentDepthRaw);
        needsUpdate = true;
    }
    
    if(needsUpdate){
        renderer.update();
    }
}

void testApp::loadShaders(){
	pointCloudDOF.load("shaders/DOFCloud");
}

//--------------------------------------------------------------
void testApp::draw(){
    
    renderTarget.begin();
    ofClear(0);
    
    cam.begin(ofRectangle(0,0,1920,1080));
//    ofDrawGrid();
    
    if(depthImages.isLoaded()){
        ofPushStyle();
        ofSetColor(255);
//        renderer.drawWireFrame(false);
        ofPopStyle();
        //for(int i = 0; i < debugNodes.size(); i++) debugNodes[i].draw();
    }
    
    //RENDERER
    //glPointSize(4);
    ofPushStyle();
    glPushMatrix();
    glScalef(1,-1,1);
    if(useShader){
        pointCloudDOF.begin();
        pointCloudDOF.setUniform1f("minSize", timeline.getKeyframeValue("Min Point Size"));
        pointCloudDOF.setUniform1f("maxSize", timeline.getKeyframeValue("Max Point Size"));
        pointCloudDOF.setUniform1f("focalRange", timeline.getKeyframeValue("Focal Range"));
        pointCloudDOF.setUniform1f("focalDistance", timeline.getKeyframeValue("Focal Distance"));
    }
    else{
    	glPointSize(4);
    }
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    glEnable(GL_POINT_SMOOTH); // makes circular points
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
    mesh.drawVertices();
    if(useShader){
        pointCloudDOF.end();    	
    }
    glPopMatrix();
    ofPopStyle();
    
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
            if(meshIndex == meshVertices.size()){
                ofLogError("exceeded max particles");
                return;
            }
        }
    }
	
    memset(&(meshColors[meshIndex].r), 0, sizeof(ofFloatColor)*(meshColors.size()-meshIndex));
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
//		if(movie.getSpeed() != 0.0){
//			movie.setSpeed(0);
//		}
//		else{
//			movie.play();
//			movie.setSpeed(1.0);
//		}
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    
    if(key == 'R'){
        cam.reset();
    }
    
    if(key == 'K'){
        for(int i = 0; i < emmiters.size(); i++){
        	emmiters[i].particles.clear();
        }
    }
    
    if(key == 'L'){
    	loadShaders();
    }
    
    if(key == 'N'){
    	useShader = !useShader;
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