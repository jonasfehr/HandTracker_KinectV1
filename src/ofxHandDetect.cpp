//
//  handDetect.c
//  conturBounce
//
//  Created by Jonas Fehr on 23/03/15.
//
//

#include "ofxHandDetect.h"

using namespace cv;

void ofxHandDetect::setup(int _w, int _h)
{
    
    outputImage.allocate(_w, _h, GL_RGBA);//A32F_ARB);
    outputImage.begin();
    ofClear(255,255,255, 0);
    ofNoFill();
    ofSetLineWidth(2);
    ofEnableSmoothing();
    outputImage.end();
    
    output.allocate(_w, _h);
    
    width = _w;
    height = _h;
    
    handPosition = ofVec2f(-1, -1);
    
    // draw the Mask
    areaMask = cv::Mat::zeros( cvSize(width,height), CV_8U );
    
    ofImage imgToLoad;
    imgToLoad.loadImage("areaMask.png");
    ofxCv::toCv(imgToLoad).convertTo(areaMask, CV_8U);
    
    // circle(areaMask,ofxCv::toCv(ofVec2f(width/2, height/2)),height/4,CV_RGB(255,255,255),-1,CV_AA,0);
    // cv::Point p1 = cv::Point(0,0);//ofxCv::toCv(ofVec2f((width-height)/2, 0));
    // cv::Point p2 = cv::Point(width, height);//ofxCv::toCv(ofVec2f((width-height)/2+height, height));
    //  rectangle(areaMask, p1, p2, CV_RGB(255,255,255),-1,CV_AA,0);//
}

