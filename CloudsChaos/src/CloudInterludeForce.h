//
//  CloudInterludeForce.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//

#ifndef CloudsInterlude_CloudInterludeForce_h
#define CloudsInterlude_CloudInterludeForce_h

#include "ofMain.h"
#include "CloudInterludeParticle.h"

class CloudInterludeForce {
  public:    
    virtual void applyForce(vector<CloudInterludeParticle>& particles) = 0;    
};

#endif
