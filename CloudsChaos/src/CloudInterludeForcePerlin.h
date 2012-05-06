//
//  CloudInterludeForcePerlin.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CloudsInterlude_CloudInterludeForcePerlin_h
#define CloudsInterlude_CloudInterludeForcePerlin_h

#include "CloudInterludeForce.h"
class CloudInterludeForcePerlin : public CloudInterludeForce {
  public:
    
    float amplitude;
    float density;
    float speed;
    float currentOffset;
    
    CloudInterludeForcePerlin() {
        amplitude = 0;
        density = 0;
        speed = 0;
        currentOffset = 0;
    }
    
    void update(){
        currentOffset += speed;
    }
    
    void applyForce(vector<CloudInterludeParticle>& particles){
        //currentOffset = 0;
        for(int i = 0; i < particles.size(); i++){
            ofVec3f& pos = particles[i].position;
//            particles[i].force +=  ofVec3f(ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset)*amplitude,
//                                           ofSignedNoise(pos.z/density, pos.x/density, pos.y/density, currentOffset)*amplitude,
//                                           ofSignedNoise(pos.y/density, pos.z/density, pos.x/density, currentOffset)*amplitude );
            particles[i].force +=  ofVec3f(ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset)*amplitude,
                                           ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+1000)*amplitude,
                                           ofSignedNoise(pos.x/density, pos.y/density, pos.z/density, currentOffset+2000)*amplitude );
        }
    }
};

#endif