void ofxHandDetect::update(ofPixels _input, int _nearThreshold, int _farThreshold,  int _indx_show)
{
    
    /* Mat nearFarCut = Mat::zeros( cvSize(width,height), CV_32FC1 );
     
     nearFarCut  = ofxCv::toCv(_input);
     
     float low1 = _nearThreshold;
     float high1 = _farThreshold;
     float low2 = 1.0;
     float high2 = 0.0;
     nearFarCut = low2 + (ofxCv::toCv(_input) - low1) * (high2 - low2) / (high1 - low1);
     
     nearFarCut.convertTo(depthImage, CV_8UC1, 255);*/
    
    
    ofxCv::toCv(_input).convertTo(depthImage, CV_8UC1, 1);
    
    
    //cut Far and Near
    threshold(depthImage, depthImage, _farThreshold, 0, THRESH_TOZERO);
    threshold(depthImage, depthImage, _nearThreshold, 0, THRESH_TOZERO_INV);
    
    depthImage.copyTo(inputImage);
    
    
    outputImage.begin();
    ofClear(255,255,255, 0);
    outputImage.end();
    
    bool justActiveArea = true;
    if(justActiveArea) {
        bitwise_and(depthImage, areaMask, depthImage);
    }
    
    
    
    
    
    
    
    
    
    
    
    //ERODE first then DILATE to eliminate the noises.
    int closingNum = 1;
    bitwise_not(depthImage, depthImage);
    erode(depthImage, depthImage, cv::Mat(), cv::Point(-1,-1), closingNum);
    dilate(depthImage, depthImage, cv::Mat(), cv::Point(-1,-1), closingNum);
    bitwise_not(depthImage, depthImage);
    
    
    bool armSeparation = true;
    if(armSeparation)
    {
        // After the technique presented in KinectArms
        // Get edges around arms and substract
        // ev. normalize to get bigger differences between uppe rand lower...
        
        // create Canny
        //IplImage* depthImageCanny = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
        Mat depthImageCanny = Mat::zeros( cvSize(width,height), CV_8UC1 );
        
        
        Canny(depthImage, depthImageCanny, 12, 7, 3); // for K:V2 130, 100, 3);
        
        dilate(depthImageCanny, depthImageCanny, cv::Mat(), cv::Point(-1,-1), 2);
        erode(depthImageCanny, depthImageCanny, cv::Mat(), cv::Point(-1,-1), 1);
        
        threshold(depthImage, depthImage, 50, 255, THRESH_BINARY );
        
        Mat depthImageErodedArms = Mat::zeros( cvSize(width,height), CV_8UC1 );
        int closingNum = 1;
        erode(depthImage, depthImageErodedArms, cv::Mat(), cv::Point(-1,-1), closingNum);
        
        // crossing
        Mat armDivider = Mat::zeros( cvSize(width,height), CV_8UC1 );
        
        bitwise_and(depthImageCanny, depthImageErodedArms, armDivider);
        
        //Mat depthImage0 = Mat::zeros( cvSize(width,height), CV_8UC1 );
        //threshold(depthImage, depthImage0, 0, 255, THRESH_BINARY_INV);
        
        //bitwise_or(armDivider, depthImage0, armDivider); // Maybe not
        
        
        
        //diliate the devider
        closingNum = 1;
        erode(armDivider, armDivider, cv::Mat(), cv::Point(-1,-1), closingNum);
        
        Mat armEdges = Mat::zeros( cvSize(width,height), CV_8UC1 );
        
        
        bitwise_or(depthImageCanny, armDivider, armEdges); // edges depthImage
        bitwise_not(armEdges, armEdges); // invert for && depthImageing
        
        
        bitwise_and(depthImage,armEdges,depthImage);
        //bitwise_xor(depthImage,armDivider,depthImage); // would do the job also
        
        IplImage* img ;
        switch (_indx_show){
            case 1: img = new IplImage(depthImage);
                break;
            case 2: img = new IplImage(depthImageCanny);
                break;
            case 3: img = new IplImage(depthImageErodedArms);
                break;
            case 4: img = new IplImage(armDivider);
                break;
            case 5: img = new IplImage(armEdges);
                break;
            case 6: img = new IplImage(areaMask);
                break;
            default: img = new IplImage(depthImage);
                break;
        }
      //  if(!hands.empty())img = new IplImage(hands[0].armBlob2);
        cvCopy(img, output.getCvImage());
        output.flagImageChanged();
        
    }
    
    /* IplImage* depthImageImg = new IplImage(depthImage);
     cvFindContours( depthImageImg, storage, &contours, sizeof(CvContour),
     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );
     
     // We choose the first contour in the list which is longer than 650.
     // You might want to change the threshold to which works the best for you.
     
     while(contours && contours->total <= 500)
     {
     contours = contours->h_next;
     }
     
     cvDrawContours( frame, contours, CV_RGB(100,100,100), CV_RGB(0,255,0), 1, 2, CV_AA, cvPoint(0,0) );
     
     //
     // Use a rectangle to cover up the contour.
     // Find the center of the rectangle (armcenter). Fingertip() needs it.
     //
     if(contours)
     {
     contourcenter =  cvMinAreaRect2(contours,0);
     armcenter.x = cvRound(contourcenter.center.x);
     armcenter.y = cvRound(contourcenter.center.y);
     //cvCircle(frame,armcenter,10,CV_RGB(255,255,255),-1,8,0);
     getconvexhull();
     fingertip();
     hand();
     }*/
    
    // 2nd approach
    
    
    getHandBoundries(depthImage);
    
    
    Mat dilatedTableEdges = Mat::zeros( cvSize(width,height), CV_8UC1 );
    erode(areaMask, dilatedTableEdges, cv::Mat(), cv::Point(-1,-1), 5); // last 2 closing num, may change
    
    
    
    for(int i=0; i<hands.size(); i++)
    {
        // Find base of each arm
        Hand hand = hands[i];
        hands[i].armBase = findArmBase(dilatedTableEdges, hand);
        
        
        // Calculate the center of geometry -> allready done in getHandsBoundaries
        
         /*ofVec2f center;
        for(size_t j=0; j<hands[i].boundary.size(); j++){
         center.operator+=(hands[i].boundary[j]);
         }
         center.x = (int)(center.x / hands[i].boundary.size());
         center.y = (int)(center.y / hands[i].boundary.size());
         */
        
        
        // Calculate mean depth and area and palm center
        
        /*   // Get arm blob
         static BinaryImage armBlob;
         hands[i].CreateArmBlob(armBlob);
         
         // Get arm blob indices
         static Types<Point2Di>::FlatImage armBlobIndices;
         Util::Helpers::GetBlobIndices(armBlob, armBlobIndices);
         
         // Calculate mean depth
         float depth = 0;
         for(size_t j=0; j<armBlobIndices.size; j++)
         {
         Point2Di point = armBlobIndices.data[j];
         depth += depthImageF.data[point.y][point.x];
         }
         depth /= armBlobIndices.size;
         
         hands[i].meanDepth = depth;
         hands[i].area = armBlobIndices.size;*/
        
        
        // Find palm center
        hand = hands[i];
        hands[i].palmCenter = findPalmCenter(hand);
    }
    
    
    
}

void Hand::draw()
{
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofCircle(palmCenter, 20);
    
   // ofCircle(centroid, 10);
    
    ofLine(palmCenter, armBase);
    
    // cout << ofToString(armBase.x)<<" : "<<ofToString(armBase.y) << "\n";
    
    
    ofSetColor(0, 0, 255);
    //
    boundaryPoly.draw();
    
}


