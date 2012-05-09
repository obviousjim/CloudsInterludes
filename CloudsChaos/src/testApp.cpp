#include "testApp.h"

//Wireframe/Point DOF
//Mesh Distortion
//Mesh Luminosity
//Wind force
//Color map
//Typographic effect

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
    ofSetFrameRate(60);
    ofBackground(30);
    
    ofToggleFullscreen();
    
    renderTarget.allocate(1920, 1080, GL_RGB, 4);
    focusChannel.allocate(1920, 1080, GL_RGB, 4);
    modelTarget.allocate(1920, 1080, GL_RGB, 4);
    blurBuffer.allocate(1920, 1080, GL_RGB, 4);
    
    renderer.setSimplification(2);
    renderer.forceUndistortOff = true;
    renderer.addColors = true;
    
    cam.setup();
    cam.usemouse = true;
    cam.autosavePosition = true;
    cam.speed = cameraSpeed = 20;
    cam.setScale(1, -1, 1);
    cam.targetNode.setScale(1, -1, 1);
    cam.loadCameraPosition();
	cam.maximumY =  120;
	cam.minimumY = -120;
	cam.rollSpeed = 1;
    cam.loadCameraPosition();
    
    currentSaveFolder = "EXPORT_" + ofToString(ofGetMonth()) + "_" + ofToString(ofGetDay()) + "_" + ofToString(ofGetHours()) + ofToString(ofGetMinutes());
    savingImage.allocate(1920, 1080, OF_IMAGE_COLOR);
	createdFolder = false;
    
    track.setCamera(cam);
    track.setXMLFileName("cameraMoves.xml");
    timeline.setup();
    timeline.setMovePlayheadOnDrag(false);
    timeline.setDurationInFrames(500);
    timeline.setOffset(ofVec2f(0,0));
        
    int absoluteMaxParticles = 500000;
    depthImages.setup();
    timeline.setPageName("Source");
    timeline.addElement("Depth", &depthImages);
    timeline.addElement("Track", &track);
    
    timeline.addPage("Rendering");
    timeline.addKeyframes("Simplify", "zthreshold.xml", ofRange(0, 8 ), 2);
    timeline.addKeyframes("Z Threshold", "zthreshold.xml", ofRange(0, sqrtf(5000) ), sqrtf(5000));
    timeline.addKeyframes("Edge Snip", "edgesnip.xml", ofRange(0, sqrtf(2000) ), sqrtf(2000));

    timeline.addPage("Particles");
    timeline.addKeyframes("Max Particles", "maxParticles.xml", ofRange(0, absoluteMaxParticles) );
    timeline.addKeyframes("Birthrate", "particleBirthrate.xml", ofRange(.000, .05) );
    timeline.addKeyframes("Lifespan", "particleLifespan.xml", ofRange(2, 900) );
    timeline.addKeyframes("Lifespan Variance", "particleLifespanVariance.xml", ofRange(0, 100) );
    timeline.addKeyframes("Drag Force", "particleDragFroce.xml", ofRange(0, 1.0), 0);
    
    timeline.addPage("DOF");
    timeline.addKeyframes("DOF Distance", "DOFDistance.xml", ofRange(0,sqrtf(1000.0)), 10 );
    timeline.addKeyframes("DOF Range", "DOFRange.xml", ofRange(10,sqrtf(500.0)) );
    timeline.addKeyframes("DOF Blur", "DOFBlur.xml", ofRange(0,5.0) );
    timeline.addKeyframes("Min Point Size", "dofMinPoint.xml", ofRange(1, 5));
    timeline.addKeyframes("Max Point Size", "dofMaxPoint.xml", ofRange(10, 45));
    
    timeline.addPage("Perlin");
    timeline.addKeyframes("Perlin Amplitude", "perlinAmplitude.xml", ofRange(0, sqrtf(20)) );
    timeline.addKeyframes("Perlin Density", "perlinDensity.xml", ofRange(0, sqrtf(2000)));
    timeline.addKeyframes("Perlin Speed", "perlinSpeed.xml", ofRange(0, sqrtf(2)), 0);

    timeline.addPage("Mesh Rendering");
	timeline.addKeyframes("Wireframe Alpha", "wireframeAlpha.xml", ofRange(0, 1.0), 0.0);
    timeline.addKeyframes("Wireframe Perlin Density", "wireframePerlinDensity.xml", ofRange(500, 3000.0), 0.0);
    timeline.addKeyframes("Wireframe Perlin Speed", "wireframePerlinSpeed.xml", ofRange(0, .2), 0.0);
    timeline.addKeyframes("Wireframe Lumin Contrast", "wireframeLuminContrast.xml", ofRange(0, 10.0), 1.0);
    timeline.addKeyframes("Wireframe Lumin Exponent", "wireframeLuminExponent.xml", ofRange(1.0, 5.0), 0.0);
    
    timeline.addPage("Mesh Distort");
    timeline.addKeyframes("Mesh Implode", "meshImplode.xml", ofRange(1.0, 2500), 1.0 );
    timeline.addKeyframes("Mesh Perlin Amplitude", "meshPerlinAmplitude.xml", ofRange(0, sqrtf(20)) );
    timeline.addKeyframes("Mesh Perlin Density", "meshPerlinDensity.xml", ofRange(0, sqrtf(2000)));
    timeline.addKeyframes("Mesh Perlin Speed", "meshPerlinSpeed.xml", ofRange(0, sqrtf(2)), 0);
    
    timeline.addPage("Typography");
    timeline.addKeyframes("Type Alpha", "typeAlpha.xml", ofRange(0,1));
	timeline.addKeyframes("Chance of Attaching Type", "typeChance.xml", ofRange(0,0.5) );
	timeline.addKeyframes("Line Alpha", "lineAlpha.xml", ofRange(0,1.0) );

    luminosityChannel = 0; //for mesh perlin
        
    panel.setup("Settings", "settings.xml");
    panel.add(setCompDirectory.setup("load comp"));
    panel.add(renderOutput.setup("render", false));
    panel.add(lockToTrackToggle.setup("lock to track",false));
    panel.add(resetCamera.setup("reset cam", false));
    panel.add(useShaderToggle.setup("use shader",true));
    panel.add(reloadShaders.setup("reload shader", false));
    panel.add(showType.setup("show type", false));
    panel.add(clear.setup("clear", false));
    panel.add(cameraSpeedSlider.setup("camera speed", cameraSpeed, .5, 40));
    panel.add(freezeParticles.setup("freeze particles", false));
    panel.add(useColors.setup("use palette", false));
    panel.add(loadPalette.setup("load palette", false));
	panel.add(drawPointcloud.setup("draw pointcloud", false));
    
    renderOutput = false; // force false
    framesSaved = 0;

    ofxXmlSettings defaults;
    defaults.loadFile("defaults.xml");
    string defaultTake = defaults.getValue("take", "");
    string defaultComp = defaults.getValue("comp", "");
    string defaultPalette = defaults.getValue("palette", "");
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
//		  timeline.addElement("Video", &playerElement);
//        timeline.setDurationInFrames(movie.getDuration());
    }
    
    if(defaultComp != ""){
        cout << "loading default comp" << defaultComp << endl;
        timeline.loadElementsFromFolder(defaultComp);
        currentSaveFolder = defaultComp + "EXPORTS";
    }
    
    if(defaultPalette != ""){
        colorPalette.loadImage(defaultPalette);
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
        mesh.addTexCoord(ofVec2f(0.0,0.0));
    }

    loadShaders();
}

