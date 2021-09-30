#ifndef _interpolator_linear_H_
#define _interpolator_linear_H_

#include <string>
#include <vector>
#include "interpolator.h"

//--------------------------------
// Linear 
//--------------------------------

class InterpolatorLinear : public Interpolator
{
public:
    InterpolatorLinear() : Interpolator("Linear") {}
    virtual glm::vec3 interpolate(int segment, double u) const {
        // todo: your code here

        // given a segment n, the two end points are n and n+1
        glm::vec3 p0= getControlPoint(segment);
        glm::vec3 p1= getControlPoint(segment + 1);

        glm::vec3 p = (1 - (float)u) * p0 + (float)u * p1;



        return p;
    }

    virtual void computeControlPoints(const std::vector<glm::vec3>& keys) {
        // todo: your code here

        if (keys.size() == 0 || keys.size() == 1) {
            clearControlPoints();
        }
        else {
            clearControlPoints();
            for (int i = 0; i < keys.size(); i++) {
                mCtrlPoints.push_back(keys[i]);
            }
        }
    }
};

#endif
