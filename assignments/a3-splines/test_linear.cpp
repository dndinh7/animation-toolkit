#include "interpolator_linear.h"
#include <iostream>
#include <vector>
using namespace glm;

void test() {
    std::cout << "\n----- Starting Individual Testing-----" << std::endl;


    std::cout << "Testing key1: 2 control points" << std::endl;
    std::vector<vec3> key1;
    key1.push_back(vec3(0, 0, 0));
    key1.push_back(vec3(8, 2, 0));

    InterpolatorLinear interp1;
    interp1.computeControlPoints(key1);

    for (int i = 0; i < interp1.getNumControlPoints(); i++) {
        std::cout << "control points: " << to_string(interp1.getControlPoint(i)) << std::endl;
    }

    vec3 val1;
    for (int i = 0; i < 5; i++) {
        val1 = interp1.interpolate(0, 0.25f*i); // only 1 segment exists
        std::cout << "interpolate(0, " << std::to_string(0.25f*i) << ") = " << to_string(val1) << std::endl;
    }


    std::cout << "\nTesting key2: 1 control point" << std::endl;
    // testing when there are 0 or 1 key points, should print nothing
    std::vector<vec3> key2;
    key2.push_back(vec3(1));

    InterpolatorLinear interp2;
    interp2.computeControlPoints(key2);

    for (int i = 0; i < interp2.getNumControlPoints(); i++) {
        std::cout << "control points: " << to_string(interp2.getControlPoint(i)) << std::endl;
    }

    std::cout << "\nTesting key3: 0 control points" << std::endl;
    // 0 keys, should print nothing
    std::vector<vec3> key3;
    
    InterpolatorLinear interp3;
    interp3.computeControlPoints(key3);

    for (int i = 0; i < interp3.getNumControlPoints(); i++) {
        std::cout << "control points: " << to_string(interp3.getControlPoint(i)) << std::endl;
    }

    std::cout << "\nTesting key4: 6 control points" << std::endl;
    std::vector<vec3> key4;
    key4.push_back(vec3(0, 0, 0));
    key4.push_back(vec3(8, 2, 0));
    key4.push_back(vec3(1));
    key4.push_back(vec3(-3, 4, 11));
    key4.push_back(vec3(100, -902, 77));
    key4.push_back(vec3(-3, 11, 2));

    InterpolatorLinear interp4;
    interp4.computeControlPoints(key4);

    for (int i = 0; i < interp4.getNumControlPoints(); i++) {
        std::cout << "control points: " << to_string(interp4.getControlPoint(i)) << std::endl;
    }

    vec3 val4;
    // there is 1 less segment then there are control points
    for (int i = 0; i < interp4.getNumControlPoints()-1; i++) {
        for (int j = 0; j < 5; j++) {
            val4 = interp4.interpolate(i, 0.25f * j); // only 1 segment exists
            std::cout << "interpolate(" << std::to_string(i) << ", " << std::to_string(0.25f * j) << ") = " << to_string(val4) << std::endl;
        }
    }

}

int main()
{
   std::vector<vec3> keys;
   keys.push_back(vec3(0,0,0));
   keys.push_back(vec3(1,1,0));
   keys.push_back(vec3(2,0,0));

   InterpolatorLinear interp;
   interp.computeControlPoints(keys);

   for (int i = 0; i < interp.getNumControlPoints(); i++)
   {
      std::cout << "control points: " << to_string(interp.getControlPoint(i)) << std::endl;
   }

   vec3 value;
   value = interp.interpolate(0, 0.0);
   std::cout << "interpolate(0, 0) = " << to_string(value) << std::endl;

   value = interp.interpolate(1, 1.0);
   std::cout << "interpolate(1, 1) = " << to_string(value) << std::endl;

   value = interp.interpolate(1, 0.5);
   std::cout << "interpolate(1, 0.5) = " << to_string(value) << std::endl;

   test();
}

