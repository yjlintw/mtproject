#include "ofApp.h"

//----------- TODO --------------
// 1. Camera Calibration
// 2. Projection Calibration / Mapping
// 3. Communication between OF and Unity
//      a. Design Protocol
//      b. Implement
// 4. Make settings as a text file (Save / Load)
// 

//--------------------------------------------------------------
void ofApp::setup(){
    // OpenFrameworks setup
    ofSetFrameRate(60);
    KinectSetup();
    //
    readyToDraw = false;
    recordBackground = false;
    matReady = false;

    // Initialize
    irImage.allocate(640,480, ofImageType::OF_IMAGE_GRAYSCALE);
    avgImage.allocate(640,480, ofImageType::OF_IMAGE_GRAYSCALE);
    diffImage.allocate(640,480, ofImageType::OF_IMAGE_GRAYSCALE);
    fingerImage.allocate(640,480, ofImageType::OF_IMAGE_GRAYSCALE);
    fidImage.allocate(640,480, ofImageType::OF_IMAGE_GRAYSCALE); 
    
    
    

    blobThreshold = 30;
    imageThreshold = 10;
    fingerSizeThreshold = 0.065;
    fingerSizeLowerThreshold = 0.015;

    ofAddListener(blobTracker.blobAdded, this, &ofApp::blobAdded);
    ofAddListener(blobTracker.blobMoved, this, &ofApp::blobMoved);
    ofAddListener(blobTracker.blobDeleted, this, &ofApp::blobDeleted);

    // Fiducial
    fidfinder.detectFinger = false;
    fidfinder.maxFingerSize = 100;
    fidfinder.minFingerSize = 1;
    fidfinder.fingerSensitivity = 0.05f;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    kinect.update();

    // a new frame
    if (kinect.isFrameNew()) {
        irImage.setFromPixels(kinect.getPixels());
        if (!readyToDraw) {
            avgImage.clone(irImage);
            diffImage.clone(irImage);
            readyToDraw = true;
        }
    }

    ImageProcessing();

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255,255,255);
    ofSetLineWidth(1);
    int offset = 60;
    // debug info
    string info = "fps: " + ofToString(ofGetFrameRate(), 2);
    ::ofDrawBitmapString(info, 10, ofGetHeight() - offset); 
    offset -= 15;
    ofDrawBitmapString("blobThreshold: " + ofToString(blobThreshold), 10, ofGetHeight() - offset);
    offset -= 15;
    ofDrawBitmapString("imageThreshold: " + ofToString(imageThreshold), 10, ofGetHeight() - offset);
    offset -= 15;
    ofDrawBitmapString("fingerSizeThreshold: " + ofToString(fingerSizeLowerThreshold) + ":" + ofToString(fingerSizeThreshold), 10, ofGetHeight() - offset);
    offset -= 15;
    if (recordBackground) {
        ::ofSetColor(255, 0, 0);
        ::ofDrawBitmapString("Recording Background", 10, ofGetHeight() - offset);
        ::ofSetColor(255, 255, 255);
    }

    // kinect view
    // kinect.draw(0,0,800,600);
    if (readyToDraw) {
        irImage.draw(0, 0, 640, 480);
        if (matReady) {
            avgImage.draw(0, 480, 400, 300);
            diffImage.draw(640, 0, 400, 300);
            fingerImage.draw(640, 300, 400, 300);
            fidImage.draw(640, 600, 400, 300);
            // draw block tracker
            blobTracker.draw(0,0, 640, 480);
            for (int i = 0; i < blobTracker.size(); i++) {
                // if (!blobTracker[i].isCircular()) {
                //     continue;
                // }
                float rectRatio = blobTracker[i].boundingRect.width / blobTracker[i].boundingRect.height;
                std::cout << rectRatio << std::endl;
                // if (rectRatio > 1.5 || rectRatio < 0.5) {
                //     continue;
                // } 
                if (blobTracker[i].boundingRect.width > fingerSizeThreshold || blobTracker[i].boundingRect.height > fingerSizeThreshold) {
                    continue;
                }

                if (blobTracker[i].boundingRect.width < fingerSizeLowerThreshold || blobTracker[i].boundingRect.height < fingerSizeLowerThreshold) {
                    continue;
                }
                ofFill();
                ofSetColor(255, 0, 0);
                ofCircle(blobTracker[i].centroid.x * 640,
                 blobTracker[i].centroid.y * 480,
                 10);
                
                ofSetColor(0);
                ofDrawBitmapString(ofToString( blobTracker[i].id ),
                           blobTracker[i].centroid.x * 640,
                           blobTracker[i].centroid.y * 480);
            }
            
            // get fiducial info
            for (list<ofxFiducial>::iterator fiducial = fidfinder.fiducialsList.begin(); fiducial != fidfinder.fiducialsList.end(); fiducial++) {
                fiducial->draw(0, 0);
                fiducial->drawCorners(0,0);
                ofSetColor(0, 255, 0);
                ofDrawBitmapString("a: " + ofToString(fiducial->getAngleDeg(),2), fiducial->getX(), fiducial->getY() + 8);
                ofDrawBitmapString("s: " + ofToString(fiducial->getRootSize(),2), fiducial->getX(), fiducial->getY() + 16);
                ofSetColor(0, 0, 255);
                if (fiducial->isPointInside(mouseX, mouseY)) {
                    ofFill();
                } else {
                    ofNoFill();
                }
                ofCircle(mouseX, mouseY, 10);
                ofSetColor(255,255,255);

            }
            
            // get finger
            for (list<ofxFinger>::iterator finger = fidfinder.fingersList.begin(); finger != fidfinder.fingersList.end(); finger++) {
                finger->draw(0,0);
            }
        }
    }

}

