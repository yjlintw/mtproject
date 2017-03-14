#include "ofApp.h"
#include <set>


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

    // Camera Calibration
    // TODO:: Currently the calibration points is fixed size;
    calibrationPoints[1] = cv::Point2f(0, 0);
    calibrationPoints[0] = cv::Point2f(ofGetWidth(), 0);
    calibrationPoints[3] = cv::Point2f(ofGetWidth(), ofGetHeight());
    calibrationPoints[2] = cv::Point2f(0, ofGetHeight());

    cameraPoints[0] = cv::Point2f(0, 0);
    cameraPoints[1] = cv::Point2f(640, 0);
    cameraPoints[2] = cv::Point2f(640, 480);
    cameraPoints[3] = cv::Point2f(0, 480);
    for (int i = 0; i < 4; i++) {
        pointsSet[i] = false;
    }

    warpMat = cv::Mat(2, 3, CV_32FC1);
    debugPoint = cv::Point2f(-1, -1);
    

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
    
    //
    fingerDirty = false;
    markerDirty = false;
    
    // Syphon
//    mainOutputSyphonServer.setName("Screen Output");
//    
//    mClient.setup();
//    mClient.set("", "Simple Server");
    
    // UDP setup
    udpPort = 11999;
    networkSetup(udpPort);
    
}

//--------------------------------------------------------------

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

void ofApp::networkSetup(int port) {
    udpConnection.Create();
    udpConnection.Connect("127.0.0.1", port);
    udpConnection.SetNonBlocking(true);
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
    updateBlobTracker();
    updateFidMarker();
    sendMsg();
    

}

void ofApp::sendMsg() {
    Json::Value root;
    if (fingerDirty) {
        for (auto item : fingerPoints) {
            root["fingers"][std::to_string(item.first)] = item.second.toJSON();
        }
        fingerDirty = false;
    }
    
    if (markerDirty) {
        for (auto item: markers) {
            root["markers"][std::to_string(item.first)] = item.second.toJSON();
        }
        markerDirty = false;
    }
    
    if (!root.isNull()) {
#ifdef DEBUG
        std::cout << root.toStyledString() << std::endl;
#endif
        Json::FastWriter writer;
        std::string output = writer.write(root);
        udpConnection.Send( output.c_str(), output.length() );
    }
    
    // clear dead points
    for (auto it = fingerPoints.begin(); it != fingerPoints.end(); ) {
        if (it->second.getAction() == 3) {
            it = fingerPoints.erase(it);
        } else {
            it++;
        }
    }
    for (auto it = fingerPoints_cv.begin(); it != fingerPoints_cv.end(); ) {
        if (it->second.getAction() == 3) {
            it = fingerPoints_cv.erase(it);
        } else {
            it++;
        }
    }
    
    for (auto it = markers.begin(); it != markers.end(); ) {
        if (it->second.getAction() == 3) {
            it = markers.erase(it);
        } else {
            it++;
        }
    }
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
        drawCameraViews();
        drawMarkers();
        drawFingers();
        //draw the fingers
        for (list<ofxFinger>::iterator finger = fidfinder.fingersList.begin(); finger != fidfinder.fingersList.end(); finger++) {
            finger->draw(20, 20);
        }
    }

    drawCalibrationPoints();
    
    // Syphon
//    mClient.draw(50, 50);
//    mainOutputSyphonServer.publishScreen();
}

//--------------------------------------------------------------


// Draw Methods
void ofApp::drawCameraViews() {
    irImage.draw(0, 0, 640, 480);
    if (matReady) {
        avgImage.draw(0, 480, 400, 300);
        diffImage.draw(640, 0, 400, 300);
        fingerImage.draw(640, 300, 400, 300);
        fidImage.draw(640, 600, 400, 300);
    }
}

void ofApp::drawMarkers() {
    if (matReady) {
        for (auto marker : markers) {
            ofPolyline line;
            line.addVertex(marker.second.corners[0].x, marker.second.corners[0].y);
            line.addVertex(marker.second.corners[1].x, marker.second.corners[1].y);
            line.addVertex(marker.second.corners[2].x, marker.second.corners[2].y);
            line.addVertex(marker.second.corners[3].x, marker.second.corners[3].y);
            line.close();
            ofSetColor(255, 255, 0);
            line.draw();
            ofDrawCircle(marker.second.center.x, marker.second.center.y, 100);
        }
    }
}

