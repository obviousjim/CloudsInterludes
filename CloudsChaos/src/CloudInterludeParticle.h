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
    }
    
    void update(){
        velocity += force;
        position += velocity;
        force = ofVec3f(0,0,0);
        energy--;
    }
    
    float energy;
    ofVec3f position;
    ofVec3f velocity;
    ofVec3f force;
    
};


#endif
