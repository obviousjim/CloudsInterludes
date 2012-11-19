#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofBackground(0);
	ofSetVerticalSync(true);
	
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	ofToggleFullscreen();
	rendering = false;
	
	cam.setup();
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	gui.setup("gui");
	
	gui.add(seed.setup("seed", ofxParameter<int>(), 0, 100));
	gui.add(heroNodes.setup("hero nodes", ofxParameter<int>(), 5, 20));
	gui.add(heroRadius.setup("spawn radius", ofxParameter<float>(), 5, 1000));
	gui.add(heroRadiusVariance.setup("radius var", ofxParameter<float>(), 5, 1000));
	gui.add(numIterations.setup("num iterations", ofxParameter<int>(), 1, 20));
	gui.add(numBranches.setup("num branches", ofxParameter<int>(), 1, 20));
	gui.add(numSurvivingBranches.setup("surviving branches", ofxParameter<int>(), 1, 10));
	gui.add(minDistance.setup("min branch dist", ofxParameter<float>(), 10, 1000));
	gui.add(distanceRange.setup("branch dist rng", ofxParameter<float>(), 0, 3.0));
	gui.add(stepSize.setup("step size", ofxParameter<float>(), 1, 300));
	gui.add(numPointsAtReplicate.setup("points at replicate",ofxParameter<int>(), 10, 1000));
	gui.add(replicatePointDistance.setup("replicate distance",ofxParameter<float>(), 5, 500));
	gui.add(pointSize.setup("replicate point size",ofxParameter<float>(), 1, 5));
	gui.add(lineThickness.setup("line thickness", ofxParameter<float>(), 2, 10));

	gui.add(minAttractRadius.setup("min attract radius", ofxParameter<float>(), 10, 1000));
	gui.add(minRepelRadius.setup("min repel radius", ofxParameter<float>(), 0, 1000));
	gui.add(minFuseRadius.setup("min fuse radius", ofxParameter<float>(), 1, 100));
	
	gui.add(maxAttractForce.setup("max attract force", ofxParameter<float>(), 0, 1.0));
	gui.add(maxRepelForce.setup("max repel force", ofxParameter<float>(), 0, 1.0));

//	gui.add(maxTraverseDistance.setup("max traverse dist", ofxParameter<float>(), 100, 1000));
	gui.add(maxTraverseAngle.setup("max traverse angle", ofxParameter<float>(), 0, 180));
//	gui.add(traverseStepSize.setup("traverse step", ofxParameter<float>(), .5, 5));
	
	gui.add(lineStartTime.setup("line start", ofxParameter<float>(), 0, 1.0));
	gui.add(lineEndTime.setup("line end", ofxParameter<float>(), 0, 1.0));
	gui.add(lineFadeVerts.setup("line fade verts", ofxParameter<int>(), 1, 10));

	gui.loadFromFile("settings.xml");
	
	timeline.setup();

	timeline.setFrameRate(24);
	timeline.setFrameBased(true);
	timeline.setDurationInFrames(25*24);
	timeline.addTrack("camera", &camTrack);
	timeline.addColors("line color");
	timeline.addColors("node color");
	camTrack.setCamera(cam);

	renderTarget.allocate(1920, 1080, GL_RGB, 4);

	traversedNodePoints.setUsage( GL_DYNAMIC_DRAW );
	traversedNodePoints.setMode(OF_PRIMITIVE_POINTS);

	loadShader();
}