//--------------------------------------------------------------
void testApp::update(){
    
    if(!take.valid()){
        return;
    }
    
    if(reloadShaders){
        loadShaders();

    }
    if(saveCameraPoint){
    	track.sample();
    }
    
    if(clear){
        for(int i = 0; i < emmiters.size(); i++){
        	emmiters[i].particles.clear();
        }
    }
    
    if(setCompDirectory){
        ofFileDialogResult r = ofSystemLoadDialog("Select Comp", true);
        if(r.bSuccess){

            timeline.loadElementsFromFolder(r.getPath() + "/");
            currentSaveFolder = r.getPath() + "/EXPORTS";
            ofDirectory(currentSaveFolder).create(true);
            
            ofxXmlSettings defaults;
            defaults.loadFile("defaults.xml");
            defaults.setValue("comp", r.getPath() + "/");
			defaults.saveFile();
        }
    }
    
    if(loadPalette){
        ofFileDialogResult r = ofSystemLoadDialog("Load Palette", false);
        if(r.bSuccess){
			colorPalette.loadImage(r.getPath());
            
            ofxXmlSettings defaults;
            defaults.loadFile("defaults.xml");
            defaults.setValue("palette", r.getPath());
			defaults.saveFile();
        }
    }
    
    track.lockCameraToTrack = lockToTrackToggle;
    if(lockToTrackToggle){
        cam.targetNode.setPosition(cam.getPosition());
        cam.targetNode.setOrientation(cam.getOrientationQuat());
        cam.rotationX = cam.targetXRot = -cam.getHeading();
        cam.rotationY = cam.targetYRot = -cam.getPitch();
    }
    if(resetCamera){
        cam.reset();
    }
     
    cam.speed = cameraSpeed;
    
    //VIEW
    float fboHeight = ofGetHeight() - timeline.getDrawRect().height - 28;
    float fboWidth = 1920./1080 * fboHeight;
    fboRect = ofRectangle(timeline.getDrawRect().x, timeline.getDrawRect().height + 28, fboWidth, fboHeight);
    cam.applyRotation = cam.applyTranslation = fboRect.inside(ofGetMouseX(),ofGetMouseY()) && !lockToTrackToggle;
    
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
        
        renderer.calculateTextureCoordinates = useColors && colorPalette.isAllocated();
        renderer.update();

        //modify the colors
        float wireframeAlpha = timeline.getKeyframeValue("Wireframe Alpha");
        float luminSpeed    = timeline.getKeyframeValue("Wireframe Perlin Speed");
        float luminDensity  = timeline.getKeyframeValue("Wireframe Perlin Density");
        float luminContrast = timeline.getKeyframeValue("Wireframe Lumin Contrast");
        float luminExponent = timeline.getKeyframeValue("Wireframe Lumin Exponent");
                
        luminosityChannel += luminSpeed;
        
        vector<ofFloatColor>& colors = renderer.getMesh().getColors();
        vector<ofVec3f>& verts = renderer.getMesh().getVertices();
        for(int i = 0; i < renderer.getTotalPoints(); i++){
            if(renderer.isVertexValid(i)){
                int vertexIndex = renderer.vertexIndex(i);
                
                ofVec3f& vert = verts[vertexIndex];
                float alpha = ofNoise(vert.x/luminDensity+luminosityChannel,
                                 vert.y/luminDensity+luminosityChannel, 
                                 vert.z/luminDensity+luminosityChannel);
            	alpha = (alpha - .05f) * luminContrast + 0.5f;
                alpha = 1 - powf(alpha, luminExponent);
                alpha *= wireframeAlpha;
                colors[vertexIndex] = ofFloatColor(1.0,1.0,1.0,alpha);
            }
        }
        
        /*
        //now distort the mesh
        float implode = timeline.getKeyframeValue("Mesh Implode");
        float implodeSqr = implode*implode;
        ofVec3f zero(0,0,0);
        for(int i = 0; i < renderer.getTotalPoints(); i++){
            if(renderer.isVertexValid(i)){
                int vertexIndex = renderer.vertexIndex(i);                
                
                //float alpha = ofMap(verts[vertexIndex].length(), implode, implode-2000, 1.0, 0.0, true);
                //float effect = implode - verts[vertexIndex].length();
                
                if(alpha == 0){
                    //verts[vertexIndex] *= 1.0/(effect+1.);
                    verts[vertexIndex] = zero;
                }
                else if(alpha != 1.0){
                    verts[vertexIndex].getInterpolated(zero, alpha);
                }

            }
        }
		*/
    }
    
    
    dragForce->dragForce = powf( timeline.getKeyframeValue("Drag Force"), 2.0);
    perlinForce->amplitude = powf( timeline.getKeyframeValue("Perlin Amplitude"), 2.0);
    perlinForce->density = powf( timeline.getKeyframeValue("Perlin Density"), 2.0);
    perlinForce->speed = powf( timeline.getKeyframeValue("Perlin Speed"), 2.0);
    
