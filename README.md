# HandTracker_KinectV1
Hand Tracking with a Kinect V1

Introduction
------------
Code for Tracking palms using a Kinect. It is inspired and based on the work of KinectArms(http://hci.usask.ca/publications/view.php?id=286) and the work of Wei-chao Chen (陳威詔)’s master thesis “Real-Time Palm Tracking and Hand Gesture Estimation Based on Fore-Arm Contour” (http://pc01.lib.ntust.edu.tw/ETD-db/ETD-search/view_etd?URN=etd-0718111-154755)
Additionally the code also contanins the calculation of movement qualities and the gathered data is projected into realworld coordinates to be used with projection mapping.
However, if the Tracking is of any usage, feel free to use the code.

Licence
-------
The code in this repository is available under the MIT License.

Installation
------------
- it was built using openFrameworks 0.8.4
needed addons:
- ofxKinect
- ofxOpenCv
- ofxXmlSettings
- ofxCv
- ofxKinectProjectorToolkit
- ofxUI
- ofxOsc
- ofxBiquadFilter

most of them are though used for further processing of the data, for simple tracking just the ofxKinect, ofxOpenCV and ofxCv is used. 


Dependencies
------------


Compatibility
------------


Known issues
------------
none

Version history
------------

### v0.1
- initial version
