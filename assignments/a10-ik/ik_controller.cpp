#include "ik_controller.h"
#include "atkui/framework.h"
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

  if (length(goalPos - j3->getGlobalTranslation()) <= epsilon) return true; // if within epsilon of goal, then we reached goal

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

  j2->setLocalRotation(R21);

  j1->setLocalRotation(IdentityQ);

  skeleton.fk();

  vec3 p30 = j3->getGlobalTranslation();

  vec3 r_bar = p30 - j1->getGlobalTranslation();

  vec3 e_bar = goalPos - p30;

  vec3 rXe = cross(r_bar, e_bar);

  float phi2 = atan2(length(rXe), dot(r_bar, r_bar) + dot(r_bar, e_bar));

  vec3 u = normalize(rXe);
  if (length(rXe) <= epsilon) {
      u = vec3(0, 0, 1);
  }

  Joint* par = j1->getParent();

  quat rot = (j1 != skeleton.getRoot()) ? angleAxis(phi2, inverse(par->getGlobalRotation()) * u) : angleAxis(phi2, u);

  j1->setLocalRotation(rot * j1->getLocalRotation());

  skeleton.fk();

  if (length(goalPos - j3->getGlobalTranslation()) <= epsilon) return true; // if within epsilon of goal, then we reached goal

  return false;
}

bool IKController::solveIKCCD(Skeleton& skeleton, int jointid, 
    const vec3& goalPos, const std::vector<Joint*>& chain, 
    float threshold, int maxIters, float nudgeFactor) {
  // There are no joints in the IK chain for manipulation
  if (chain.size() == 0) return true;

  float epsilon = 0.001f;

  // TODO: Your code here
  Joint* endEffector = skeleton.getByID(jointid);

  vec3 EEPos = endEffector->getGlobalTranslation();

  if (length(goalPos - EEPos) < threshold) return true;

  int iterations = 0;

  while (length(goalPos - EEPos) > threshold && iterations < maxIters) {
      for (int i = 0; i < chain.size(); i++) {
          Joint* rotJoint = chain[i];

          vec3 rotJointPos = rotJoint->getGlobalTranslation();

          vec3 r_bar = EEPos - rotJointPos;

          vec3 e_bar = goalPos - EEPos;

          if (length(e_bar) > epsilon) {
              vec3 rXe = cross(r_bar, e_bar);

              float phi = atan2(length(rXe), dot(r_bar, r_bar) + dot(r_bar, e_bar));

              vec3 u = normalize(rXe);
              if (length(rXe) > epsilon) {
                  float nudgeAngle = nudgeFactor * phi;

                  Joint* par = rotJoint->getParent();

                  quat rot = angleAxis(nudgeAngle, inverse(par->getGlobalRotation()) * u);

                  rotJoint->setLocalRotation(rot * rotJoint->getLocalRotation());

                  skeleton.fk();
              }

              EEPos = endEffector->getGlobalTranslation();
          }    
      }
      iterations++;
  }

  if (length(goalPos - EEPos) <= threshold) return true;

  return false;
}