//    meshForce->minRadius = timeline.getKeyframeValue("Min Radius");
//    meshForce->attractScale = timeline.getKeyframeValue("Mesh Attract");
	
    dragForce->update();
    perlinForce->update();
//    meshForce->update();
    
    
    //GENERATOR
    float birthRate = timeline.getKeyframeValue("Birthrate");
    float lifeSpan  = timeline.getKeyframeValue("Lifespan");
    float lifeSpanVariance = timeline.getKeyframeValue("Lifespan Variance");
    int maxParticles = timeline.getKeyframeValue("Max Particles");
    float typeChance = timeline.getKeyframeValue("Chance of Attaching Type");
    totalParticles = 0;
    for(int i = 0; i < emmiters.size(); i++){
    	emmiters[i].birthRate = 0;
        emmiters[i].freeze = freezeParticles;
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
            g.texcoord = renderer.getMesh().getTexCoord( renderer.vertexIndex(i) );
            g.remainingParticles = maxParticles - totalParticles;
            g.showType = showType;
            g.typeChance = typeChance;
        }
    }
    
    for(int i = 0; i < emmiters.size(); i++){
    	emmiters[i].update();
    }

    
    //put the particles in the mesh;
    copyVertsToMesh();        
}

void testApp::loadShaders(){
	pointCloudDOF.load("shaders/DOFCloud");
    pointCloudDOF.begin();
    pointCloudDOF.setUniform1i("tex", 0);
    pointCloudDOF.end();
    dofRange.load("shaders/dofrange");
    dofBlur.load("shaders/dofblur");
    dofBlur.begin();
    dofBlur.setUniform1i("tex", 0);
    dofBlur.setUniform1i("range", 1);
    dofBlur.end();
}