void ofxHandDetect::draw()
{
    float x = 0;
    float y = 0;
    float w = width;
    float h = height;
    
    outputImage.begin();
    for (int i = 0; i < hands.size(); i++)
    {
        hands[i].draw();
    }
    outputImage.end();
    
    
    ofSetColor(255, 255);
    outputImage.draw(x, y, w, h);
    
    
    
}
void ofxHandDetect::drawContours()
{
    output.draw(0, 0, width, height);
}
vector<Hand> ofxHandDetect::getHands()
{
    return hands;
}

vector<ofVec2f> ofxHandDetect::getHandPos()
{
    vector <ofVec2f> handPositions;
    for(int i=0; i < hands.size(); i++){
        ofVec2f point = hands[i].palmCenter;
        handPositions.push_back(point);
    }
    
    return handPositions;
}

vector<ofPolyline> ofxHandDetect::getContours(){
    vector<ofPolyline> handsPolys;
    for(int i = 0; i<hands.size();i++){
        handsPolys.push_back(hands[i].boundaryPoly);
    }
    return handsPolys;
}









// funktions adapted from KinectArm

struct CompareContourArea
{
    CompareContourArea(const std::vector<double>& areaVec)
    : mAreaVec(areaVec) {}
    
    // Sort contour indices into decreasing order, based on a vector of
    // contour areas.  Later, we will use these indices to order the
    // contours (which are stored in a separate vector).
    bool operator()(size_t a, size_t b) const
    {
        return mAreaVec[a] > mAreaVec[b];
    }
    
    const std::vector<double>& mAreaVec;
};

void ofxHandDetect::getHandBoundries(cv::Mat _armBlobs){
    // cv::Mat armBlobsMat(DEPTH_RES_Y, DEPTH_RES_X, CV_8UC1, (void*)armBlobs.data);
    
    // adapted form ofxCv
    
    vector<vector<cv::Point> > allContours;
    bool simplify = false;
    int simplifyMode = simplify ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE;
    cv::findContours(_armBlobs, allContours, CV_RETR_EXTERNAL, simplifyMode);
    
    int imgMinArea = 1000;
    
    // filter the contours
    vector<size_t> allIndices;
    vector<double> allAreas;
    double imgArea = _armBlobs.rows * _armBlobs.cols;
    for(size_t i = 0; i < allContours.size(); i++) {
        double curArea = contourArea(Mat(allContours[i]));
        allAreas.push_back(curArea);
        if(curArea >= imgMinArea) {
            allIndices.push_back(i);
        }
    }
    
    // sort by size
    if (allIndices.size() > 1) {
        std::sort(allIndices.begin(), allIndices.end(), CompareContourArea(allAreas));
    }
    
    // generate polylines and bounding boxes from the contours
    contoursLocal.clear();
    hands.clear();
    hands.resize(allIndices.size());
    // boundingRects.clear();
    for(size_t i = 0; i < allIndices.size(); i++) {
        contoursLocal.push_back(allContours[allIndices[i]]);
        hands[i].boundaryPoly = ofxCv::toOf(contoursLocal[i]);
        //  boundingRects.push_back(boundingRect(contours[i]));
        
        vector<cv::Point> tmp;
        for(size_t i2 = 0; i2 < contoursLocal[i].size(); i2++) {
            hands[i].boundary.push_back(ofxCv::toOf(contoursLocal[i][i2]));
            tmp.push_back(contoursLocal[i][i2]);
        }
        
        std::vector<std::vector<cv::Point> > contourVec;
        contourVec.push_back(contoursLocal[i]);
        
        Mat drawedContour = cv::Mat::zeros( cvSize(width,height), CV_8U );
        cv::drawContours(drawedContour,contourVec,0,Scalar(255,255,255),CV_FILLED, 0);
        

        hands[i].armBlob = drawedContour;
        
        
        
        
        ofVec2f center;
        cv::Moments m = moments(contoursLocal[i]);
        center = ofxCv::toOf(cv::Point2f(m.m10 / m.m00, m.m01 / m.m00));
        hands[i].centroid = center;
    }
    
    
    


/*
 
 // from kinectArms
 
 const int smallBoundarySize = 40;
 vector<CvSeq*> contours;
 for(CvSeq* contour = firstContour; contour!= 0; contour = contour->h_next)
 {
 // Remove any small boundaries
 if(contour->total < smallBoundarySize)
 continue;
 
 contours.push_back(contour);
 }
 
 
 
 // Get the contours of the blobs
 static CvSeq* firstContour = 0;
 
 Mat armBlobsDilated = _armBlobs;
 
 dilate(armBlobsDilated, armBlobsDilated, cv::Mat(), cv::Point(-1,-1), 1);
 
 CvMat armsBlobCvMat = armBlobsDilated;//_armBlobs;
 static CvMemStorage* storage = cvCreateMemStorage(0);
 
 
 int numContours = cvFindContours(&armsBlobCvMat, storage, &firstContour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
 
 
 
 // Put contours in a nice list and get rid of all small contours
 const int smallBoundarySize = 40;
 vector<CvSeq*> contours;
 for(CvSeq* contour = firstContour; contour!= 0; contour = contour->h_next)
 {
 // Remove any small boundaries
 if(contour->total < smallBoundarySize)
 continue;
 
 contours.push_back(contour);
 }
 
 
 // Clear all previous hands
 hands.clear();
 
 
 // Get the hand boundaries (take as many as we are allowed)
 hands.resize(contours.size());
 for(int i=0; i<hands.size(); i++)
 {
 CvSeq& contour = *contours[i];
 hands[i].boundrySeq = &contour;
 //  ((HandPimpl*)hands[i].handPimpl)->SetBoundary(contour);
 
 CvSeqReader reader;
 cvStartReadSeq(&contour, &reader, 0);
 
 for(int j=0; j<contour.total; j++)
 {
 CvPoint point;
 memcpy(&point, reader.ptr, contour.elem_size);
 
 hands[i].boundary.push_back(ofVec2f(point.x, point.y));
 
 CV_NEXT_SEQ_ELEM(contour.elem_size, reader);
 }
 
 
 
 //   cvDrawContours(&blobImage, const_cast<CvSeq*>(((HandPimpl*)handPimpl)->GetBoundary()), color, color, 0, thickness);
 
 IplImage armBlobImage = hands[i].armBlob;
 
 cvDrawContours( &armBlobImage, &contour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, -1);
 //hands[i].armBlob = Mat(armBlobImage, true);
 
 //   cv::drawContours(hands[i].armBlob, contours, i, CV_RGB(255, 255, 255));
 
 
 }
 */
}

