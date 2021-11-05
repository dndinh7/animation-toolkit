#include <iostream>
#include "atkui/framework.h"
#include "atk/toolkit.h"
#include "devil.h"

using namespace glm;
using namespace atk;

class Unique7 : public atkui::Framework
{
public:
    Unique7(std::string file) : atkui::Framework(atkui::Perspective), file(file) {
    }

    void setup() {
        BVHReader reader;
        reader.load(file, skeleton, motion);
        motion.update(skeleton, 0);
        color = vec3(0.8f, 0.3f, 0.1f);

        skeleton.fk();
    }

    void scene() {
        Joint* rHand = skeleton.getByName("rMid1");

        Transform rh = rHand->getLocal2Global();

        Joint* lHand = skeleton.getByName("lMid1");

        Transform lh = lHand->getLocal2Global();

        Transform Boxing(
            glm::angleAxis(0.0f, vec3(1, 0, 0)),
            vec3(0, 0, 0),
            vec3(20, 20, 20));


        setColor(vec3(226/255.0f, 25/255.0f, 33/255.0f));
        push();
        transform(rh*Boxing);
        drawSphere(vec3(0), 1.0f);
        pop();

        push();
        transform(lh*Boxing);
        drawSphere(vec3(0), 1.0f);
        pop();


        time += dt();
        motion.update(skeleton, time);

        color += vec3(agl::random(-0.01f, 0.01f), agl::random(-0.01f, 0.01f), agl::random(-0.01f, 0.01f));
        if (color.x < 0) color.x = 0;
        if (color.x > 1) color.x = 1;
        if (color.y < 0) color.y = 0;
        if (color.y > 1) color.y = 1;
        if (color.z < 0) color.z = 0;
        if (color.z > 1) color.z = 1;

        setColor(color);
        for (int i = 0; i < skeleton.getNumJoints(); i++) {
            Joint* joint = skeleton.getByID(i);
            if (joint->getParent() == 0) continue;

            vec3 p1 = joint->getGlobalTranslation();
            vec3 p2 = joint->getParent()->getGlobalTranslation();
            drawEllipsoid(p1, p2, 5);
        }

        Joint* head = skeleton.getByName("head");
        Transform headPos = head->getLocal2Global();

        Transform header(
            glm::angleAxis(0.0f, vec3(0, 1, 0)),
            vec3(0, 0, 0),
            vec3(35, 35, 10));

        setColor(vec3(1, 1, 0));
        push();

        transform(headPos*header);

        drawCylinder(vec3(0), 1);

        pop();

        Transform topHatBrim(
            glm::angleAxis(glm::pi<float>() / 2, vec3(1, 0, 0)),
            vec3(0, 15.0f, 0),
            vec3(40, 40, 2.5f));

        Transform topHat(
            glm::angleAxis(glm::pi<float>() / 2, vec3(1, 0, 0)),
            vec3(0, 35.0f, 0),
            vec3(30.0f, 30.0f, 40.0f));
        
        setColor(vec3(0));
        push();
        transform(headPos * topHatBrim);
        drawCylinder(vec3(0), 1);
        pop();

        push();
        transform(headPos * topHat);
        drawCylinder(vec3(0), 1);
        pop();

        Transform lEyePos = skeleton.getByName("leftEye")->getLocal2Global();

        Transform rEyePos = skeleton.getByName("rightEye")->getLocal2Global();

        setColor(vec3(0));
        push();

        transform(lEyePos);
        drawSphere(vec3(0), 7.5f);

        pop();

        push();

        transform(rEyePos);
        drawSphere(vec3(0), 7.5f);

        pop();

    }


private:
    std::string file;
    Skeleton skeleton;
    Motion motion;
    vec3 color;

    float timeScale = 1.0f;
    int currentFrame = 0;
    bool paused = false;
    float time = 0;
};


int main(int argc, char** argv) {
    std::string file= "../motions/Beta/strike.bvh";
    Unique7 viewer(file);

    viewer.run();
}
