//
//  CloudInterludeParticleGenerator.h
//  CloudsInterlude
//
//  Created by James George on 5/4/12.
//

#ifndef CloudsInterlude_CloudInterludeParticleGenerator_h
#define CloudsInterlude_CloudInterludeParticleGenerator_h

#include "ofMain.h"
#include "CloudInterludeParticle.h"
#include "CloudInterludeForce.h"

class CloudInterludeParticleGenerator {
  public:
    CloudInterludeParticleGenerator(){
        birthRate = 10;
        lifespan = 200;
        numToBear = 0;
        //setup forces
    }
    
    void update(){
        
        numToBear += birthRate;
        while(numToBear > 1.0){
            CloudInterludeParticle p;
            p.energy = lifespan;
            p.position = position;
            p.velocity = direction;
            particles.push_back(p);
            numToBear--;
        }
        
        for(int i = 0; i < forces.size(); i++){
            forces[i]->applyForce(particles);
        }
        
//        for(int i = 0; i < particles.size(); i++){
        for(int i = particles.size()-1; i >= 0; i--){
            particles[i].update();
            if(particles[i].energy < 0){
                particles.erase(particles.begin() + i); 
            }
        }
    }
    
    void drawTextDebug(float x, float y){
        ofDrawBitmapString("Num Particles " + ofToString(particles.size()), x, y);
    }
    
    void drawDebug(){
        ofPushStyle();
        ofNoFill();
        ofSetColor(255,0,0);
        ofSphere(position, 20);
        ofLine(position, position+direction*100);
        ofPopStyle();
    }
    
    void drawParticleDebug(){
        ofPushStyle();
        glPointSize(4);
        ofMesh m;
        for(int i = 0; i < particles.size(); i++){
            m.addVertex(particles[i].position);
//            m.addColor(ofFloatColor(particles[i].energy/lifespan));
            if(particles[i].connectorEnergy > 0){
                ofLine(particles[i].position, particles[i].connectorPoint);
            }
        }
        m.drawVertices();
        
       // m.drawWireframe();
        ofPopStyle();
    
    }
    
    vector<CloudInterludeParticle> particles;
    vector<CloudInterludeForce*> forces;
    
    void addForce(CloudInterludeForce* force){
        forces.push_back(force);
    }
    
    float birthRate;
    float lifespan;
    ofVec3f position;
    ofVec3f direction;
    
  protected:
    float numToBear;
};


#endif
