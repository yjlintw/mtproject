#ifndef _YJ_MARKER_H
#define _YJ_MARKER_H
#include <vector>
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "json/json.h"

namespace YJ {
    class Marker {
    public:
        Marker() : corners { std::vector<cv::Point2f>{} }, center { cv::Point2f{0, 0} }, angle { 0 }, id { -1 }, action { 0 }
        {}
        Marker(std::vector<cv::Point2f> in_cs, cv::Point2f in_center, double in_ang, int in_id, int in_action = 0) : corners { in_cs },
        center { in_center }, angle { in_ang }, id { in_id }, action { in_action }
        { }
        
        Marker(Json::Value jo) {
            id = jo["id"].asInt();
            center.x = jo["center"]["x"].asFloat();
            center.y = jo["center"]["y"].asFloat();
            
            for (int i = 0; i < jo["corners"].size(); i++) {
                cv::Point2f pt( jo["corners"][i]["x"].asFloat(), jo["corners"][i]["y"].asFloat() );
                corners.push_back(pt);
            }
            
            angle = jo["angle"].asFloat();
            action = jo["action"].asInt();
                
        }
        
        std::vector<cv::Point2f> corners;
        cv::Point2f center;
        double angle;
        
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
            result["center"]["x"] = center.x;
            result["center"]["y"] = center.y;
            for ( int i = 0; i < corners.size(); i++) {
                cv::Point2f pt = corners[i];
                result["corners"][i]["x"] = pt.x;
                result["corners"][i]["y"] = pt.y;
            }
            
            result["angle"] = angle;
            result["action"] = action;
            return result;
        }
    private:
        int id;
        int action; // 0: present, 1: Add, 2: Move, 3: Remove
    };
}

#endif
