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
        numToBear = ofRandom(0,1);
        //setup forces
    }
    
    void update(){
        
        numToBear += birthRate;
        while(numToBear > 1.0 && remainingParticles > 0){
            CloudInterludeParticle p;
            p.energy = p.initialEnergy = lifespan + ofRandom(-lifespanVariance/2, lifespanVariance/2);
            p.origin = p.position = position;
            p.velocity = direction;
            if(showType && ofRandomuf() < typeChance){
                p.hasType = true;
            }
            particles.push_back(p);
            numToBear--;
            remainingParticles--;
        }
        
        //sacrifice the rest
        numToBear -= int(numToBear);
        
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
    
    /*
    void drawParticleDebug(){
        ofPushStyle();
        glPointSize(4);
        ofMesh m;
        for(int i = 0; i < particles.size(); i++){
            m.addVertex(particles[i].position);
//            m.addColor(ofFloatColor(particles[i].energy/lifespan));
//            if(particles[i].connectorEnergy > 0){
//                ofLine(particles[i].position, particles[i].connectorPoint);
//            }
        }
        m.drawVertices();
        
       // m.drawWireframe();
        ofPopStyle();
    }
     */
    
    vector<CloudInterludeParticle> particles;
    vector<CloudInterludeForce*> forces;
    
    void addForce(CloudInterludeForce* force){
        forces.push_back(force);
    }
    
    int remainingParticles;
    float birthRate;
    float lifespan;
    float lifespanVariance;
    ofVec3f position;
    ofVec3f direction;

    bool showType;
    float typeChance;
  protected:
    float numToBear;
};


#endif
