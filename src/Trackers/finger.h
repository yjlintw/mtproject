#ifndef _YJ_FINGER_H
#define _YJ_FINGER_H
#include "ofxOpenCv.h"
#include "ofxCv.h"

namespace YJ 
{
    class Finger: public cv::Point2f {
        public:
        Finger(float in_x, float in_y, int in_id) 
        : cv::Point2f { in_x, in_y }, id { in_id }
        { }
        Finger(cv::Point2f pt, int in_id) : cv::Point2f { pt }, id { in_id }
        { }    

        int getId() {
            return id;
        }
        
        private:
        int id;

    };
}
#endif
