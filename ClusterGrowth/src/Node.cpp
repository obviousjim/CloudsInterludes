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
	targeted = false;
}

void Node::replicate(vector<Node*>& allNodes, vector<ofVec3f>& fusePoints, vector<ofVec3f>& fuseDistances){
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
		n->nodeColorTrack = nodeColorTrack;
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
		n->replicatePointSize = replicatePointSize;
		
		n->numPointsAtReplicate = numPointsAtReplicate;
		n->replicatePointDistance = replicatePointDistance;
		
		n->actualDistance = minDistance * ofRandom(distanceRange);
		
		float alpha = .2;
		//blank vertices befor and after
		mesh->addColor(ofFloatColor(0));
		mesh->addVertex(position);

		n->startIndex = mesh->getNumVertices();

		mesh->addColor(ofFloatColor(alpha));
		mesh->addVertex(position);
		
		float minRepelSqr = minRepelRadius*minRepelRadius;
		float minAttractSqr = minAttractRadius*minAttractRadius;
		float minFuseSqr = minFuseRadius*minFuseRadius;

		Node* targetNode = NULL;
		Node* terminationNode = NULL;
		float closestDistance = 100000;
		for(int i = 0; i < allNodes.size(); i++){
			if(
			   allNodes[i]->replicated &&
			   !allNodes[i]->targeted &&
			   allNodes[i]->sourceId != n->sourceId)
			{
				
				float distSquared = allNodes[i]->position.distanceSquared(n->position);
				if(distSquared < closestDistance) {
					targetNode = allNodes[i];
					closestDistance = distSquared;
				}
			}
		}
		
		if(targetNode != NULL){
			targetNode->targeted = true;
		}
		
		//cout << "target node is " << sqrt(closestDistance) << " away " << endl;
		float distanceTraveled = 0;
		while(distanceTraveled < n->actualDistance){
		
			ofVec3f force = ofVec3f(0,0,0);
			if(targetNode != NULL){
				float distToTarget = targetNode->position.distanceSquared(n->position);
				if(distToTarget < minFuseRadius){
					terminationNode = targetNode;
					break;
				}
				else{
					ofVec3f forceDirection = (targetNode->position - n->position).normalized();
					float forceMagnitude = maxAttractForce * powf(1.0 - (distToTarget / minAttractSqr), 2);
					force += forceDirection * forceMagnitude;
					
					/////////
					n->direction = forceDirection;
					/////////
				}
			}
			else {
			
				for(int ni = 0; ni < allNodes.size(); ni++){
					
					if(allNodes[ni]->terminated){
						continue;
					}
					
					float distSquared = allNodes[i]->position.distanceSquared(n->position);
					if(allNodes[ni]->sourceId == n->sourceId && distSquared < minRepelSqr){
						ofVec3f forceDirection = (n->position - allNodes[ni]->position).normalized();
						float forceMagnitude = maxRepelForce * powf(1.0 - (distSquared / minAttractSqr), 2);
						force += forceDirection * forceMagnitude;
					}
				}
			}
			
			if(terminationNode != NULL){
				mesh->addColor(ofFloatColor(alpha));
				mesh->addVertex(targetNode->position);
				n->endEnergy++;
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
		
		/*
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
		*/
		
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
	
	//TO DRAW CLUSTERS
	/*
	for(int i = 0; i < numPointsAtReplicate; i++){
		ofVec3f point = position + ofRandomPointOnUnitSphere() * powf(ofRandomuf(),2)*replicatePointDistance;
		fusePoints.push_back(point);
		float spriteSize = ofMap(point.distance(position),0,replicatePointDistance, 1.0, replicatePointSize);
		fuseDistances.push_back(ofVec3f(spriteSize,0,0));
	}
	 */
	
	//JUST ONE NODE
	ofVec3f point = position;
	fusePoints.push_back(point);
	float spriteSize = replicatePointSize;
	fuseDistances.push_back(ofVec3f(spriteSize,0,0));
	
	for(int i = 0; i < allNodes.size(); i++){
		allNodes[i]->targeted = false;
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
	fade(endEnergy, true);
}

void Node::fade(int maxEnergy, bool leaf){

	int energyIndex = startEnergy;
	for(int i = startIndex; i < endIndex; i++){
		if(mesh->getColor(i).a > 0){
			float positionOnPath = ofMap(energyIndex++, 0, maxEnergy, 1.0, 0);
			float alpha = 0.0;
			if(leaf){
				//alpha = ofMap(i, startIndex, endIndex, 1.0, 0);
				alpha = 1.0;
			}
			ofFloatColor color = nodeColorTrack->getColorAtPosition(positionOnPath);
//			alpha *= .8;
			//mesh->setColor(i, ofFloatColor(alpha*125./255,alpha*142/255., alpha*193/255.));
			mesh->setColor(i, color*alpha);
		}
	}
	
	if(parent != NULL){
		parent->fade(maxEnergy, false);
	}
}

void Node::terminate(){
	
	if(terminated){
		return;
	}
	cout << "Terminated" << endl;
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

