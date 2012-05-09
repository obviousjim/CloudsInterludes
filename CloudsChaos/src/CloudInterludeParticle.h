//
//  CloudInterludeParticle.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CloudsInterlude_CloudInterludeParticle_h
#define CloudsInterlude_CloudInterludeParticle_h

#include "ofMain.h"

class CloudInterludeParticle {
  public:
    CloudInterludeParticle(){
        energy = 0;
        position = ofVec3f(0,0,0);
        velocity = ofVec3f(0,0,0);
        force = ofVec3f(0,0,0);
        connectorEnergy = 0;
        hasType = false;
    }
    
    void update(){
        velocity += force;
        position += velocity;
        direction = velocity.getNormalized();
        force = ofVec3f(0,0,0);
        energy--;
        connectorEnergy = MAX(connectorEnergy-1, 0);	
        energyPercent = energy/initialEnergy;
    }
    
    ofVec3f connectorPoint;
    float connectorEnergy;
    bool hasType;
    float energy;
    float initialEnergy;
    float energyPercent;
    ofVec3f origin;
    ofVec3f position;
    ofVec3f velocity;
    ofVec3f force;
    ofVec2f texcoord;
    ofVec3f direction;
    
};


#endif
