//
//  CloudInterludeForceDrag.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//

#ifndef CloudsInterlude_CloudInterludeForceDrag_h
#define CloudsInterlude_CloudInterludeForceDrag_h

#include "CloudInterludeForce.h"
class CloudInterludeForceDrag : public CloudInterludeForce {
  public:
    float dragForce;
    CloudInterludeForceDrag() {
        dragForce = 1.0;
    }
    
    void applyForce(vector<CloudInterludeParticle>& particles){
        //float oneMinusDrag = (1.0 - dragForce);
        for(int i = 0; i < particles.size(); i++){
            particles[i].velocity -= (particles[i].direction * dragForce);
        }
    }
};

#endif
