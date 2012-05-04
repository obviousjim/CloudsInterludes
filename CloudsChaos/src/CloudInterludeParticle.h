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
    CloudInterludeParticle();
    
    void update();
    
    ofVec3f position;
    ofVec3f velocity;
    ofVec3f force;
};


#endif
