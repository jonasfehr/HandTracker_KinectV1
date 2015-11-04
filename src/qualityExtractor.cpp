//
//  qualityExtractor.cpp
//  LeapTracker
//
//  Created by Jonas Fehr on 31/03/15.
//
//

#include "qualityExtractor.h"

void qualityExtractWindow::setup(int _length)
{
    length = _length;
    velTreshold = 100;
    maxMovement = 50;
    maxVelocity = 800;
    
    filterPos.init(0.1400982208, -0.0343775491, 0.0454003083, 0.0099732061, 0.0008485135,  1, -1.9185418203, 1.5929378702, -0.5939699187, 0.0814687111); // 1.5 Hz
    //filterPos.init(0.1851439645, 0.1383283833, 0.1746892243, 0.1046627716, 0.0464383730,     1,     -1.2982434912, 1.4634092217, -0.7106501488, 0.2028836637); //4.5 Hz
    //filterPos.init(0.3730569536, 0.8983119412, 0.9660856693, 0.5189611913, 0.1099005390,         1,       0.8053107424, 0.8110594452,  0.2371869724, 0.0849291749); // 7.5 Hz
    

    lowPass.setFc(0.5);
}


void qualityExtractWindow::add(ofVec3f _position)
{
    
    Qualities newInput;
    int winQuSize = winQualities.size();
    Qualities lastInput;
    if(winQualities.size() > 0) lastInput = winQualities.back();
    
    // Protect of jitter
    float dist = lastInput.position.distance(_position);

    
//   if( dist > maxMovement ) winQualities.clear();// DO SOMETHING idea: set flag and when the next call, calculate midpoint to the previous
    
    
    // Position
    lowPass.update(_position);
    
    newInput.position = filterPos.calc(lowPass.value());//_position);
    
    // Distance
    dist = lastInput.position.distance(newInput.position);
    newInput.distance = dist;
    
    // calculate Velocity

    newInput.velocity = dist / ofGetLastFrameTime();
    
    // detect if active
    if(newInput.velocity > velTreshold){
        newInput.active = true;
    }else{
        newInput.active = false;
    }
    
    // Create unit Vector of direction moving
    ofVec3f direction = newInput.position.operator-(lastInput.position);
    direction.normalize();
    newInput.direction = direction;
    
    // add new points to vector;
    winQualities.push_back(newInput);
    
    // make window slide
    if(winQualities.size()>length) winQualities.erase(winQualities.begin());
}

void qualityExtractWindow::update(){
    int windowLength = winQualities.size();
    activity = 0;
    energy = 0;
    consistence = 0;
    
    ofVec3f pointA =   winQualities[0].position;
    ofVec3f pointB =   winQualities[windowLength].position;
    
    float pathLength;
    
  //  ofVec3f generalDirection = (pointA.operator-(pointB)).normalized();

    
    for(int i = 0; i<windowLength; i++)
    {
        if (winQualities[i].active) activity++;
        
        energy += winQualities[i].velocity;
        
        pathLength += winQualities[i].distance;
        
     //   consistence += generalDirection.dot(winQualities[i].direction);

    }
    //alternative approach
    int numOfSam = 10;
    int stepsBetween = windowLength/numOfSam;
    for(int i = 0; i<numOfSam; i++)
    {
        if(i < numOfSam-1){
            float ratioDirection = winQualities[i*stepsBetween].direction.dot(winQualities[(i+1)*stepsBetween].direction);
            
            consistence += ratioDirection;
            
        }
        
        
        
    }
    
    

    activity /= windowLength;
    
    
    energy /= windowLength;
    energy /= maxVelocity;
    
    directivity = pointA.distance(pointB)/pathLength; // Maybe sqrt to get an exponential curve??
    
    consistence /= numOfSam;
    if(consistence < 0) consistence = 0;
  
}

void qualityExtractWindow::draw(int _x, int _y){
    int gap = 30;

    
    ofPushMatrix();
    ofTranslate(_x, _y);

    
    ofSetColor(255);
    ofDrawBitmapString(name+" size: " + ofToString(length), 0, 0.5*gap);
    ofDrawBitmapString("Activity    : " + ofToString(activity), 0, gap);
    ofDrawBitmapString("Energy      : " + ofToString(energy), 0, 2*gap);
    ofDrawBitmapString("Directivity : " + ofToString(directivity), 0, 3*gap);
    ofDrawBitmapString("Consistence : " + ofToString(consistence), 0, 4*gap);
    ofDrawBitmapString("maxV: " + ofToString(maxVelocity), 0, 5*gap);
    ofDrawBitmapString("maxM: " + ofToString(maxMovement), 0, 5.5*gap);
    ofDrawBitmapString("tVel: " + ofToString(velTreshold), 0, 6*gap);


    ofSetLineWidth(1);
    ofSetColor(255, 255);
    ofNoFill();
    ofRect(0, gap+5, 200, 10);
    ofRect(0, 2*gap+5, 200, 10);
    ofRect(0, 3*gap+5, 200, 10);
    ofRect(0, 4*gap+5, 200, 10);


    ofFill();
    ofRect(0, gap+5, 200*activity, 10);
    ofRect(0, 2*gap+5, 200*energy, 10);
    ofRect(0, 3*gap+5, 200*directivity, 10);
    ofRect(0, 4*gap+5, 200*consistence, 10);
    
    ofSetColor(255, 50);
    ofRect(-4, 0, 208, 6.*gap);
    ofPopMatrix();

}
void qualityExtractWindow::drawTracking(int _x, int _y){
    ofPushMatrix();
    ofTranslate(_x,_y);
    ofPolyline poly;
    for(int i = 0; i < winQualities.size(); i++){
        poly.addVertex(winQualities[i].position);

    }
    poly.draw();
    
    ofPopMatrix();
}

void qualityExtractWindow::setTresholdVelocity(ofVec3f _position){
   
    float dist = lastPosForMax.distance(_position);
    lastPosForMax = _position;
    float velocity = dist / ofGetLastFrameTime();
    velTreshold = velTreshold*0.81 + velocity*0.1;
    velTreshold *= 1.1; // add 10%
}

void qualityExtractWindow::setMaxSpeed(ofVec3f _position){
    
    float dist = lastPosForMax.distance(_position);
    lastPosForMax = _position;
    float velocity = dist / ofGetLastFrameTime();
    maxVelocity = maxVelocity*0.8 + velocity*0.2;
    maxMovement = maxMovement*0.8 + dist*0.2;
    cout<< ofToString(velocity)+"\n";
}
void qualityExtractWindow::sendOsc(ofxOscSender &sender){
    ofxOscMessage m;
    m.setAddress("/"+name);
    m.addFloatArg(activity);
    m.addFloatArg(energy);
    m.addFloatArg(directivity);
    m.addFloatArg(consistence);
    
    sender.sendMessage(m);
}