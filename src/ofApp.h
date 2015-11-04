#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "ofxHandDetect.h"
#include "IIR4.h"
#include "qualityExtractor.h"


// this must match the display resolution of your projector
#define FLOCK_RESOLUTION_X 1024
#define FLOCK_RESOLUTION_Y 1024

// Variables for OSC
#define HOST "localhost"
#define PORT_toFlock 7501
#define PORT_toMax 7504



using namespace ofxCv;
using namespace cv;



class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	    
    ofxUICanvas                 *gui;
    ofColor                     blobColors[12];
    
	ofxKinectProjectorToolkit   kpt;
    ofxKinect                   kinect;
    
    
    
    ofxHandDetect               detectHands;
	
	float                       nearThreshold;
    float                       farThreshold;
    float                       minArea;
    float                       maxArea;
    float                       threshold;
    float                       persistence;
    float                       maxDistance;
    int                         imgIndx;
    
    bool                        isTracking;
    bool                        isTrackingOld;

    
    ofxOscSender senderToFlock;
    ofxOscSender senderToMax;
    
    qualityExtractWindow longWindow;
    qualityExtractWindow midWindow;
    qualityExtractWindow shortWindow;

};
