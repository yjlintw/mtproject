#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxBlobTracker.h"
#include "ofxFidMain.h"


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    private:
        bool readyToDraw;
        bool recordBackground;
        bool matReady;
        ofxKinect kinect;
        ofImage irImage;
        ofImage avgImage;
        ofImage diffImage;
        ofImage fingerImage;
        ofImage fidImage;

        // Blob Tracking
        ofImage thresholdImage;
        int imageThreshold;
        int blobThreshold;
        ofxBlobTracker blobTracker;
        float fingerSizeThreshold;
        float fingerSizeLowerThreshold;

        // Fiducial Tracking
        ofxFiducialTracker fidfinder;
        
        //Blob Tracker
        void blobAdded(ofxBlob &_blob);
        void blobMoved(ofxBlob &_blob);
        void blobDeleted(ofxBlob &_blob);

        void KinectSetup();
        void ImageProcessing();
        void UpdateBackground(const cv::Mat& input, cv::Mat &avg);
        
};