ofVec2f ofxHandDetect::findArmBase(cv::Mat tableEdges, Hand &_hand)
{
    
    ofVec2f armBase;
    
    
    
    /* Mat image = Mat::zeros( cvSize(width,height),  CV_8U );
     tableEdges.convertTo(image, CV_8U, 5);
     IplImage* img = new IplImage(image);
     cvCopy(img, output.getCvImage());
     output.flagImageChanged();*/
    
    
    threshold(tableEdges, tableEdges, 1, 1, THRESH_TRUNC);
    
    
    
    
    
    // Find a boundary point that isn't on the table edge
    int startIndex = -1;
    /*for(int iy=0; iy<tableEdges.rows; iy++){
     for(int ix=0; ix<tableEdges.cols; ix++){
     cout << tableEdges.at<bool>(iy, ix);
     }
     cout << "\n";
     }*/
    
    bool flag = false;
    
    for(int j=0; j<_hand.boundary.size(); j++)
    {
        
        // cout << "R (default) = " << endl <<        tableEdges          << endl << endl;
        // cout << ofToString(tableEdges.at<bool>((int)_hand.boundary[j].y, (int)_hand.boundary[j].x))<<"\n";
        if(tableEdges.at<bool>((int)_hand.boundary[j].y,(int)_hand.boundary[j].x)){
            startIndex = j;
            break;
        }
    }
    
    if(startIndex == -1)
    {
        armBase = ofVec2f(-1, -1);
        return armBase;
    }
    
    
    // Make variables to hold properties of the arm base
    int maxCount = 0;
    int maxStartIndex = 0;
    
    // Go around boundary looking for the largest segment
    
    
    
    // Find base start
    int baseStartIndex = 0;
    int baseEndIndex = 0;
    bool flagCross = false;
    
    int count = 0;
    for(int j=0; j<_hand.boundary.size(); j++)
    {
        ofVec2f point = _hand.boundary[(j + startIndex) % _hand.boundary.size()];
        ofVec2f fwrdPoint = _hand.boundary[(j + startIndex + 1) % _hand.boundary.size()];
        
        bool startingBase =  tableEdges.at<bool>(point.y, point.x) && !tableEdges.at<bool>(fwrdPoint.y, fwrdPoint.x);
        bool endingBase =    !tableEdges.at<bool>(point.y, point.x) && tableEdges.at<bool>(fwrdPoint.y, fwrdPoint.x);
        
        if(!tableEdges.at<bool>(point.y, point.x)){
            count++;
            
        }
        
        
        
        
        if(startingBase)
        {
            // We are starting a base
            baseStartIndex = (j + startIndex + 1) % _hand.boundary.size();
            count = 0;
        }
        else if(endingBase)
        {
            if(count > maxCount)
            {
                maxCount = count;
                maxStartIndex = baseStartIndex;
            }
        }
        
    }
    
    
    // Check if we found any base
    if(maxCount == 0)
    {
        armBase = ofVec2f(-1, -1);
        return armBase;
    }
    
    
    
    
    // Calculate the center of the base
    int armBaseIndex = ((maxStartIndex + (maxStartIndex + maxCount)) / 2) % _hand.boundary.size();
    
    armBase = _hand.boundary[armBaseIndex];
    
    _hand.indxBaseStart = maxStartIndex;
    _hand.indxBaseEnd = maxStartIndex+maxCount;
    
    outputImage.begin();
    ofSetColor(255, 0, 255);
    ofCircle(armBase, 10);
    outputImage.end();
    
    
    return armBase;
}

