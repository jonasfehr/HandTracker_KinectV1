#include "ofApp.h"


void ofApp::setup() {
    ofSetFrameRate(30);
    
    nearThreshold = 255;//212;
    farThreshold = 146;
    minArea = 1000;
    maxArea = 70000;
    threshold = 15;
    persistence = 15;
    maxDistance = 32;
    
    ofSetVerticalSync(true);
    
    blobColors[0] = ofColor(255, 0, 0);
    blobColors[1] = ofColor(0, 255, 0);
    blobColors[2] = ofColor(0, 0, 255);
    blobColors[3] = ofColor(255, 255, 0);
    blobColors[4] = ofColor(255, 0, 255);
    blobColors[5] = ofColor(0, 255, 255);
    blobColors[6] = ofColor(255, 127, 0);
    blobColors[7] = ofColor(127, 0, 255);
    blobColors[8] = ofColor(0, 255, 127);
    blobColors[9] = ofColor(127, 255, 0);
    blobColors[10]= ofColor(255, 0, 127);
    blobColors[11]= ofColor(0, 127, 255);
    
    // set up kinect
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();

        
    kpt.loadCalibration("/Users/Fehrj1/Desktop/calibration.xml");
    
    
    // setup gui
    gui = new ofxUICanvas();
    gui->setHeight(800);
    gui->setName("parameters");
    gui->addLabel("kinect");
    gui->addSpacer();
    gui->addSlider("nearThresh", 0, 255, &nearThreshold);
    gui->addSlider("farThresh", 0, 255, &farThreshold);
    gui->addLabel("contours");
    gui->addSpacer();
    gui->addIntSlider("imgIndx", 1, 6, &imgIndx);

    
    detectHands.setup(640, 480);
    
    senderToFlock.setup(HOST, PORT_toFlock);
    senderToMax.setup(HOST, PORT_toMax);
    
    longWindow.name = "Long";
    midWindow.name = "Mid";
    shortWindow.name = "Short";
    
    longWindow.setup(30);
    midWindow.setup(20);
    shortWindow.setup(10);

    isTracking = isTrackingOld = false;
}

