//
//  CloudInterludeForceMeshAttractor.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//

#ifndef CloudsInterlude_CloudInterludeForceMeshAttractor_h
#define CloudsInterlude_CloudInterludeForceMeshAttractor_h

#include "ofMain.h"
class CloudInterludeForceMeshAttractor : public CloudInterludeForce {
  public:

    CloudInterludeForceMeshAttractor(){
        
    }
    
    void applyForce(vector<CloudInterludeParticle>& particles){

        int step = 200;
        int rand = ofRandom(step);
        float minRadiusSqr = minRadius*minRadius;
            //for(int m = rand; m < mesh->getVertices().size(); m+=step) {
        for(int m = 0; m < mesh->getIndices().size(); m += 3) {
            for(int i = 0; i < particles.size(); i+=step){
                if( particles[i].position.distanceSquared( mesh->getVertex( mesh->getIndex(m) )) < minRadiusSqr){
     //               ofVec3f toPointOnMesh = particles[i].position - mesh->getVertex(m);
    //                float distanceToMesh = toPointOnMesh.length();
  //                  ofVec3f directionToMesh = toPointOnMesh / distanceToMesh;
//                    particles[i].force += directionToMesh * (attractScale / distanceToMesh); 
                    particles[i].connectorPoint = mesh->getVertex( mesh->getIndex(m) );
                    particles[i].connectorEnergy = 10;
                }
            }
        }
    }
                                                 
    ofMesh* mesh;
    float minRadius;
    float attractScale;                                        
};

#endif
