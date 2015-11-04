//
//  qualityExtractor.h
//  LeapTracker
//
//  Created by Jonas Fehr on 31/03/15.
//
//

#pragma once

#include "ofMain.h"
#include "IIR4.h"
#include "ofxOsc.h"
#include "ofxBiquadFilter.h"



struct Qualities
{
    ofVec3f position;
    float velocity;
    float distance;
    bool active;
    ofVec3f direction;
};


class qualityExtractWindow{
    
public:
    void setup(int _length);
    void add(ofVec3f _position);
    void update();
    void draw(int _x, int _y);
    void drawTracking(int _x, int _y);
    void setTresholdVelocity(ofVec3f _position);
    void setMaxSpeed(ofVec3f _position);
    void sendOsc(ofxOscSender &sender);


    vector <Qualities> winQualities;
    
    float activity;
    float directivity;
    float energy;
    float consistence;
    string name = "noName";
    
    
private:
    int length;
    float velTreshold;
    float maxMovement;
    float maxVelocity;
    ofVec3f lastPosForMax;
    
    IIR4Vec3f filterPos;
    
    ofxBiquadFilter3f lowPass;

};
class extract{
public:

};
