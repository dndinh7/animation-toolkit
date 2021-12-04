#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "cyclops.h"
#include "ik_controller.h"
#include <algorithm>
#include <string>

using namespace glm;
using namespace atk;

class Eyes {
public:
    Eyes(const vec3& EBPos) : radius(30.0f), eyeball(EBPos) {
    }
    
    vec3 eyeball;
    float radius;

};

class Unique10 : public atkui::Framework
{
public:
	Unique10() : atkui::Framework(atkui::Perspective){
        background(vec3(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f));
    }
	virtual ~Unique10() {}

    virtual void setup() {
        BVHReader reader;
        reader.load("../motions/Beta/idle.bvh", _skeleton, _motion);
        reader.load("../motions/Beta/gangnam_style.bvh", _skeleton, _motionBefore);
        _cyclops.color = vec3(0, 0, 1);
        _motion.update(_skeleton, 0);

        _rfootTarget = _skeleton.getByName("Beta:RightFoot")->getGlobalTranslation();
        _lfootTarget = _skeleton.getByName("Beta:LeftFoot")->getGlobalTranslation();

        globalHeadRot = _skeleton.getByName("Beta:Head")->getGlobalRotation();

        globalHeadSite = _skeleton.getByName("Beta:HeadSite")->getGlobalTranslation();

        hipPos = _skeleton.getByName("Beta:Hips")->getLocalTranslation();

        globalLook = _skeleton.getByName("Beta:Hips")->getGlobalTranslation();
        
        timer = agl::random(5.0f, 7.5f);

        transition = agl::random(1.5f, 3.0f);

        time = 0.0f;

        center = _skeleton.getByName("Beta:HeadSite")->getGlobalTranslation();
        Eyes e1(center + vec3(0, 150, 0));
        Eyes e2(center + vec3(150*sin(2*pi<float>()/3), 150 * cos(2 * pi<float>() / 3), 0));
        Eyes e3(center + vec3(150 * sin(4 * pi<float>() / 3), 150 * cos(4 * pi<float>() / 3), 0));

        eyes.push_back(e1);
        eyes.push_back(e2);
        eyes.push_back(e3);
    }

    virtual void scene() {
        lookAt(vec3(0, 100, 500), globalLook, vec3(0, 1, 0));
        time += dt();

        if (elapsedTime() < timer) {
            _motionBefore.update(_skeleton, elapsedTime());
            setColor(vec3(0, 1, 0));
            drawCube(vec3(0), vec3(5000, 0.2f, 5000));
        }
        else if (elapsedTime() < timer + transition) {
            time = 0;
            _motion.update(_skeleton, elapsedTime());
            _cyclops.color = vec3(0);
            setColor(vec3(0));
            background(vec3(0));
            drawCube(vec3(0), vec3(5000, 0.2f, 5000));
            for (int i = 0; i < 3; i++) {
                vec3 Z = normalize(vec3(0, 100, 350) - eyes[i].eyeball);
                vec3 X = normalize(cross(vec3(0, 1, 0), Z));
                vec3 Y = normalize(cross(Z, X));
                quat desiredRot = mat3(X, Y, Z);
                float angle = 2 * acos(desiredRot.w);
                vec3 axis = vec3(desiredRot.x/sin(angle/2), desiredRot.y/sin(angle/2), desiredRot.z/sin(angle/2));

                push();
                setColor(vec3(1));
                translate(eyes[i].eyeball);
                rotate(angle, axis);
                drawSphere(vec3(0), eyes[i].radius);
                setColor(vec3(0));
                translate(vec3(0, 0, eyes[i].radius));
                drawSphere(vec3(0), eyes[i].radius / 4);
                pop();                
            }
        }
        else {
            time += dt();
            _motion.update(_skeleton, time);
            background(vec3(0));
            _cyclops.color = vec3(0);
            scary();
            setColor(vec3(0));
            drawCube(vec3(0), vec3(5000, 0.2f, 5000));

            for (int i = 0; i < 3; i++) {
                eyes[i].eyeball = center + vec3(150*sin(1.5f*time + 2*pi<float>()*(float)i/3.0f), 150*cos(1.5f*time + 2 * pi<float>() * (float)i / 3.0f), 0);
                vec3 Z = normalize(vec3(0, 100, 350) - eyes[i].eyeball);
                vec3 X = normalize(cross(vec3(0, 1, 0), Z));
                vec3 Y = normalize(cross(Z, X));
                quat desiredRot = mat3(X, Y, Z);
                float angle = 2 * acos(desiredRot.w);
                vec3 axis = vec3(desiredRot.x / sin(angle / 2), desiredRot.y / sin(angle / 2), desiredRot.z / sin(angle / 2));

                push();
                setColor(vec3(1));
                translate(eyes[i].eyeball);
                rotate(angle, axis);
                drawSphere(vec3(0), eyes[i].radius);
                setColor(vec3(0));
                translate(vec3(0, 0, eyes[i].radius));
                drawSphere(vec3(0), eyes[i].radius / 4);
                pop();
            }
        }
        
        
        
        _cyclops.draw(_skeleton, *this);
    }