//--------------------------------------------------------------
void testApp::draw(){

    float focalRange = powf(timeline.getKeyframeValue("DOF Range"), 2);
    float focalDistance = powf(timeline.getKeyframeValue("DOF Distance"), 2);
    
    //////////////////////////
    //START RENDER FOCUS CHANNEL
    focusChannel.begin();    
    ofClear(255,255,255,255);
    
    cam.begin(ofRectangle(0,0,1920,1080));
    glEnable(GL_DEPTH_TEST);
    
    dofRange.begin();
    dofRange.setUniform1f("focalDistance", focalDistance);
    dofRange.setUniform1f("focalRange", focalRange);
    
    ofDisableAlphaBlending();
    //renderer.drawMesh(false);
    ofPushStyle();
    ofSetLineWidth(4);
    renderer.drawWireFrame(false);
    ofPopStyle();
    dofRange.end();
    
    cam.end();
    
    glDisable(GL_DEPTH_TEST);
    focusChannel.end();
    
    //STOP RENDER FOCUS CHANNEL
	//////////////////////////
    
    //////////////////////////
    //START RENDER WIREFRAME
    modelTarget.begin();
    ofClear(0,0,0,0);
    
    cam.begin(ofRectangle(0,0,1920,1080));
    ofPushStyle();
    
    ofEnableAlphaBlending();
    glEnable(GL_DEPTH_TEST);
    
//    if(selfOcclude){
        //occlude points behind the mesh
//        ofPushMatrix();
//        ofSetColor(0, 0, 0, 0);
//ofVec3f camTranslateVec = cam.getLookAtDir();    
//        ofTranslate(camTranslateVec);
//        renderer.drawMesh(false);
//        ofPopMatrix();
//    }
    
    ofEnableAlphaBlending();
    //ofEnableBlendMode(blendMode);
    glEnable(GL_DEPTH_TEST);
    
//    ofSetColor(255*wireframeAlpha);        
    //float thickness = timeline.getKeyframeValue("Wireframe Thickness");
    //thickness *= thickness;
    //ofSetLineWidth(thickness);
    ///add colors
	if(!drawPointcloud){
		renderer.drawWireFrame(false);    
    }
    else{
        glPointSize(timeline.getKeyframeValue("Min Point Size"));
        renderer.drawPointCloud(false);    
    }
    
    ofPopStyle();
    
    glDisable(GL_DEPTH_TEST);
    cam.end();
    
    modelTarget.end();
    //STOP RENDER MODEL TARGET
	//////////////////////////
    
    //////////////////////////
    //START RENDER POINTS
    renderTarget.begin();
    ofClear(0);
    
    cam.begin(ofRectangle(0,0,1920,1080));
    
    ofEnableAlphaBlending();
    //RENDERER
    ofPushStyle();
    glPushMatrix();
    glScalef(1,-1,1);
    if(useShaderToggle){
        pointCloudDOF.begin();
        pointCloudDOF.setUniform1f("minSize", timeline.getKeyframeValue("Min Point Size"));
        pointCloudDOF.setUniform1f("maxSize", timeline.getKeyframeValue("Max Point Size"));
        pointCloudDOF.setUniform1f("focalRange", focalRange);
        pointCloudDOF.setUniform1f("focalDistance", focalDistance);
        pointCloudDOF.setUniform1i("useTexture", useColors && colorPalette.isAllocated() ? 1 : 0);
    }
    else{
    	glPointSize(timeline.getKeyframeValue("Min Point Size"));
    }
    
    if(useColors && colorPalette.isAllocated()){
    	colorPalette.bind();
    }
//    if(drawPointcloud){
//        ofPushMatrix();
//        ofScale(1,-1,1);
//        ofEnableAlphaBlending();
//        renderer.drawPointCloud(false);    
//        ofPopMatrix();
//	}

    ofEnableBlendMode(OF_BLENDMODE_ADD);
    glEnable(GL_POINT_SMOOTH); // makes circular points
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
    
    mesh.drawVertices();
    
    if(useColors && colorPalette.isAllocated()){
		colorPalette.unbind();    	
    }

    if(useShaderToggle){
        pointCloudDOF.end();    	
    }

    if(showType){
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofPushStyle();
        float baseLineAlpha = timeline.getKeyframeValue("Line Alpha");
        //ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
        for(int i = 0; i < emmiters.size(); i++){
            for(int v = 0; v < emmiters[i].particles.size(); v++){
                if(emmiters[i].particles[v].hasType){
                    float fade = emmiters[i].particles[v].energyPercent;
                    if(fade > 0){
                        ofSetColor(fade*255,fade*255,fade*255);
 //                       ofTranslate(emmiters[i].particles[v].position);
//                        ofScale(10, -10, 10);
                        ofDrawBitmapString(ofToString(int(emmiters[i].particles[v].energy)), emmiters[i].particles[v].position);
//                        ofDrawBitmapString(ofToString(int(emmiters[i].particles[v].energy)), ofVec3f(0,0,0));
   //                     ofPopMatrix();
                        float lineAlpha = baseLineAlpha*fade;
                        if(lineAlpha > 0){
                            ofSetLineWidth(1);
                            ofSetColor(lineAlpha*255, lineAlpha*255, lineAlpha*255);
                            ofLine(emmiters[i].particles[v].position,emmiters[i].particles[v].origin);
                    	}
                	}
                }
            }
        }
		ofPopStyle();
    }
    glPopMatrix();
    ofPopStyle();

    cam.end();
    renderTarget.end();
	//STOP RENDER POINTS
	//////////////////////////

    //////////////////////////
	// COMP THE WIREFRAME INTO THE POINTS
    //////////////////////////
	blurBuffer.begin();
    ofClear(0);
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    ofSetColor(255);
    dofBlur.begin();
    
    //mulit-text
    //our shader uses two textures, the top layer and the alpha
    //we can load two textures into a shader using the multi texture coordinate extensions
    glActiveTexture(GL_TEXTURE0_ARB);
    modelTarget.getTextureReference().bind();
    
    glActiveTexture(GL_TEXTURE1_ARB);
    focusChannel.getTextureReference().bind();
    float blurAmount = timeline.getKeyframeValue("DOF Blur");
    dofBlur.setUniform2f("sampleOffset", blurAmount, 0);
    //draw a quad the size of the frame
    glBegin(GL_QUADS);
    
    //move the mask around with the mouse by modifying the texture coordinates
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 0);		
    glVertex2f(0, 0);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 1920, 0);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 1920, 0);		
    glVertex2f(1920, 0);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 1920, 1080);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 1920, 1080);
    glVertex2f(1920, 1080);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 1080);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 1080);		
    glVertex2f(0, 1080);
    
    glEnd();
    
    
    //deactive and clean up
    glActiveTexture(GL_TEXTURE1_ARB);
    focusChannel.getTextureReference().unbind();
    
    glActiveTexture(GL_TEXTURE0_ARB);
    modelTarget.getTextureReference().unbind();
    
    dofBlur.end();
    
    ofPopStyle();
    
    blurBuffer.end();     
    
    modelTarget.begin();
    ofClear(0.0,0.0,0.0,0.0);
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    ofSetColor(255, 255, 255, 255);
    dofBlur.begin();
    
    //mulit-text
    //our shader uses two textures, the top layer and the alpha
    //we can load two textures into a shader using the multi texture coordinate extensions
    //canabalize the 
    glActiveTexture(GL_TEXTURE0_ARB);
    blurBuffer.getTextureReference().bind();
    
    glActiveTexture(GL_TEXTURE1_ARB);
    focusChannel.getTextureReference().bind();
    
    dofBlur.setUniform2f("sampleOffset", 0, blurAmount);
    glBegin(GL_QUADS);
    
    //move the mask around with the mouse by modifying the texture coordinates
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 0);
    glVertex2f(0, 0);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 1920, 0);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 1920, 0);		
    glVertex2f(1920, 0);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 1920, 1080);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 1920, 1080);
    glVertex2f(1920, 1080);
    
    glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 1080);
    glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 1080);		
    glVertex2f(0, 1080);
    
    glEnd();
    
    //deactive and clean up
    glActiveTexture(GL_TEXTURE1_ARB);
    focusChannel.getTextureReference().unbind();
    
    glActiveTexture(GL_TEXTURE0_ARB);
    blurBuffer.getTextureReference().unbind();
    
    dofBlur.end();
    
    //COMP IN THE POINTS
    renderTarget.draw(0, 0);
    
    ofPopStyle();
    
    modelTarget.end();

    //NOW MODEL TARGET HAS THE DOF"d Wireframe + POINTS
    
    //COMP
    
    ofPushStyle();
    ofSetColor(0);
    ofRect(fboRect);
    ofPopStyle();

    modelTarget.getTextureReference().draw(fboRect); 
    if(renderOutput){
        modelTarget.getTextureReference().readToPixels(savingImage.getPixelsRef());
        char filename[512];        		
        sprintf(filename, "%s/save_frame_%05d.png", currentSaveFolder.c_str(), timeline.getCurrentFrame());
        savingImage.saveImage(filename);
		framesSaved++;
    }
    
    ofDrawBitmapString("totalParticles " + ofToString(totalParticles ), fboRect.x+fboRect.width + 10, fboRect.y + 15);
    ofDrawBitmapString("Saving to " + currentSaveFolder, 10, fboRect.y - 15);
    timeline.draw();
    panel.setPosition(ofPoint(fboRect.x+fboRect.width + 10, fboRect.y + 32));
    panel.draw();
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
    vector<ofVec2f>& meshTexCoords = mesh.getTexCoords();
    for(int i = 0; i < emmiters.size(); i++){
        for(int v = 0; v < emmiters[i].particles.size(); v++){
            meshVertices[meshIndex] = emmiters[i].particles[v].position;
            float color = emmiters[i].particles[v].energy / emmiters[i].particles[v].initialEnergy;
            meshColors[meshIndex] = ofFloatColor(color,color,color,color);
            if(useColors){
                meshTexCoords[meshIndex] = emmiters[i].particles[v].texcoord;
            }
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

    }
    
    if(key == 'C'){
    }
    
    if(key == 'L'){
    	//loadShaders();
        //lockToTrackToggle != lockToTrackToggle;
        lockToTrackToggle = !lockToTrackToggle;
    }
    
    if(key == 'T'){
        track.sample();
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