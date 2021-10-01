#ifndef _interpolator_catmullrom_
#define _interpolator_catmullrom_

#include <string>
#include <vector>
#include <iostream>
#include "interpolator.h"

//--------------------------------
// Catmull-Rom 
//--------------------------------

class InterpolatorCatmullRom : public Interpolator
{
public:
    InterpolatorCatmullRom() : Interpolator("Catmull-Rom") {}

    virtual glm::vec3 interpolate(int segment, double u) const {
       // todo: your code here
       // see: interpolator.h and writeup for details
        glm::vec3 b0 = getControlPoint(segment * 3);
        glm::vec3 b1 = getControlPoint(segment * 3 + 1);
        glm::vec3 b2 = getControlPoint(segment * 3 + 2);
        glm::vec3 b3 = getControlPoint(segment * 3 + 3);

        float t = (float) u;

        glm::vec3 p = (1 - t) * (1 - t) * (1 - t) * b0 +
            3 * (1 - t) * (1 - t) * t * b1 +
            3 * (1 - t) * t * t * b2 +
            t * t * t * b3;

       return p;
    }


    virtual void computeControlPoints(const std::vector<glm::vec3>& keys) {
       // todo: your code here
       // see: interpolator.h and writeup for details

       // each control point should only have 2 extra points, and we should not duplicate given points into the mCtrlPoints
        if (keys.size() == 0 || keys.size() == 1) { // is this necessary?
            clearControlPoints();
        } else {
            clearControlPoints();

            int j = 0;
            for (int i = 0; i < 1 + 3 * (keys.size() - 1); i++) {
                if (i % 3 == 0) { // this will be a point on the curve given
                    mCtrlPoints.push_back(keys[j]);
                    j++;
                }
                else {
                    mCtrlPoints.push_back(glm::vec3(0)); // this will be a temp value until we finally define it
                }

            }

            for (int i = 0; i < keys.size()-1; i++) { // we will consider keypoint i and i+1 at the same time
                glm::vec3 b0 = getControlPoint(3 * i);
                glm::vec3 b1;
                glm::vec3 b2;
                glm::vec3 b3 = getControlPoint(3 * (i + 1));


                if (i == 0) {
                    b1 = b0 + (b3 - b0) / 6.0f;
                } 
                else {
                    b1 = b0 + (b3 - getControlPoint((i-1)*3)) / 6.0f; // getControlPoint((i-1)*3) gets the point p_i-1 before b0 = p_i
                }
                if (i + 1 == keys.size() - 1) {
                    b2 = b3 - (b3 - b0) / 6.0f;
                }
                else {
                    b2 = b3 - (getControlPoint((i + 2) * 3) - b0) / 6.0f; // getControlPoint((i + 1) * 3) gets p_i+2 after b3 = p_i+1
                }

                editControlPoint(3 * i, b0); // we will re-edit already put in points, but it should be fine
                editControlPoint(3 * i + 1, b1);
                editControlPoint(3 * i + 2, b2);
                editControlPoint(3 * i + 3, b3);


            }
        }
    }
};

#endif