    void scary() {
        globalHeadRot = globalHeadRot * angleAxis(pi<float>() * 2*dt(), vec3(0, 0, 1));

        Joint* spine = _skeleton.getByName("Beta:Spine");
        spine->setLocalRotation(spine->getLocalRotation() * -angleAxis(pi<float>() / 2.0f, vec3(1, 0, 0)));
        _skeleton.fk();

        _lshoulderTarget = _skeleton.getByName("Beta:LeftShoulder")->getGlobalTranslation();
        _rshoulderTarget = _skeleton.getByName("Beta:RightShoulder")->getGlobalTranslation();

        vec3 rshoulder = _rshoulderTarget + vec3(0, -5 + 5 * cos(3.0f * elapsedTime()), 0);
        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightShoulder")->getID(), rshoulder, 0.001f);

        vec3 lshoulder = _lshoulderTarget + vec3(0, -5 + 5 * sin(3.0f * elapsedTime()), 0);
        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftShoulder")->getID(), lshoulder, 0.001f);

        _handPos = _skeleton.getByName("Beta:Hips")->getGlobalTranslation() + vec3(0, 15, 20) + vec3(0, 15 * sin(10.0f * elapsedTime()), 0);

        vec3 lfTarget = _lfootTarget + vec3(35.0f, 45.0f, 10) + vec3(35.0f * sin(10.0f * elapsedTime()), 45.0f * sin(10.0f * elapsedTime()), 10 * sin(10.0f * elapsedTime()));
        vec3 rfTarget = _rfootTarget + vec3(-35.0f, 45.0f, 10) + vec3(-35.0f * sin(10.0f * elapsedTime() + pi<float>()), 45.0f * sin(10.0f * elapsedTime() + pi<float>()), 10 * sin(10.0f * elapsedTime() + pi<float>()));

        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftFoot")->getID(), lfTarget, 0.001f);
        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightFoot")->getID(), rfTarget, 0.001f);
        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftHand")->getID(), globalHeadSite, 0.001f);
        ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightHand")->getID(), globalHeadSite, 0.001f);


        _skeleton.getByName("Beta:Head")->setLocalRotation(_skeleton.getByName("Beta:Head")->getLocalRotation() * inverse(_skeleton.getByName("Beta:Head")->getGlobalRotation()) * globalHeadRot);
        _skeleton.fk();

        hipPos += vec3(0, 0, 2.0f);

        _skeleton.getRoot()->setLocalTranslation(hipPos);

        _skeleton.fk();
        
    }

protected:
    Motion _motion;
    Motion _motionBefore;
    Skeleton _skeleton;
    Cyclops _cyclops;
    IKController ik;

    vec3 _handPos;
    quat globalHeadRot;
    vec3 globalHeadSite;
    vec3 hipPos;
    vec3 globalLook;

    // feet movement
    vec3 _lfootTarget;
    vec3 _rfootTarget;

    // Shoulder pos
    vec3 _rshoulderTarget;
    vec3 _lshoulderTarget;

    // time to go scary
    float timer;
    float time;
    float transition;

    // center for eyes
    vec3 center;

    std::vector<Eyes> eyes;
};

int main(int argc, char** argv) {
   Unique10 viewer;
   viewer.run();
   return 0;
}