void ofApp::update() {
    kinect.update();
    
    vector<ofVec2f> handPos2D;
    ofxOscMessage m;
    vector<Hand> hands;
    
    if(kinect.isFrameNew()) {
        // process kinect depth image
        ofPixels bufferImage;
        bufferImage.allocate(kinect.width, kinect.height, 1);
        bufferImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, 1);
   /*     bool justActiveArea = true;
        if(justActiveArea) {
            // draw an area where information is tracked
            IplImage* areaMask = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
            cvCircle(areaMask,ofxCv::toCv(ofVec2f(640/2, 480/2)),240,CV_RGB(255,255,255),-1,CV_AA,0);
            
            
            cvAnd(grayImage.getCvImage(), areaMask, grayImage.getCvImage(), NULL);
            grayImage.flagImageChanged();
            
        }*/
        detectHands.update(bufferImage, nearThreshold, farThreshold, imgIndx);
        
        hands = detectHands.getHands();
        
    }
    
    if(!hands.empty()){
        ofPoint point =  hands[0].palmCenter;

        //ofVec3f handPosReal = kinect.getWorldCoordinateAt(handPos2D[0].x,handPos2D[0].y);
        ofVec3f handPosReal = ofVec3f(point.x,point.y,0);//
        longWindow.add(handPosReal);
        longWindow.update();
        longWindow.sendOsc(senderToMax);
        
        midWindow.add(handPosReal);
        midWindow.update();
        midWindow.sendOsc(senderToMax);
        
        shortWindow.add(handPosReal);
        shortWindow.update();
        shortWindow.sendOsc(senderToMax);
        
        isTracking = true;
    } else {
        isTracking = false;
    }
    
    if(isTracking && !isTrackingOld){
        m.clear();
        m.setAddress("/isTracking");
        m.addIntArg(1);
        senderToMax.sendMessage(m);
        isTrackingOld = isTracking;
    }
    if(!isTracking && isTrackingOld){
        m.clear();
        m.setAddress("/isTracking");
        m.addIntArg(0);
        senderToMax.sendMessage(m);
        isTrackingOld = isTracking;
    }
    
    
    // Send coordinates of contours to the Flock
    
    ofRectangle flockSpace = ofRectangle(0, 0, FLOCK_RESOLUTION_X, FLOCK_RESOLUTION_Y);
    
    m.clear();
    m.setAddress("/numOfHandsDetected");
    m.addIntArg(hands.size());
    senderToFlock.sendMessage(m);
    
    for(int i = 0; i<hands.size(); i++){
        ofPolyline simpleContour;
        simpleContour = hands[i].boundary;
        simpleContour.simplify(0.5f);
        float simplifyAmount = .1f;
        // simplify more if still too big
        while (simpleContour.size() > 200){
            simpleContour.simplify(simplifyAmount);
            simplifyAmount += .1f;
        }
        simpleContour.close();
        m.clear();
        m.setAddress("/Hand_"+ofToString(i+1)+"_Contour");

        // the first two values are the position ot the hand
        ofPoint point =  hands[i].palmCenter;
        ofVec3f wp = kinect.getWorldCoordinateAt(point.x, point.y);
        ofVec2f pp = kpt.getProjectedPoint(wp);
        point.x = pp.x*FLOCK_RESOLUTION_X;//ofMap(pp.x, 0, 1, 0, flockSpace.width);
        point.y = pp.y*FLOCK_RESOLUTION_Y;// ofMap(pp.y, 0, 1, 0, flockSpace.height);
            m.addIntArg(point.x);
            m.addIntArg(point.y);

        //ofVec2f vel = shortWindow.winQualities[0].velocity; // optional velocity would be interesting for the collisions with the boids
        
        
        for(int j = 0; j<simpleContour.size(); j++){
            ofPoint point =  simpleContour[j];
            ofVec3f wp = kinect.getWorldCoordinateAt(point.x, point.y);
            ofVec2f pp = kpt.getProjectedPoint(wp);
            point.x = pp.x*FLOCK_RESOLUTION_X;//ofMap(pp.x, 0, 1, 0, flockSpace.width);
            point.y = pp.y*FLOCK_RESOLUTION_Y;// ofMap(pp.y, 0, 1, 0, flockSpace.height);
            
            
            if(flockSpace.inside(point)|| (j>hands[i].indxBaseStart && j<hands[i].indxBaseEnd)){ // check if the point actually is inside the projected .
                m.addIntArg(point.x);
                m.addIntArg(point.y);
            }
        }
        senderToFlock.sendMessage(m);
    }
    
    
}

void ofApp::draw() {
    // GUI
    ofBackground(0);
    ofSetColor(255);
    ofPushMatrix();
    kinect.draw(0, 0);
    detectHands.draw();

    ofTranslate(640, 0);
    detectHands.drawContours();
    detectHands.draw();
    ofSetColor(0,255,0);
    longWindow.drawTracking(0,0);
    ofSetColor(0,0,255);
    midWindow.drawTracking(0,0);
    ofSetColor(255,0,0);
    shortWindow.drawTracking(0,0);
    
    ofTranslate(-640, 480);

    
    ofTranslate(640, 0);


    
    ofPopMatrix();

    shortWindow.draw(10, 500);
    midWindow.draw(230, 500);
    longWindow.draw(450, 500);
    
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void ofApp::keyPressed(int key) {
    if(key == 'e') {
        detectHands.createMask();
        
    }
    if(key == 'r') {
        detectHands.resetMask();
        
    }
    if(key == 'g') {
        detectHands.createGenericMask();
        
    }
    
    vector<ofVec2f> handPos2D = detectHands.getHandPos();
    ofVec3f handPosReal = ofVec3f(0,0,0);
    if(!handPos2D.empty())
        handPosReal = ofVec3f(handPos2D[0].x,handPos2D[0].y,0); //kinect.getWorldCoordinateAt(handPos2D[0].x,handPos2D[0].y);
    if(key=='t')
    {
        shortWindow.setTresholdVelocity(handPosReal);
        longWindow.setTresholdVelocity(handPosReal);
        midWindow.setTresholdVelocity(handPosReal);
    }
    if(key=='m')
    {
        longWindow.setMaxSpeed(handPosReal);
        midWindow.setMaxSpeed(handPosReal);
        shortWindow.setMaxSpeed(handPosReal);
    }

    
}