//--------------------------------------------------------------
void testApp::loadShader(){
	billboard.load("shaders/Billboard");
	billboard.begin();
	billboard.setUniform1i("tex", 0);
	billboard.end();
	
	ofDisableArbTex();
	nodeSprite.loadImage("shaders/dot.png");
	ofEnableArbTex();
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
	
//	gui.add(lineStartTime.setup("line start", ofxParameter<float>(), 0, 1.0));
//	gui.add(lineEndTime.setup("line end", ofxParameter<float>(), 0, 1.0));
//	gui.add(lineFadeVerts.setup("line fade verts", ofxParameter<int>(), 1, 10));

	int vertsToHighlight = ofMap(timeline.getPercentComplete(), lineStartTime, lineEndTime, 0, traversal.getVertices().size());
	for(int i = 0; i < vertsToHighlight; i++){;
//		float fade = ofMap(i, vertsToHighlight*.9, vertsToHighlight, 1.0, 0, true);
		if(traversalIndexToNodeIndex.find(i) != traversalIndexToNodeIndex.end()){
			//traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i ] ].x = 1.0;
//			cout << "setting color of  line point " << i << " to node index " << endl;
			traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i ] ].x = 1.0;
			traversedNodePoints.getColors()[ traversalIndexToNodeIndex[i ] ].r = 0.0;
		}
		ofFloatColor lineColor = timeline.getColor("line color");
		traversal.setColor(i, lineColor);
	}
	
	for(int i = vertsToHighlight; i < traversal.getVertices().size(); i++){
		traversal.setColor(i, ofFloatColor(0));
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	renderTarget.begin();
	ofClear(0,0,0,0);
	
	cam.begin(ofRectangle(0,0,1920,1080));
	ofPushStyle();

	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofSetColor(255);
	
	ofSetLineWidth(	lineThickness );
	geometry.setMode(OF_PRIMITIVE_LINE_STRIP);
	geometry.draw();
	
	glPointSize(pointSize);
	ofSetColor(255*.15);
	points.drawVertices();
//	ofSetColor(255, 0, 0);
	traversal.setMode(OF_PRIMITIVE_LINE_STRIP);
	traversal.draw();

	billboard.begin();
	ofEnablePointSprites();
	ofDisableArbTex();
	nodeSprite.getTextureReference().bind();
	traversedNodePoints.drawVertices();
	nodeSprite.getTextureReference().unbind();
	billboard.end();
	ofDisablePointSprites();
	ofEnableArbTex();
	ofPopStyle();
	
	cam.end();
	renderTarget.end();
	
	renderTarget.draw(fboRect.getX(), fboRect.getMaxY(), fboRect.width,-fboRect.height);
	
	if(rendering && timeline.getIsPlaying()){
		char fileName[1024];
		sprintf(fileName, "%sframe_%05d.png", renderFolder.c_str(), frameNum++);
		ofImage image;
		image.setUseTexture(false);
		renderTarget.readToPixels(image.getPixelsRef());
		image.mirror(true, false);
		image.saveImage(string(fileName));
	}
	
	timeline.draw();
	gui.draw();
}

//--------------------------------------------------------------
void testApp::generate(){
	for(int i = 0; i < nodes.size(); i++){
		delete nodes[i];
	}
	
	nodes.clear();
	geometry.clear();
	points.clear();
	fusePoints.clear();
	srand(seed);
	
	for(int i = 0; i < heroNodes; i++){
		Node* n = new Node();
		n->position = ofRandomPointOnUnitSphere() * heroRadius + ofRandom(heroRadiusVariance);
		n->sourceId = i;
		n->minDistance = minDistance;
		n->distanceRange = distanceRange;
		n->numReplications = numBranches;
		n->mesh = &geometry;
		n->stepSize = stepSize;
		n->numSurvivingBranches = numSurvivingBranches;
		n->minFuseRadius = minFuseRadius;
		n->lineColor = timeline.getColor("line color");
		n->nodeColor = timeline.getColor("node color");
		n->replicatePointDistance = replicatePointDistance;
		n->numPointsAtReplicate = numPointsAtReplicate;
		
		n->minAttractRadius = minAttractRadius;
		n->minRepelRadius = minRepelRadius;
		n->minFuseRadius = minFuseRadius;
		
		n->maxAttractForce = maxAttractForce;
		n->maxRepelForce = maxRepelForce;
		
		nodes.push_back(n);
	}
	
	for(int i = 0; i < numIterations; i++){
		
		if(geometry.getVertices().size() > 200000){
			break;
		}
		
		int numNodes = nodes.size();
		for(int n = 0; n < numNodes; n++){
			if(nodes[n]->leaf){
				nodes[n]->replicate( nodes, points.getVertices() );
			}
		}
	}

	int numNodes = nodes.size();
	for(int n = 0; n < numNodes; n++){
		if(nodes[n]->leaf){
			nodes[n]->finalize();
		}
	}
	
}

