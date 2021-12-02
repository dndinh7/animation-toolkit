#include "ik_controller.h"
#include <cmath>
#include <iostream>

using namespace atk;
using namespace glm;
using namespace std;

bool IKController::solveIKAnalytic(Skeleton& skeleton, 
    int jointid, const vec3& goalPos, float epsilon) {
  if (jointid == -1) return true;

  Joint* ankle = skeleton.getByID(jointid);
  if (!ankle->getParent()) {
    std::cout << "Warning: solveIKAnalytic() needs joint "
      "with parent and grandparent\n";
    return false;
  }

  Joint* knee = ankle->getParent();
  if (!knee->getParent()) {
    std::cout << "Warning: solveIKAnalytic() needs joint "
      "with parent and grandparent\n";
    return false;
  }

  Joint* hip = knee->getParent();


  // TODO: Your code here
  Joint* j3 = skeleton.getByID(jointid);

  Joint* j2 = j3->getParent();

  Joint* j1 = j2->getParent();

  float l1 = length(j3->getLocalTranslation());
  float l2 = length(j2->getLocalTranslation());

  float r = length(goalPos - j1->getGlobalTranslation());

  float cphi = (r * r - l1 * l1 - l2 * l2) / (-2.0f * l1 * l2);

  cphi = std::max(std::min(cphi, 1.0f), -1.0f); // clamps cphi
  float phi = acos(cphi);

  float theta2z = phi - pi<float>();

  vec3 limbDir = normalize(j2->getLocalTranslation());
  vec3 axis = cross(limbDir, vec3(0, 0, -1));
  axis = (limbDir[1] < 0) ? cross(limbDir, vec3(0, 0, 1)) : axis;

  quat R21 = angleAxis(theta2z, axis);

  skeleton.getByID(j2->getID())->setLocalRotation(R21);

  skeleton.fk();

  vec3 p30 = j3->getGlobalTranslation();

  vec3 r_bar = p30 - j1->getGlobalTranslation();


  vec3 e_bar = goalPos - p30;

  float cphi2 = length(cross(r_bar, e_bar));

  float phi2 = atan2(length(cross(r_bar, e_bar)), dot(r_bar, r_bar) + dot(r_bar, e_bar));

  vec3 axis2 = normalize(cross(r_bar, e_bar));

  quat R10 = angleAxis(phi2, axis2);

  skeleton.fk();

  if (length(j3->getGlobalTranslation()) - length(goalPos) > epsilon) return false;


  return true;
}

bool IKController::solveIKCCD(Skeleton& skeleton, int jointid, 
    const vec3& goalPos, const std::vector<Joint*>& chain, 
    float threshold, int maxIters, float nudgeFactor) {
  // There are no joints in the IK chain for manipulation
  if (chain.size() == 0) return true;

  // TODO: Your code here
  
  std::cout << skeleton.getByID(jointid)->getLocalTranslation() << std::endl;

  return false;
}
