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
        for (int i = 0; i < numArms; i++) {
            armDrawer.push_back(atkui::SkeletonDrawer());
        }

        for (int i = 0; i < numArms/2; i++) {
            vec3 color = vec3(agl::random(), agl::random(), agl::random());
            armDrawer[i].color = color;
            armDrawer[i + numArms/2].color = color;
            ballCol.push_back(color);
        }
        
	}

	void setup() {
        Skeleton arm;
        for (int i = 0; i < numArms; i++) {
            arm = Skeleton();
            arms.push_back(arm);
            Joint* shoulder = new Joint("Shoulder");
            arms[i].addJoint(shoulder);
            shoulder->setLocalTranslation(vec3(350*sin(2*pi<float>()*((float)i/(float)numArms)), 0, 350 * cos(2 * pi<float>()*((float)i / (float)numArms))));

            Joint* elbow = new Joint("Elbow");
            arms[i].addJoint(elbow, shoulder);
            elbow->setLocalTranslation(vec3(100, 0, 0));

            Joint* wrist = new Joint("Wrist");
            arms[i].addJoint(wrist, elbow);
            wrist->setLocalTranslation(vec3(80, 0, 0));

            arms[i].fk();
            
        }

        numBalls = numArms / 2;

        for (int i = 0; i < numBalls; i++) {

            ballPos.push_back(vec3(349 * sin(2 * pi<float>() * ((float)i / (float)numArms)), 135, 349 * cos(2 * pi<float>() * ((float)i / (float)numArms))));
            ballPosDraw.push_back(vec3(0));
        }

	}

	void scene() {
        for (int i = 0; i < numArms; i++) {
            armDrawer[i].draw(arms[i], *this);
        }
        for (int i = 0; i < numBalls; i++) {
            setColor(ballCol[i]);
            ballPosDraw[i] = vec3(ballPos[i].x * cos(elapsedTime()*2.0f + i*0.5f), ballPos[i].y, ballPos[i].z * cos(elapsedTime()*2.0f + i*0.5f));
            drawSphere(ballPosDraw[i], 25.0f);

            solveIKTwoLink(arms[i], ballPosDraw[i]);
            solveIKTwoLink(arms[i + numArms/2], ballPosDraw[i]);
        }
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
    std::vector<Skeleton> arms;
    std::vector<atkui::SkeletonDrawer> armDrawer;
    int numArms = 32; // has to be even
    int numBalls;
    std::vector<vec3> ballPos;
    std::vector<vec3> ballCol;
    std::vector<vec3> ballPosDraw;

    atk::Skeleton arm1;
    atk::Skeleton arm2;
    atkui::SkeletonDrawer mDrawer;
    atkui::SkeletonDrawer mDrawer2;

};

int main(int argc, char** argv) {
   Unique9 viewer;
   viewer.run();
   return 0;
}

