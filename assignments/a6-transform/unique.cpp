#include "atkui/framework.h"
#include "atk/toolkit.h"
#include <deque>
using namespace atk;
using glm::vec3;

class Steve : public atkui::Framework {
public:
    Steve() : atkui::Framework(atkui::Perspective) {
        background(vec3(0));
        cameraPos = vec3(-500, 500, -500);

        setCameraEnabled(false);
    }
    virtual ~Steve() {}

    virtual void setup() {
        
        lookAt(cameraPos, vec3(0));

        Joint* root = new Joint("Hip");
        root->setLocalTranslation(vec3(0, 170.0f, 0));
        _steve.addJoint(root);

        Joint* neck = new Joint("Neck");
        neck->setLocalTranslation(vec3(0, 120.0f, 0));
        _steve.addJoint(neck, root);

        Joint* head = new Joint("Head");
        head->setLocalTranslation(vec3(40.0f*cos(0), 30.0f, 40.0f * sin(0)));
        _steve.addJoint(head, neck);

        Joint* lLeg = new Joint("LeftLeg");
        lLeg->setLocalTranslation(vec3(20.0f, -20.0f, 0));
        _steve.addJoint(lLeg, root);

        Joint* rLeg = new Joint("RightLeg");
        rLeg->setLocalTranslation(vec3(-20.0f, -20.0f, 0));
        _steve.addJoint(rLeg, root);

        Joint* lFoot = new Joint("LeftFoot");
        lFoot->setLocalTranslation(vec3(0, -140.0f, 0));
        _steve.addJoint(lFoot, lLeg);

        Joint* rFoot = new Joint("RightFoot");
        rFoot->setLocalTranslation(vec3(0, -140.0f, 0));
        _steve.addJoint(rFoot, rLeg);

        Joint* lShoulder = new Joint("LeftShoulder");
        lShoulder->setLocalTranslation(vec3(40.0f, 0, 0));
        _steve.addJoint(lShoulder, neck);

        Joint* rShoulder = new Joint("RightShoulder");
        rShoulder->setLocalTranslation(vec3(-40.0f, 0, 0));
        _steve.addJoint(rShoulder, neck);

        Joint* lArm = new Joint("LeftArm");
        lArm->setLocalTranslation(vec3(20.0f, -120.0f, 0));
        _steve.addJoint(lArm, lShoulder);

        Joint* rArm = new Joint("RightArm");
        rArm->setLocalTranslation(vec3(-20.0f, -120.0f, 0));
        _steve.addJoint(rArm, rShoulder);

        _steve.fk();
        
    }

    virtual void scene()
    {
        setColor(vec3(52.0f/255.0f, 140.0f/255.0f, 49.0f/255.0f));
        drawCube(vec3(0), vec3(5000.0f, 0.1f, 5000.0f));
        setColor(vec3(1, 215.0f/255.0f, 0));
        drawCube(vec3(0), vec3(300.0f, 0.2f, 5000.0f));


        setColor(vec3(1));
        float theta = 0.45f * sin(3.0f*elapsedTime());
        float theta2 = -0.45f * sin(3.0f*elapsedTime());

        _steve.getByName("Hip")->setLocalTranslation(_steve.getByName("Hip")->getLocalTranslation() + vec3(0, 0, -25.0f * dt()));
        _steve.getByName("Hip")->setLocalRotation(glm::angleAxis(0.60f * sin(3.0f * elapsedTime()), vec3(1, 0, 0)));

        _steve.getByName("LeftLeg")->setLocalRotation(glm::angleAxis(0.60f * sin(3.0f * elapsedTime()), vec3(1, 0, 0)));
        _steve.getByName("RightLeg")->setLocalRotation(glm::angleAxis(theta2, vec3(1, 0, 0)));
        _steve.getByName("LeftShoulder")->setLocalRotation(glm::angleAxis(theta2, vec3(1, 0, 1)));
        _steve.getByName("RightShoulder")->setLocalRotation(glm::angleAxis(theta, vec3(1, 0, 1)));

        _steve.getByName("Head")->setLocalTranslation(vec3(40.0f * cos(5.0f*elapsedTime()), 30.0f, 40.0f * sin(5.0f*elapsedTime())));

        _steve.fk();

        for (int i = 0; i < _steve.getNumJoints(); i++) {
            Joint* cur_joint = _steve.getByID(i);
            if (cur_joint == _steve.getRoot()) {
                continue;
            }

            Joint* parent = cur_joint->getParent();
            vec3 globalParentPos = parent->getGlobalTranslation();
            vec3 globalPos = cur_joint->getGlobalTranslation();

            setColor(vec3(0, 1, 1));
            drawEllipsoid(globalParentPos, globalPos, 12.5f);
        }

        setColor(vec3(1, 1, 0));
        drawSphere(_steve.getByName("Head")->getGlobalTranslation(), 50.0f);

        setColor(vec3(106.0f / 255.0f, 13.0f / 255.0f, 173.0f / 255.0f));

        drawSphere(_steve.getByName("LeftArm")->getGlobalTranslation(), 25.0f);
        drawSphere(_steve.getByName("RightArm")->getGlobalTranslation(), 25.0f);
        drawSphere(_steve.getByName("LeftFoot")->getGlobalTranslation(), 25.0f);
        drawSphere(_steve.getByName("RightFoot")->getGlobalTranslation(), 25.0f);

        cameraPos += vec3(0, 0, -22.5f * dt());
        lookAt(cameraPos, vec3(0));

    }

protected:
    Skeleton _steve;
    vec3 cameraPos;

};

int main(int argc, char** argv)
{
    Steve viewer;
    viewer.run();
}