//--------------------------------------------------------------

void ofApp::exit() {
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'r':
            recordBackground = !recordBackground;
            break;
        case '+':
            blobThreshold++;
            if (blobThreshold > 255) blobThreshold = 255;
            break;
        case '-':
            blobThreshold--;
            if (blobThreshold < 0) blobThreshold = 0;
            break;
        case '[':
            imageThreshold--;
            if (imageThreshold < 0) imageThreshold = 0;
            break;
        case ']':
            imageThreshold++;
            if (imageThreshold > 255) imageThreshold = 255;
            break;
        case ',':
            fingerSizeThreshold-=0.001;
            if (fingerSizeThreshold < 0) fingerSizeThreshold = 0;
            break;
        case '.':
            fingerSizeThreshold+=0.001;
            if (fingerSizeThreshold > 1) fingerSizeThreshold = 1;
            break;
        case 'k':
            fingerSizeLowerThreshold-=0.001;
            if (fingerSizeLowerThreshold < 0) fingerSizeLowerThreshold = 0;
            break;
        case 'l':
            fingerSizeLowerThreshold+=0.001;
            if (fingerSizeLowerThreshold > 1) fingerSizeLowerThreshold = 1;
            break;
;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::KinectSetup() {
    kinect.setRegistration(false);
    kinect.setLed(ofxKinect::LED_OFF);

    kinect.init(true);
    kinect.open();

    // print the intrinsic IR sensor value
    if (kinect.isConnected()) {
        std::cout << "Kinect is connected" << std::endl;
    }
}

void ofApp::UpdateBackground(const cv::Mat& input, cv::Mat& avg) {
    avg = avg * 0.8 + input * 0.2;
}

void ofApp::ImageProcessing() {
    cv::Mat irMat = ofxCv::toCv(irImage);
    cv::Mat avgMat = ofxCv::toCv(avgImage);
    cv::Mat diffMat = ofxCv::toCv(diffImage);
    cv::Mat fingerMat = ofxCv::toCv(fingerImage);
    cv::Mat fidMat = ofxCv::toCv(fidImage);

    if (recordBackground) {
        UpdateBackground(irMat, avgMat);
        matReady = true;
    } else {
        if (matReady) {
            ofxCv::absdiff(avgMat, irMat, diffMat);
            cv::Mat blurMat;
            cv::GaussianBlur(diffMat, blurMat, cv::Size(33,33),0, 0);
            cv::absdiff(diffMat, blurMat, blurMat);
            cv::GaussianBlur(blurMat, fingerMat, cv::Size(7, 7), 0, 0);
            cv::threshold(fingerMat, fingerMat, imageThreshold, 255, cv::THRESH_BINARY);
            cv::threshold(diffMat, fidMat, blobThreshold, 255, cv::THRESH_BINARY);
            //
            fidImage.update();
            fingerImage.update();
            ofxCvGrayscaleImage tmpImg;
            tmpImg.allocate(640,480);
            tmpImg.setFromPixels(fingerImage.getPixels());
            blobTracker.update(tmpImg, blobThreshold);
            tmpImg.setFromPixels(fidImage.getPixels());
            fidfinder.findFiducials(tmpImg);  

        }
    }

    irImage.update();
    avgImage.update();
    diffImage.update();
    fingerImage.update();
    fidImage.update();

    irMat.release();
    avgMat.release();
    diffMat.release();
    fingerMat.release();
    fidMat.release();
}


// Blob Tracker
void ofApp::blobAdded(ofxBlob &_blob){
    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " added" );
}

void ofApp::blobMoved(ofxBlob &_blob){
    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " moved" );
}

void ofApp::blobDeleted(ofxBlob &_blob){
    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " deleted" );
}