ofVec2f ofxHandDetect::findPalmCenter(Hand _hand)
{
    ofVec2f palmCenter;
    
    Mat distImage = Mat::zeros( cvSize(width,height),  CV_32FC1 );
    
  //  threshold(_hand.armBlob, _hand.armBlob, 1, 255, THRESH_TRUNC);
    cv::distanceTransform(_hand.armBlob, distImage, CV_DIST_C, 3);  //!!Could use CV_DIST_L2 for euclidian distance
    
    
    /*Mat image = Mat::zeros( cvSize(width,height),  CV_8UC1 );
    distImage.convertTo(image, CV_8UC1, 5);
    IplImage* img = new IplImage(image);
    cvCopy(img, output.getCvImage());
    output.flagImageChanged();
   */
    // Only check points that are farther from the arm base than the geometric center
    float armCenterMagnitude = _hand.centroid.distanceSquared(_hand.armBase);
    
    // Get arm blob indices
    vector<ofVec2f> indices;
    for(int y=0; y<_hand.armBlob.rows; y++)
    {
        for(int x=0; x<_hand.armBlob.cols; x++)
        {
            if(_hand.armBlob.at<bool>(y, x))
            {
                indices.push_back(ofVec2f(x, y));
                //indices.data[indices.size] = Point2Di(x, y);
                //indices.size++;
            }
        }
    }
    
    
    
    // Find the max value of the distance transform that is further from arm base than geometric center is
    ofVec2f maxValuePoint;
    float maxValue = 0;
    
    for(int i=0; i<indices.size(); i++)
    {
        ofVec2f point = indices[i];
        float pointMagnitude = point.distanceSquared(_hand.armBase);
        
        float value = distImage.at<float>(point.y, point.x) * pointMagnitude;
      //  cout << ofToString(pointMagnitude > armCenterMagnitude)+"\n";
        if(pointMagnitude > armCenterMagnitude && value > maxValue)  // pointMagnitude > armCenterMagnitude always true;
        {
            maxValuePoint = point;
            maxValue = value;
            
        }
        
    }

    
    
    float palmThreshold = 0;
    if(maxValue > palmThreshold){
        palmCenter = maxValuePoint;
    }else{
        palmCenter = ofVec2f(-1, -1);
    }
    return palmCenter;
}

void ofxHandDetect::createMask(){
    inputImage.copyTo(areaMask);
    threshold(areaMask, areaMask, 150, 255, THRESH_BINARY);
    
    ofImage imgToSave;
    ofxCv::toOf(areaMask, imgToSave);
    imgToSave.saveImage("areaMask.png");
}
void ofxHandDetect::resetMask(){
    areaMask = cv::Mat::zeros( cvSize(width,height), CV_8U );
    cv::Point p1 = cv::Point(0,0);//ofxCv::toCv(ofVec2f((width-height)/2, 0));
    cv::Point p2 = cv::Point(width, height);//ofxCv::toCv(ofVec2f((width-height)/2+height, height));
    rectangle(areaMask, p1, p2, CV_RGB(255,255,255),-1,CV_AA,0);//
}
void ofxHandDetect::createGenericMask(){
    areaMask = cv::Mat::zeros( cvSize(width,height), CV_8U );
    // circle(areaMask,ofxCv::toCv(ofVec2f(width/2, height/2)),height/4,CV_RGB(255,255,255),-1,CV_AA,0);
    int red = 40;
    cv::Point p1 = ofxCv::toCv(ofVec2f((width-height)/2 + red, red));
    cv::Point p2 = ofxCv::toCv(ofVec2f((width-height)/2+height-red, height-red));
    rectangle(areaMask, p1, p2, CV_RGB(255,255,255),-1,CV_AA,0);//
}

