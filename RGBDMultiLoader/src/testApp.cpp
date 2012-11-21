/**
 * Example - Mesh Builder
 * This example shows how to create a RGBD Mesh on the CPU
 *
 *
 * James George 2012 
 * Released under the MIT License
 *
 * The RGBDToolkit has been developed with support from the STUDIO for Creative Inquiry and Eyebeam
 */

#include "testApp.h"

//http://www.cs.cmu.edu/~mws/rpos.html
ofVec3f ofRandomPointOnUnitSphere(){
	float phi = 2*PI*ofRandomuf();
	float theta = acos(ofRandomf());
	return ofVec3f(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
}

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofBackground(25);
	ofEnableSmoothing();
	
	justLoadedNewScene = false;
	rendering = false;

    //set up the game camera
    cam.setup();
    cam.speed = 20;
//    cam.autosavePosition = true;
//	cam.loadCameraPosition();
    cam.targetNode.setPosition(ofVec3f());
    cam.targetNode.setOrientation(ofQuaternion());
    cam.targetXRot = -180;
    cam.targetYRot = 0;
    cam.rotationZ = 0;    
    
	ofxTimeline::removeCocoaMenusFromGlut("RGBDMultiLoader");
	
	timeline.setup();
	timeline.setFrameBased(true);
	timeline.setDurationInSeconds(60);
	timeline.setFrameRate(24);

	timeline.setPageName("Camera");
	timeline.addTrack("cam", &camTrack);
	timeline.addFlags("scene");
	timeline.addFlags("notes");
	timeline.addCurves("x shift", ofRange(-.15,.15), 0);
	timeline.addCurves("y shift", ofRange(-.15,.15), 0);
	timeline.addCurves("x offset", ofRange(-1000,  1000), 0);
	timeline.addCurves("z offset", ofRange(0, 2000));
	timeline.addSwitches("pause video");
	
	timeline.addPage("Rendering");
	timeline.addCurves("Z Threshold", ofRange(500, 2000), 1200);
    timeline.addCurves("Point Alpha");
	timeline.addCurves("Point Size", ofRange(.5, 10));
    timeline.addCurves("Wireframe Alpha");
	timeline.addCurves("Wireframe Thickness",ofRange(.5, 5));

	timeline.addPage("Particles");
	timeline.addCurves("particle fade", ofRange(0, 1.0));
	timeline.addCurves("particle size max", ofRange(.5, 10),7);
	timeline.addCurves("particle size min", ofRange(.5, 5), 2);
	timeline.addCurves("birth rate", ofRange(0, .4));
	timeline.addCurves("life span", ofRange(0, 300));
	timeline.addCurves("life span var", ofRange(0, 1.0));
	
	timeline.addPage("Particle Forces");
	timeline.addCurves("perlin amp", ofRange(0, 1));
	timeline.addCurves("perlin speed", ofRange(0,1));
	timeline.addCurves("perlin density", ofRange(0, 1000));
	timeline.addCurves("gravity amp", ofRange(-.5, .5), 0.0);
	timeline.addCurves("spin force", ofRange(-.5, .5), 0.0);
	timeline.addCurves("explode force", ofRange(0, 20), 0.0);
	timeline.addCurves("explode var", ofRange(0, 1.));
	timeline.addCurves("z wind", ofRange(0, 2.));
	
    timeline.addPage("Wireframe Flicker");
    timeline.addCurves("Wireframe Perlin Density", "wireframePerlinDensity.xml", ofRange(10, 3000.0), 0.0);
    timeline.addCurves("Wireframe Perlin Speed", "wireframePerlinSpeed.xml", ofRange(0, .2), 0.0);
	timeline.addCurves("Wireframe Lumin Min", "wireframeLuminMin.xml", ofRange(0, .5), 0.0);
	timeline.addCurves("Wireframe Lumin Mid", "wireframeLuminMid.xml", ofRange(0, 1.0), .5);
    timeline.addCurves("Wireframe Lumin Contrast", "wireframeLuminContrast.xml", ofRange(0, 10.0), 1.0);
    timeline.addCurves("Wireframe Lumin Exponent", "wireframeLuminExponent.xml", ofRange(1.0, 5.0), 0.0);
	
	timeline.setCurrentPage(0);
	
	timeline.setMinimalHeaders(true);

	camTrack.setCamera(cam);
	
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
	
	luminosityChannel = 0;
	
	meshBuilder.addColors = true;
	meshBuilder.cacheValidVertices = true;
	meshBuilder.getHoleFiller().enable = true;

	renderTarget.allocate(1920, 1080, GL_RGB, 4);
    simplify = 1;
	
	scenePaths["jer"]   = "/Volumes/IMPOSTER/EyeoMediaBin/Jer03"; //6297 - 6751
	scenePaths["bruce"] = "/Volumes/IMPOSTER/BruceSterlingAustin/TAKE_07_08_14_15_35";
	scenePaths["paola"] = "/Volumes/IMPOSTER/EyeoMediaBin/Paola03";
	scenePaths["sofy"]  = "/Volumes/IMPOSTER/ITPClouds/TAKE_07_12_21_07_00"; //9190
	
    gui.setup("tests");
    gui.add(simplify.setup("simplify", ofxParameter<int>(), 1, 8));
    gui.add(drawWireframe.setup("draw wireframe",ofxParameter<bool>()));
	gui.add(drawPointcloud.setup("draw pointcloud",ofxParameter<bool>()));
	gui.add(drawParticles.setup("draw particles",ofxParameter<bool>()));
	gui.add(supressPlay.setup("supress play",ofxParameter<bool>()));

	gui.add(numAmbientParticles.setup("num ambient", ofxParameter<int>(), 500, 200000));
	gui.add(particleBoxWidth.setup("ap box width", ofxParameter<int>(), 500, 3000));
	gui.add(particleBoxHeight.setup("ap box height", ofxParameter<int>(), 500, 3000));
	gui.add(particleBoxDepth.setup("ap box depth", ofxParameter<int>(), 3000, 10000));

	
	gui.add(particleMaxDistance.setup("max particle distance", ofxParameter<float>(), 0, 1000));
	gui.add(particeMaxSize.setup("max particle size", ofxParameter<float>(), .5, 9));
	gui.add(particleFade.setup("particle alpha", ofxParameter<float>(), 0, 1.0));

    gui.loadFromFile("defaultSettings.xml");
    
	currentFrameNumber = 0;
	
	particleRenderer.meshBuilder = &meshBuilder;
	particleRenderer.setup(100000);
	
	loadShader();
	
    //attemping to load the last scene
    loadDefaultScene();
	
	if(numAmbientParticles < 0 ) numAmbientParticles = 50000;
	generateAmbientParticles();
	
	cout << "generated " << clusters.getVertices().size() << " cluster points " << endl;
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
}


