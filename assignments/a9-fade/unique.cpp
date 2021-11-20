#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>
#include "atk/toolkit.h"
#include "atkui/skeleton_drawer.h"
#include "atkui/framework.h"
using namespace atk;
using namespace glm;

class Unique9 : public atkui::Framework {
public:
	Unique9() : atkui::Framework(atkui::Perspective) {

        mDrawer.color = vec3(1, 0, 0);
        mDrawer2.color = vec3(0, 1, 0);
        ballPos = vec3(0, 0, 0);
	}

	void setup() {
        Joint* shoulder1 = new Joint("Shoulder1");
        arm1.addJoint(shoulder1);
        shoulder1->setLocalTranslation(vec3(200, 0, 0));

        Joint* elbow1 = new Joint("Elbow1");
        arm1.addJoint(elbow1, shoulder1);
        elbow1->setLocalTranslation(vec3(100, 0, 0));

        Joint* wrist1 = new Joint("Wrist1");
        arm1.addJoint(wrist1, elbow1);
        wrist1->setLocalTranslation(vec3(80, 0, 0));

        Joint* shoulder2 = new Joint("Shoulder2");
        arm2.addJoint(shoulder2);
        shoulder2->setLocalTranslation(vec3(-200, 0, 0));

        Joint* elbow2 = new Joint("Elbow2");
        arm2.addJoint(elbow2, shoulder2);
        elbow2->setLocalTranslation(vec3(100, 0, 0));

        Joint* wrist2 = new Joint("Wrist2");
        arm2.addJoint(wrist2, elbow2);
        wrist2->setLocalTranslation(vec3(80, 0, 0));

        arm1.fk();
        arm2.fk();
	}

	void scene() {
        mDrawer.draw(arm1, *this);
        mDrawer2.draw(arm2, *this);

        setColor(vec3(0, 0, 1));
        ballPos = vec3(0, 100*sin(elapsedTime()), 100*cos(elapsedTime()));
        drawSphere(ballPos, 25.0f);
       
        solveIKTwoLink(arm1, ballPos);
        solveIKTwoLink(arm2, ballPos);
	}

    void solveIKTwoLink(Skeleton& skeleton, const vec3& goalPosition)
    {
        float l1 = length(skeleton.getByID(1)->getLocalTranslation());
        float l2 = length(skeleton.getByID(2)->getLocalTranslation());

        float r = length(goalPosition - skeleton.getByID(0)->getGlobalTranslation());

        float cphi = (r * r - l1 * l1 - l2 * l2) / (-2.0f * l1 * l2);

        cphi = max(min(cphi, 1.0f), -1.0f); // clamps cphi
        float phi = acos(cphi);

        float theta2z = phi - pi<float>();

        float sTheta1z = -(l2)*sin(theta2z) / r;
        sTheta1z = max(min(sTheta1z, 1.0f), -1.0f);

        float theta1z = asin(sTheta1z);

        float beta = atan2(-(goalPosition.z - skeleton.getByID(0)->getGlobalTranslation().z), (goalPosition.x - skeleton.getByID(0)->getGlobalTranslation().x));
        float gamma = asin(max(min((goalPosition.y - skeleton.getByID(0)->getGlobalTranslation().y)/ r, 1.0f), -1.0f));

        quat Ry = angleAxis(beta, vec3(0, 1, 0));
        quat RzGamma = angleAxis(gamma, vec3(0, 0, 1));
        quat RzTheta1 = angleAxis(theta1z, vec3(0, 0, 1));

        Transform F10(Ry * RzGamma * RzTheta1, skeleton.getByID(0)->getLocalTranslation());
        Transform F21(angleAxis(theta2z, vec3(0, 0, 1)), skeleton.getByID(1)->getLocalTranslation());
        Transform F32(angleAxis(0.0f, vec3(0, 0, 1)), skeleton.getByID(2)->getLocalTranslation());

        skeleton.getByID(0)->setLocal2Parent(F10);
        skeleton.getByID(1)->setLocal2Parent(F21);
        skeleton.getByID(2)->setLocal2Parent(F32);
        skeleton.fk();
    }

protected:
    atk::Skeleton arm1;
    atk::Skeleton arm2;
    atkui::SkeletonDrawer mDrawer;
    atkui::SkeletonDrawer mDrawer2;

    vec3 ballPos;
    vec3 ballVel;
    vec3 ballAccel;
};

int main(int argc, char** argv) {
   Unique9 viewer;
   viewer.run();
   return 0;
}

