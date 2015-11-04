#pragma once

#include "ofMain.h"

class IIR4{
public:
    void init(float _b0, float _b1, float _b2, float _b3, float _b4, float _a0, float _a1, float _a2, float _a3, float _a4);
    float calc(float _x);
    
    
    
    // coefficients
    float a0;
    float a1;
    float a2;
    float a3;
    float a4;
    
    float b0;
    float b1;
    float b2;
    float b3;
    float b4;
    
    // data stored
    float x_b0;
    float x_b1;
    float x_b2;
    float x_b3;
    float x_b4;
    
    float x_a1;
    float x_a2;
    float x_a3;
    float x_a4;
    
    float y;
    
    
};
class IIR4Vec3f{
public:
    void init(float _b0, float _b1, float _b2, float _b3, float _b4, float _a0, float _a1, float _a2, float _a3, float _a4);
    ofVec3f calc(ofVec3f _x);
    
    IIR4 x;
    IIR4 y;
    IIR4 z;
};