void testApp::generateAmbientParticles(){
	ambientParticles.clear();
	/*
	for(int i = 0; i < numAmbientParticles; i++){
		ambientParticles.addVertex(ofVec3f(ofRandom(-particleBoxWidth, particleBoxWidth),
										   ofRandom(-particleBoxHeight, particleBoxHeight),
										   ofRandom(0, particleBoxDepth)));
	}
	 */
	
	int partcleBoxStepSize = 50;
	for(int z = 0; z < particleBoxDepth; z+=partcleBoxStepSize){
		for(int y = -particleBoxHeight; y < particleBoxHeight; y+=partcleBoxStepSize){
			for(int x = -particleBoxWidth; x < particleBoxWidth; x+=partcleBoxStepSize){
				ambientParticles.addVertex(ofVec3f(x,y,z) + ofVec3f(ofRandomf(),ofRandomf(),ofRandomf())*2);
				ambientParticles.addColor(ofFloatColor(1));
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::generateClusters(){
	for(int i = 0; i < 25; i++){
		ofVec3f cluster = ofVec3f(ofRandom(-3000, 3000.),
								  ofRandom(-3000, 3000.),
								  ofRandom(-3000, 3000.));

		
		clusterCenters.push_back(cluster);
		generateSubClusters(cluster, ofRandom(50), 1000);
	}
}

//--------------------------------------------------------------
void testApp::generateSubClusters(ofVec3f center, int maxSubs, float maxRadius){
	
	/*
	if(clusters.getVertices().size() > 200000) return;

	clusters.addVertex(center);
	if(maxRadius < 100){
		return;
	}
	
	int subClusters = ofRandom(maxSubs/2., maxSubs);
	for(int i = 0; i < subClusters; i++){
		generateSubClusters(center + ofRandomPointOnUnitSphere() * maxRadius,
							maxSubs/2, maxRadius * .3);
	}
	 
	 */
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
	if(bang.track->getName() == "scene"){
		vector<string> parts = ofSplitString(bang.flag, " ");
		timeline.stop();
		particleRenderer.sampleTextureColors(player.getVideoPlayer()->getPixelsRef());
		player.getVideoPlayer()->stop();
		if(parts.size() == 2 &&
		   scenePaths.find(parts[0]) != scenePaths.end() &&
		   loadScene(scenePaths[parts[0]]))
		{
			
			player.getVideoPlayer()->setFrame(ofToInt(parts[1]));
			player.update();
			if(!supressPlay){
				player.getVideoPlayer()->play();
			}
			timeline.play();
		}
	}
}

//--------------------------------------------------------------
bool testApp::loadNewScene(){
    ofFileDialogResult r = ofSystemLoadDialog("Select a Scene", true);
    if(r.bSuccess){
        return loadScene(r.getPath());
    }
    return false;
}

//--------------------------------------------------------------
bool testApp::loadDefaultScene(){
    ofxXmlSettings settings;
    if(settings.loadFile("RGBDSimpleSceneDefaults.xml")){
        if(!loadScene(settings.getValue("defaultScene", ""))){
            return loadNewScene();
        }
        return true;
    }
    return loadNewScene();
}

//--------------------------------------------------------------
bool testApp::loadScene(string takeDirectory){
    if(player.setup(takeDirectory)){
        ofxXmlSettings settings;
        settings.loadFile("RGBDSimpleSceneDefaults.xml");
        settings.setValue("defaultScene", player.getScene().mediaFolder);
        settings.saveFile();
        meshBuilder.setup(player.getScene().calibrationFolder);
        
        //populate
        player.update();
        justLoadedNewScene = true;
        meshBuilder.setXYShift(player.getXYShift());
		meshBuilder.setTexture(*player.getVideoPlayer());
		meshBuilder.setDepthPixels(player.getDepthPixels());
		
        meshBuilder.update();
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void testApp::update(){
	
	if(!ofGetMousePressed(0)){
		timeline.setOffset(ofVec2f(0,ofGetHeight()-timeline.getHeight()));
	}
	
	ofRectangle fboContainer = ofRectangle(0,0,ofGetWidth()-250,timeline.getTopLeft().y);
	
	fboRect = ofRectangle(0,0,16,9);
	fboRect.scaleTo(fboContainer);
	fboRect.x = 200;
	cam.applyRotation = cam.applyTranslation = fboRect.inside(mouseX, mouseY) && !camTrack.lockCameraToTrack;
	
	if (!player.isLoaded()) {
		return;
	}
	
	if(!rendering){
		if(timeline.getIsPlaying() && !player.getVideoPlayer()->isPlaying() && !supressPlay){
			player.getVideoPlayer()->play();
		}
		else if(!timeline.getIsPlaying() && player.getVideoPlayer()->isPlaying()){
			player.getVideoPlayer()->stop();
		}
	}
	else {
		if(player.getVideoPlayer()->isPlaying()){
			player.getVideoPlayer()->stop();
		}
		if(!timeline.isSwitchOn("pause video")){
			player.getVideoPlayer()->nextFrame();
		}
	}

	bool needsUpdate = false;
    //copy any GUI changes into the mesh builder
    if(meshBuilder.shift.x != timeline.getValue("x shift") ||
	   meshBuilder.shift.y != timeline.getValue("y shift") ||
	   meshBuilder.farClip != timeline.getValue("Z Threshold") ||
	   meshBuilder.getSimplification() != simplify)
	{
        meshBuilder.setXYShift(ofVec2f(timeline.getValue("x shift"),
									   timeline.getValue("y shift")));
        meshBuilder.setSimplification(simplify);
        simplify = meshBuilder.getSimplification();
		meshBuilder.farClip = timeline.getValue("Z Threshold");

		needsUpdate = true;
    }
    
    //update the mesh if there is a new depth frame in the player
    player.update();
    needsUpdate |= player.isFrameNew();

	if(needsUpdate){
		meshBuilder.update();
		meshBuilder.updateCenter();
	}
	
//	updateParticleSystem();
//	updatePerlinLuminosity();
}

//--------------------------------------------------------------
void testApp::updateParticleSystem(){
	
	if(!drawParticles){
		return;
	}
	
	particleRenderer.fade = timeline.getValue("particle fade");
	particleRenderer.perlinForce->amplitude = timeline.getValue("perlin amp");
	particleRenderer.perlinForce->speed = timeline.getValue("perlin speed");
	particleRenderer.perlinForce->density = timeline.getValue("perlin density");
	particleRenderer.gravityForce->gravity = timeline.getValue("gravity amp");
	particleRenderer.birthRate = timeline.getValue("birth rate");
	particleRenderer.lifeSpan = timeline.getValue("life span");
	particleRenderer.lifeSpanVariance = timeline.getValue("life span var");
	particleRenderer.spinForce->center = meshBuilder.center;
	particleRenderer.spinForce->power = timeline.getValue("spin force");
	particleRenderer.spinForce->explodePower = timeline.getValue("explode force");
	particleRenderer.spinForce->explodeVar = timeline.getValue("explode var");
	particleRenderer.spinForce->zWind = timeline.getValue("z wind");
	particleRenderer.globalGeneratorOffset = ofVec3f(timeline.getValue("x offset"),0,timeline.getValue("z offset"));
	particleRenderer.maxPointSize = timeline.getValue("particle size max");
	particleRenderer.minPointSize = timeline.getValue("particle size min");
	
	particleRenderer.update();
}

//--------------------------------------------------------------
void testApp::updatePerlinLuminosity(){

	float wireframeAlpha = timeline.getValue("Wireframe Alpha");
	float luminSpeed    = timeline.getValue("Wireframe Perlin Speed");
	float luminDensity  = timeline.getValue("Wireframe Perlin Density");
	float luminContrast = timeline.getValue("Wireframe Lumin Contrast");
	float luminExponent = timeline.getValue("Wireframe Lumin Exponent");
	float luminMin = timeline.getValue("Wireframe Lumin Min");
	float luminMid = 1-timeline.getValue("Wireframe Lumin Mid");
	
	luminosityChannel += luminSpeed;
	vector<ofFloatColor>& colors = meshBuilder.getMesh().getColors();
	vector<ofVec3f>& verts = meshBuilder.getMesh().getVertices();
	float color = 0;
	int added = 0;
	for(int i = 0; i < meshBuilder.validVertIndices.size(); i++){
		
		ofVec3f& vert = verts[ meshBuilder.validVertIndices[i] ];
		float alpha = ofNoise((timeline.getValue("x offset")+vert.x)/luminDensity+luminosityChannel,
							  vert.y/luminDensity+luminosityChannel,
							  (timeline.getValue("z offset")+vert.z)/luminDensity+luminosityChannel);
		//		alpha = (alpha - luminMid) * luminContrast + luminMid;
		alpha = ofClamp((alpha - luminMid) * luminContrast + luminMid, 0, 1.0);
		//alpha = ofClamp(1 - powf(alpha, luminExponent), 0, 1.0);
		alpha = ofClamp(powf(alpha, luminExponent), luminMin, 1.0);
		alpha *= wireframeAlpha;
		if(meshBuilder.validVertIndices[i] < colors.size()){
			color += alpha;
			added++;
			colors[ meshBuilder.validVertIndices[i] ] = ofFloatColor(alpha);
		}
	}

	vector<ofVec3f>& ambientVerts = ambientParticles.getVertices();
	vector<ofFloatColor>& ambientColors =  ambientParticles.getColors();
	
	for(int i = 0; i < ambientVerts.size(); i++){
		ofVec3f& vert = ambientVerts[ i ];
		
		float alpha = ofNoise(vert.x/luminDensity+luminosityChannel,
							  -1.*vert.y/luminDensity+luminosityChannel,
							  vert.z/luminDensity+luminosityChannel);
		//		alpha = (alpha - luminMid) * luminContrast + luminMid;
		//alpha = ofClamp((alpha - luminMid) * luminContrast + luminMid, 0, 1.0);
		//alpha = ofClamp(1 - powf(alpha, luminExponent), 0, 1.0);
		alpha = ofClamp(powf(alpha, luminExponent), luminMin, 1.0);
		
		alpha *= particleFade;
		ambientColors[i] = ofFloatColor(alpha);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	updateParticleSystem();
	updatePerlinLuminosity();

	renderTarget.begin();
	ofClear(0,0,0,255);
	cam.begin(ofRectangle(0,0,renderTarget.getWidth(), renderTarget.getHeight()));
			
    if(player.isLoaded()){
		drawGeometry();
	}

	ofPushMatrix();
//	ofScale(1,-1,1);

	ambientShader.begin();

	ambientShader.setUniform1f("maxDisance", particleMaxDistance) ;
	ambientShader.setUniform1f("maxSize", particeMaxSize) ;

	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofPushStyle();
	glPointSize(3.0);
	ofSetColor(particleFade*255);
//	ambientParticles.setMode(OF_PRIMITIVE_LINES);
//	ambientParticles.draw();
	ambientParticles.drawVertices();
	ofPopStyle();
	ambientShader.end();

	ofPopMatrix();
	
//	randomUnits.drawVertices();
//	clusters.drawVertices();

	cam.end();
	
	renderTarget.end();
	
	ofEnableAlphaBlending();
	ofPushStyle();
	ofSetColor(0);
	ofRect(fboRect);
	ofPopStyle();

	
	renderTarget.getTextureReference().draw(fboRect.x,fboRect.getBottom(),fboRect.width,-fboRect.height);
	
	if(rendering){

		saveImage.setUseTexture(false);
		renderTarget.readToPixels(saveImage.getPixelsRef());
		saveImage.mirror(true, false);
		char filename[1024];
		sprintf(filename, "%sframe_%05d.png",renderFolder.c_str(), currentFrameNumber);
		saveImage.saveImage(filename);
		currentFrameNumber++;
		
	}
	timeline.draw();
    gui.draw();
}

//--------------------------------------------------------------
void testApp::drawGeometry(){
	
    float pointAlpha = timeline.getValue("Point Alpha");
	float pointSize = timeline.getValue("Point Size");
    float wireAlpha = timeline.getValue("Wireframe Alpha");
	float thickness = timeline.getValue("Wireframe Thickness");
	
	float particleAlpha = timeline.getValue("particle fade");
//	float particleSize = timeline.getValue("particle size");

	
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	
	player.getVideoPlayer()->getTextureReference().bind();
	
	ofPushMatrix();
	ofPushStyle();
	ofScale(1,-1,1);
	
	ofPushMatrix();
	ofTranslate(timeline.getValue("x offset"), 0, timeline.getValue("z offset"));
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);

	if(drawWireframe && wireAlpha > 0){

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_CULL_FACE);

		glCullFace(GL_FRONT);

		ofTranslate(0,0,-.5);
		ofSetColor(255*wireAlpha);
		thickness *= thickness;
		ofSetLineWidth(thickness);
		meshBuilder.getMesh().drawWireframe();
		
		glPopAttrib();
	}
	
	if(drawPointcloud && pointAlpha > 0){

		ofTranslate(0,0,-.5);
		ofSetColor(255*pointAlpha);
		meshBuilder.getMesh().disableColors();
		
		meshBuilder.getMesh().setMode(OF_PRIMITIVE_POINTS);
		glPointSize(pointSize*pointSize);
		meshBuilder.getMesh().draw();
		meshBuilder.getMesh().setMode(OF_PRIMITIVE_TRIANGLES);
	
		meshBuilder.getMesh().enableColors();
	}
	
	ofPopMatrix();
	
	if(drawParticles){
		
		cloudShader.begin();
		particleRenderer.mesh.drawVertices();
		cloudShader.end();
	}
	
	
	player.getVideoPlayer()->getTextureReference().unbind();
	
	
	if(justLoadedNewScene){
		cout << "RENDERED FIRST FRAME OF " << player.getVideoPlayer()->getCurrentFrame() << endl;
		justLoadedNewScene = false;
	}
	
	ofPopStyle();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::loadShader(){

	cloudShader.load("shaders/Cloud");
	cloudShader.begin();
	cloudShader.setUniform1i("tex0", 0 );
	cloudShader.end();
	
	ambientShader.load("shaders/Ambient");
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(timeline.isModal()){
		return;
	}
	
    if(key == ' '){
		timeline.togglePlay();
	}
	
	if(key == 'T'){
		camTrack.addKeyframe();
	}
	
	if(key == 'L'){
		camTrack.lockCameraToTrack = !camTrack.lockCameraToTrack;
		if(!camTrack.lockCameraToTrack){
			cam.setAnglesFromOrientation();
		}
	}
	
	if(key == 'R'){
		rendering = !rendering;
		
		if(rendering){
			char fileName[1024];
			sprintf(fileName, "frame_%02d_%02d_%02d/", ofGetDay(), ofGetHours(), ofGetMinutes() );
			renderFolder = string(fileName);
			ofDirectory(renderFolder).create(true);
		}

		timeline.stop();
		player.getVideoPlayer()->stop();
		timeline.setPercentComplete(timeline.getInOutRange().min);
		timeline.play();
	}
	
	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(key == 'S'){
		loadShader();
	}
	
	if(key == '1'){
		particleRenderer.sampleTextureColors(player.getVideoPlayer()->getPixelsRef());
	}
	
	if(key == 'P'){
		generateAmbientParticles();
	}
}

//--------------------------------------------------------------
void testApp::exit(){
    gui.saveToFile("defaultSettings.xml");
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
	ofDirectory dir(dragInfo.files[0]);
	if( dir.isDirectory() && ofxRGBDScene::isFolderValid(dragInfo.files[0]) ){
		loadScene(dragInfo.files[0]);
	}
}
