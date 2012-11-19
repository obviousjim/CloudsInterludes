//
//  Node.cpp
//  emptyExample
//
//  Created by Jim on 11/16/12.
//
//

#include "Node.h"

bool nodesort(Node* a, Node* b){
	return a->actualDistance > b->actualDistance;
}

Node::Node(){
	terminated = false;
	leaf = true;
	replicated = false;
	generation = 0;
	parent = NULL;
	startIndex = 0;
	endIndex = 0;
	endEnergy = startEnergy = 0;
}

void Node::replicate(vector<Node*>& allNodes, vector<ofVec3f>& fusePoints){
	leaf = false;
	replicated = true;
	vector<Node*> newNodes;


	for(int i = 0; i < numReplications; i++){
		Node* n = new Node();
		
		//default attributes
		n->sourceId = sourceId;
		n->minDistance = minDistance;
		n->distanceRange = distanceRange;
		n->numReplications = numReplications;
		n->mesh = mesh;
		n->stepSize = stepSize;
		n->generation = generation+1;
		
		n->lineColor = lineColor;
		n->nodeColor = nodeColor;
		
		n->position = position;
		n->direction = ofRandomPointOnUnitSphere();
		n->parent = this;
		n->startEnergy = n->endEnergy = endEnergy;
		n->minAttractRadius = minAttractRadius;
		n->minRepelRadius = minRepelRadius;
		n->minFuseRadius = minFuseRadius;
		n->numSurvivingBranches = numSurvivingBranches;
		n->maxAttractForce = maxAttractForce;
		n->maxRepelForce = maxRepelForce;
		n->minFuseRadius = minFuseRadius;
		n->leaf = false;
		
		n->numPointsAtReplicate = numPointsAtReplicate;
		n->replicatePointDistance = replicatePointDistance;
		
		n->actualDistance = minDistance * ofRandom(distanceRange);
		float distanceTraveled = 0;
		
		float alpha = .2;
		//blank vertices befor and after
		mesh->addColor(ofFloatColor(0));
		mesh->addVertex(position);

		n->startIndex = mesh->getNumVertices();

		mesh->addColor(ofFloatColor(alpha));
		mesh->addVertex(position);
		
		Node* terminationNode = NULL;

		while(distanceTraveled < n->actualDistance){

			float minRepelSqr = minRepelRadius*minRepelRadius;
			float minAttractSqr = minAttractRadius*minAttractRadius;
			float minFuseSqr = minFuseRadius*minFuseRadius;
			
			ofVec3f force = ofVec3f(0,0,0);
			for(int ni = 0; ni < allNodes.size(); ni++){
				if(allNodes[ni]->terminated){
					continue;
				}
				float distSquared = allNodes[ni]->position.distanceSquared(n->position);
				if( allNodes[ni]->sourceId != n->sourceId && distSquared < minAttractSqr){
					//if distSquared == minAttractSqr no force, otherwise max force
					if(distSquared < minFuseRadius){
						terminationNode = allNodes[ni];
						//fusePoints.push_back(terminationNode->position);
						break;
					}
					else{
						ofVec3f forceDirection = (allNodes[ni]->position - n->position).normalized();
						float forceMagnitude = maxAttractForce * powf(1.0 - (distSquared / minAttractSqr), 2);
						force += forceDirection * forceMagnitude;
					}
				}
				else if(allNodes[ni]->sourceId == n->sourceId && distSquared < minRepelSqr){
					ofVec3f forceDirection = (n->position - allNodes[ni]->position).normalized();
					float forceMagnitude = maxRepelForce * powf(1.0 - (distSquared / minAttractSqr), 2);
					force += forceDirection * forceMagnitude;
				}
			}
			
			if(terminationNode != NULL){
				break;
			}
			
			//integrate
			n->direction += force;
			n->direction.normalize();
			n->endEnergy++;
			n->position += n->direction*stepSize;
			mesh->addColor(alpha);
			mesh->addVertex(n->position);
			distanceTraveled += stepSize;
			
		}

 		n->endIndex = mesh->getNumVertices();
 
		mesh->addColor(ofFloatColor(0));
		mesh->addVertex(n->position);
		
		newNodes.push_back(n);
		allNodes.push_back(n);
		
		if(terminationNode != NULL){
			n->terminate();
			terminationNode->terminate();
			break;
		}
		
	}
	
	for(int i = 0; i < numPointsAtReplicate; i++){
		fusePoints.push_back(position + ofRandomPointOnUnitSphere() * powf(ofRandomuf(),2)*replicatePointDistance);
	}
	
	//cut off shortest paths
	sort(newNodes.begin(), newNodes.end(), nodesort);
	for(int i = 0; i < MIN(numSurvivingBranches,newNodes.size()); i++){
		newNodes[i]->leaf = !newNodes[i]->terminated;
	}
	
	for(int i = numSurvivingBranches; i < newNodes.size(); i++){
		newNodes[i]->finalize();
		//cout << "fading out with max energy" << newNodes[i]->endEnergy << endl;
	}
}

void Node::finalize(){
	fade(endEnergy);
}

void Node::fade(int maxEnergy){

	int energyIndex = startEnergy;
	for(int i = startIndex; i < endIndex; i++){
		if(mesh->getColor(i).a > 0){
			float alpha = ofMap(energyIndex++, 0, maxEnergy, .8, 0);
			//mesh->setColor(i, ofFloatColor(alpha*125./255,alpha*142/255., alpha*193/255.));
			mesh->setColor(i, nodeColor*alpha);
		}
	}
	
	if(parent != NULL){
		parent->fade(maxEnergy);
	}
}

void Node::terminate(){
	
	if(terminated){
		return;
	}
	
	if(parent!=NULL){
		terminated = true;
		leaf = false;
		for(int i = startIndex; i < endIndex; i++){
			if(mesh->getColor(i).a > 0){
//				mesh->setColor(i, ofFloatColor(.7,.4,.2	,1.0));
				mesh->setColor(i, lineColor);
				//mesh->setColor(i, ofFloatColor(.3,.5,.3));
			}
		}
		
		parent->terminate();
	}
}