//--------------------------------------------------------------
void testApp::traverse(){
	
	//find the nodes with the greatest distance and to get there
	Node* n1, *n2 = NULL;
	float maxDistance = 0;
	for(int i = 0; i < nodes.size(); i++){
		for(int j = 0; j < nodes.size(); j++){
			float dist = nodes[i]->position.distanceSquared(nodes[j]->position);
			if( i!=j && dist > maxDistance ){
				maxDistance = dist;
				n1 = nodes[j];
				n2 = nodes[i];
			}
		}
	}
	
	if(n1 == NULL || n2 == NULL) return;
	
	traversal.clear();

	
	int numTries = 0;
	ofVec3f position = n1->position;
	ofVec3f direction = (n2->position - n1->position).normalized();
	cout << "traversing " << position << " to " << n2->position << endl;
	
	//select an array of nodes along the path
	traversedNodes.clear();
	traversalIndexToNodeIndex.clear();
	traversedNodePoints.clear();
	
	Node* cur = n1;
	while(cur != n2){
		traversalIndexToNodeIndex[traversal.getVertices().size()] = traversedNodes.size();

		traversedNodes.push_back(cur);
		traversedNodePoints.addVertex(cur->position);
		traversedNodePoints.addNormal(ofVec3f(0,0,0));
		traversedNodePoints.addColor(ofFloatColor(1));
		
		ofVec3f dirToTarget = (n2->position - cur->position);
		float closestDistance = dirToTarget.lengthSquared();
		Node* closestNode = n2;
		for(int i = 0; i < nodes.size(); i++){

			if(!nodes[i]->replicated){
				continue;
			}
			
			bool containsNode = false;
			for(int j = 0; j < traversedNodes.size(); j++){
				if(traversedNodes[j] == nodes[i]){
					containsNode = true;
					break;
				}
			}
			
			if(containsNode){
//				cout << "	already contains node at index continuing " << i << endl;
				continue;
			}
			
			dirToTarget = (n2->position - position);
			ofVec3f dirToNode = (nodes[i]->position - position);
			float angleTo = dirToTarget.angle(dirToNode);
			if(angleTo > maxTraverseAngle){
				continue;
			}
			
			float lengthSquared = dirToNode.lengthSquared();
			if(lengthSquared < closestDistance){
//				cout << "	current closest node is " << sqrt(lengthSquared) << " at index " << i << endl;
				closestNode = nodes[i];
				closestDistance = lengthSquared;
			}
		}
		
//		cout << " distance " << sqrt(closestDistance) << " / " << position.distance(n2->position) << endl;
		//move towards this node smoothly

		int numSteps = 0;
		float currentDistance = dirToTarget.length();
		while(currentDistance > 2){
			float dampen = ofMap(currentDistance, 20, 2, .05, 1, true);
			direction += ( (dirToTarget / currentDistance) - direction) * dampen;
			direction.normalize();
//			direction = dirToTarget.normalized();
			position  += direction * MIN(1, direction.length());
			traversal.addColor(ofFloatColor(0));
			traversal.addVertex(position);
			dirToTarget = (closestNode->position - position);
			currentDistance = dirToTarget.length();
			if(numSteps++ > 10000){
				cout << "failed with 10000 steps";
				break;
			}
		}
		
//		cout << "added " << numSteps << " steps " << endl;
		cur = closestNode;
		
	}
	

	traversalIndexToNodeIndex[traversedNodes.size()] = traversal.getVertices().size();
	traversedNodes.push_back(n2);
	traversal.addColor(ofFloatColor(0));
	traversal.addVertex(n2->position);
	
	cout << "traversing took " << traversal.getVertices().size() << " steps " << endl;
}

//--------------------------------------------------------------
void testApp::exit(){
	camTrack.disable();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == ' '){
		timeline.togglePlay();
	}
	
	if(key == 'g'){
		generate();
		gui.saveToFile("settings.xml");
	}
	
	if(key == 't'){
		traverse();
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
	
	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(key == 'R'){
		rendering = !rendering;
		if(rendering){
			char folder[1024];
			sprintf(folder, "frames_%02d_%02d_%02d/",ofGetDay(), ofGetHours(), ofGetMinutes());
			renderFolder = folder;
			frameNum = 0;
			ofDirectory(renderFolder).create(true);
			timeline.setCurrentFrame(timeline.getInFrame());
			timeline.play();
		}
	}
	
	if(key == 'S'){
		loadShader();
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

}