void ofApp::drawFingers() {
    if (matReady) {
        // draw block tracker
        blobTracker.draw(0,0, 640, 480);
        
        // draw fingers on camera view;
        for (auto finger_cv : fingerPoints_cv) {
            ofFill();
            ofSetColor(255, 0, 0);
            ofDrawCircle(finger_cv.second.x, finger_cv.second.y, 10);
            
            ofSetColor(0);
            ofDrawBitmapString(ofToString(finger_cv.second.getId()), finger_cv.second.x, finger_cv.second.y);
            
        }

        // From Camera View to Projector View
        if (!fingerPoints.empty() && calibrationDone) {
            ofSetColor(255, 255, 0);
            for (auto finger : fingerPoints) {
                ofDrawCircle(finger.second.x, finger.second.y, 10);
                ofDrawBitmapString(ofToString(finger.second.getId()), finger.second.x, finger.second.y);
            }
        }
    }
}

void ofApp::drawCalibrationPoints() {
    // Calibration debug info
#ifdef DEBUG
    if(calibrationDone) {
        cv::Point2f transformedPt = cv::Point2f(0, 0);
        std::vector<cv::Point2f> vec {debugPoint};
        cv::transform(vec, vec, warpMat);
        ofSetColor(255, 255, 0);
        ofDrawCircle(vec[0].x, vec[0].y, 5);
    }
    
    
    if (debugPoint.x >= 0 && debugPoint.y >= 0) {
        ofSetColor(255, 0, 255);
        ofDrawCircle(debugPoint.x, debugPoint.y, 5);
    }
#endif
    
    // Corner Points : Reference Points (Fixed)
    for (int i = 0; i < 4; i++) {
        ofSetColor(200, 200, 200);
        cv::Point2f&& p = static_cast<cv::Point2f&&>(calibrationPoints[i]);
        ofDrawCircle(p.x, p.y, 10);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString("p" + std::to_string(i), p.x, p.y);
    }
    
    // Corner Points : Camera Points (Adjustable)
    for (int i = 0; i < 4; i++) {
        if (pointsSet[i]) {
            ofSetColor(255, 200, 200);
        } else {
            ofSetColor(100, 100, 255);
        }
        cv::Point2f&& p = static_cast<cv::Point2f&&>(cameraPoints[i]);
        ofDrawCircle(p.x, p.y, 10);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString("c" + std::to_string(i), p.x, p.y);
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
        case 'c':
            setCalibrationPoint();
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
    switch(button) {
        case 0:

        if (calibrationMode && currentCalibrationPt < 4) {
            cameraPoints[currentCalibrationPt].x = x;
            cameraPoints[currentCalibrationPt].y = y;
            pointsSet[currentCalibrationPt] = true;
            currentCalibrationPt++;
        } else {
            debugPoint.x = x;
            debugPoint.y = y;
        }

        break;
        default:
        break;
    }
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
    } else if (calibrationMode) {
        // calibration mode
        // Get Four Points;
        if (currentCalibrationPt < 4) {

        } else {
            warpMat = cv::getAffineTransform( cameraPoints, calibrationPoints );
            calibrationMode = false;
            calibrationDone = true;
        }
        // Get Transformation Matrix
        
        
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


void ofApp::updateBlobTracker() {
    if (matReady) {
    }    
}


void ofApp::updateFidMarker() {
    // get fiducial info
    if (calibrationDone) {
        std::set<int> currentIds {};
        for (list<ofxFiducial>::iterator fiducial = fidfinder.fiducialsList.begin(); fiducial != fidfinder.fiducialsList.end(); fiducial++) {
            fiducial->computeCorners();
            
            std::vector<cv::Point2f> corners;
            corners.push_back(cv::Point2f(fiducial->cornerPoints[0].x, fiducial->cornerPoints[0].y));
            corners.push_back(cv::Point2f(fiducial->cornerPoints[1].x, fiducial->cornerPoints[1].y));
            corners.push_back(cv::Point2f(fiducial->cornerPoints[2].x, fiducial->cornerPoints[2].y));
            corners.push_back(cv::Point2f(fiducial->cornerPoints[3].x, fiducial->cornerPoints[3].y));
            corners.push_back(cv::Point2f(fiducial->getX(), fiducial->getY()));
            
            
            cv::transform(corners, corners, warpMat);
            
            cv::Point2f t_center = corners.back();
            corners.pop_back();
            YJ::Marker newMarker {corners, t_center, fiducial->getAngleDeg(), fiducial->fidId};
#ifdef DEBUG
//            std::cout << newMarker.toJSON().toStyledString() << std::endl;
#endif      
            currentIds.insert(fiducial->fidId);
            // check Add or moved
            
            if ( markers.count(fiducial->fidId) > 0 ) {
                // Moved
                newMarker.setAction(2);
                markerMoved( newMarker );
            } else {
                newMarker.setAction(1);
                markerAdded( newMarker );
            }
            
            
        }
        
        // Check if an marker is removed
        for (auto marker : markers) {
            if( currentIds.count(marker.first) == 0 ) {
                markerDeleted(marker.second);
            }
        }
        
    }
}

void ofApp::markerAdded(YJ::Marker& marker) {
    markerDirty = true;
    markers[marker.getId()] = marker;
    
}
void ofApp::markerMoved(YJ::Marker& marker) {
    
    YJ::Marker oldMarker = markers[marker.getId()];
    float&& dx = oldMarker.center.x - marker.center.x;
    float&& dy = oldMarker.center.y - marker.center.y;
    float dangle = abs(oldMarker.angle - marker.angle);
    float dist = dx * dx + dy * dy;
    if (dist > 10  || dangle > 1) {
        markerDirty = true;
        markers[marker.getId()] = marker;
        
    }
}
void ofApp::markerDeleted(YJ::Marker& marker) {
    markers[marker.getId()].setAction(3);
    markerDirty = true;
}


cv::Point2f ofApp::transformPoint(cv::Point2f pt) {
    
    std::vector<cv::Point2f> transPts { pt };
    cv::transform(transPts, transPts, warpMat);
    return transPts[0];
}

// Blob Tracker
void ofApp::blobAdded(ofxBlob &_blob) {
#ifdef DEBUG
    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " added" );
#endif
    
    if (matReady) {
        // Noisy points
        if (_blob.boundingRect.width > fingerSizeThreshold
            || _blob.boundingRect.height > fingerSizeThreshold) {
            // Do nothing
            return;
        }
        
        if (_blob.boundingRect.width < fingerSizeLowerThreshold
            || _blob.boundingRect.height < fingerSizeLowerThreshold) {
            return;
        }
        
        cv::Point2f pt { _blob.centroid.x * 640.0f, _blob.centroid.y * 480.0f };
        
        cv::Point2f tPt = transformPoint(pt);
        
        fingerPoints[_blob.id] = YJ::Finger (tPt.x, tPt.y, _blob.id, 1 );
        fingerPoints_cv[_blob.id] = YJ::Finger { pt.x, pt.y, _blob.id, 1 };
        fingerDirty = true;
        
    }
    
}

void ofApp::blobMoved(ofxBlob &_blob) {
#ifdef DEBUG
//    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " moved" );
#endif
    
    if (matReady) {
        // Noisy points
        if (_blob.boundingRect.width > fingerSizeThreshold
            || _blob.boundingRect.height > fingerSizeThreshold) {
            // Do nothing
            return;
        }
        
        if (_blob.boundingRect.width < fingerSizeLowerThreshold
            || _blob.boundingRect.height < fingerSizeLowerThreshold) {
            return;
        }
        
        cv::Point2f pt { _blob.centroid.x * 640.0f, _blob.centroid.y * 480.0f };
        
        cv::Point2f tPt = transformPoint(pt);
        
        fingerPoints[_blob.id] = YJ::Finger { tPt.x, tPt.y, _blob.id, 2 };
        fingerPoints_cv[_blob.id] = YJ::Finger { pt.x, pt.y, _blob.id, 2 };
        fingerDirty = true;
        
    }
}

void ofApp::blobDeleted(ofxBlob &_blob) {
#ifdef DEBUG
//    ofLog(OF_LOG_NOTICE, "Blob ID " + ofToString(_blob.id) + " deleted" );
#endif
    
    if (matReady) {
        if (fingerPoints.count(_blob.id) == 1) {
            fingerPoints.at(_blob.id).setAction(3);
            fingerPoints_cv.at(_blob.id).setAction(3);
            fingerDirty = true;
        }
    }
}


// calibration
void ofApp::setCalibrationPoint() {
    calibrationMode = true;
    currentCalibrationPt = 0;
    for (int i = 0; i < 4; i++) {
        pointsSet[i] = false;
    }

}
