//
//  handDetect.h
//  conturBounce
//
//  Created by Jonas Fehr on 23/03/15.
// based on https://www.youtube.com/watch?v=th8hUD7Ajg4
// by catsass19
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
//#include <cv.h>
#include <highgui.h>
#include <stdio.h>



class Hand
{
public:
    
    ofVec2f armBase; // Base of the arm (where the arm intersects the edge of the table)
    int indxBaseStart;
    int indxBaseEnd;
    
    ofPolyline boundaryPoly;
    
    vector<ofPoint> boundary;
    
    ofVec2f palmCenter;
    ofVec2f centroid;
    
    
    cv::Mat armBlob = cv::Mat::zeros( cvSize(640, 480), CV_8UC1 );
    cv::Mat armBlob2 = cv::Mat::zeros( cvSize(640, 480), CV_8UC1 );

    
    void draw();
    
    

};



class ofxHandDetect
{
public:
    void setup(int _w, int _h);
    void update(ofPixels _input, int _nearThreshold, int _farThreshold, int _indx_show);
    void draw();
    void drawContours();
    void createMask();
    void resetMask();
    void createGenericMask();

    vector<Hand> getHands();
    vector<ofVec2f> getHandPos();
    vector<ofPolyline> getContours();


    
    ofxCvGrayscaleImage output;
    
private:
    
    void getHandBoundries(cv::Mat _armBlobs);
    ofVec2f findArmBase(cv::Mat tableEdges, Hand &_hand);
    ofVec2f findPalmCenter(Hand _hand);
    
    
    
    //CvCapture *capture;
    ofFbo outputImage;
    ofFbo output2ndApproach;

    
    cv::Mat *frame;
    //IplImage* mask = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
    cv::Mat depthImage;
    cv::Mat inputImage;


    cv::Mat areaMask;

    ofVec2f handPosition;
    vector<Hand> hands;

    
    int width, height;
    
    vector<vector<cv::Point> > contoursLocal;

};
