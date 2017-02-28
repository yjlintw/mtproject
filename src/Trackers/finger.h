#ifndef _YJ_FINGER_H
#define _YJ_FINGER_H
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "json/json.h"

namespace YJ 
{
    class Finger: public cv::Point2f {
        public:
        Finger() : cv::Point2f { 0, 0 }, id { -1 }, action { 0 }
        { }
        Finger(float in_x, float in_y, int in_id, int in_action = 0)
        : cv::Point2f { in_x, in_y }, id { in_id }, action { in_action }
        { }
        Finger(cv::Point2f pt, int in_id, int in_action = 0) : cv::Point2f { pt }, id { in_id }, action { in_action }
        { }
        
        Finger(Json::Value jo) {
            id = jo["id"].asInt();
            x = jo["x"].asFloat();
            y = jo["y"].asFloat();
            action = jo["action"].asInt();
        }

        int getId() {
            return id;
        }
        
        int getAction() {
            return action;
        }
        
        int setAction(int act) {
            action = act;
        }
        
        Json::Value toJSON() {
            Json::Value result;
            result["id"] = id;
            result["x"] = x;
            result["y"] = y;
            result["action"] = action;
            return result;
        }
        
        private:
        int id;
        int action; // 0: present, 1: Add, 2: Move, 3: Remove

    };
}
#endif
