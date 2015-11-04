#include "IIR4.h"


void IIR4::init(float _b0, float _b1, float _b2, float _b3, float _b4, float _a0, float _a1, float _a2, float _a3, float _a4)
{
    a0 = _a0;
    a1 = _a1;
    a2 = _a2;
    a3 = _a3;
    a4 = _a4;
    
    b0 = _b0;
    b1 = _b1;
    b2 = _b2;
    b3 = _b3;
    b4 = _b4;
    
    
    
    x_b4 = x_b3 = x_b2 = x_b1 = x_b0 = 0;
    x_a4 = x_a3 = x_a2 = x_a1 = 0;
    
    
}

float IIR4::calc(float _x)
{
    x_b4 = x_b3;
    x_b3 = x_b2;
    x_b2 = x_b1;
    x_b1 = x_b0;
    x_b0 = _x;
    
    y = (x_b0*b0 + x_b1*b1 + x_b2*b2 + x_b3*b3 + x_b4*b4 - x_a1*a1 - x_a2*a2 - x_a3*a3 - x_a4*a4) /a0;
    
    x_a4 = x_a3;
    x_a3 = x_a2;
    x_a2 = x_a1;
    x_a1 = y;
    
    
    return y;
}

void IIR4Vec3f::init(float _b0, float _b1, float _b2, float _b3, float _b4, float _a0, float _a1, float _a2, float _a3, float _a4)
{
    x.init(_b0, _b1, _b2, _b3, _b4, _a0, _a1, _a2, _a3, _a4);
    y.init(_b0, _b1, _b2, _b3, _b4, _a0, _a1, _a2, _a3, _a4);
    z.init(_b0, _b1, _b2, _b3, _b4, _a0, _a1, _a2, _a3, _a4);
}

ofVec3f IIR4Vec3f::calc(ofVec3f _x)
{
    return ofVec3f(x.calc(_x.x), y.calc(_x.y), z.calc(_x.z));
}

