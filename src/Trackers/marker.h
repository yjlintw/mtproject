#ifndef _YJ_MARKER_H
#define _YJ_MARKER_H
#include <vector>
#include "ofxOpenCv.h"
#include "ofxCv.h"

namespace YJ {
    class Marker {
    public:
        Marker();
        Marker(std::vector<cv::Point2f> in_cs, cv::Point2f in_center, double in_ang, int in_id) : corners { in_cs },
            center { in_center }, angle { in_ang }, id { in_id }
        { }
        std::vector<cv::Point2f> corners;
        cv::Point2f center;
        double angle;
        
        int getId() {
            return id;
        }
        
    private:
        int id;
    };
}

#endif
