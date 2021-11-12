#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "atkui/skeleton_drawer.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <deque>

using namespace glm;
using namespace atk;
using namespace atkui;

class Unique8 : public atkui::Framework
{
public:
    Unique8() : atkui::Framework(atkui::Perspective) {
        background(vec3(0));
    }
    virtual ~Unique8() {}

    void setup()
    {
        BVHReader reader;
        reader.load("../motions/Beta/standard_run.bvh", _skeleton, _motion);
        reader.load("../motions/Beta/walking.bvh", _skeleton, _lower);
        reader.load("../motions/Beta/samba_dancing.bvh", _skeleton, _upper);
        reader.load("../motions/Beta/right_strafe.bvh", _skeleton, _right);
        reader.load("../motions/Beta/left_strafe.bvh", _skeleton, _left);

        wasd.push_back(vec3(0));
        wasd.push_back(vec3(0, 0, 75.0f)); // w button
        wasd.push_back(vec3(0, 0, -75.0f)); // s button
        wasd.push_back(vec3(-75.0f, 0, 0)); // d button
        wasd.push_back(vec3(75.0f, 0, 0)); // a button

        _heading = 0;
        _offset = vec3(0);
        _offset[1] = _motion.getKey(0).rootPos[1];
        _time = 0;

        _motion = ComputeArmFreeze(_motion);
        _reoriented = reorient(_motion, _offset, _heading);
        _splice = spliceUpperBody(_lower, _upper, _alpha);
        _rBlend = blend(_left, _right, _balpha);
        
    }

    // This uses the blend command to blend two motions together
    Motion blend(const Motion& m1, const Motion& m2, double alpha)
    {
        Motion blend;
        blend.setFramerate(m1.getFramerate());

        float duration = m1.getDuration() * (1 - alpha) + m2.getDuration() * alpha;
        float deltaT = 1.0f / blend.getFramerate();
        for (float t = 0; t <= duration; t += deltaT) {
            float t1 = t / duration * m1.getDuration();
            float t2 = t / duration * m2.getDuration();
            Pose pose1 = m1.getValue(t1);
            Pose pose2 = m2.getValue(t2);
            Pose newPose = Pose::Lerp(pose1, pose2, alpha);
            blend.appendKey(newPose);
        }
        return blend;
    }

    // splices
    Motion spliceUpperBody(const Motion& lower, const Motion& upper, float alpha)
    {
        Motion result;
        result.setFramerate(lower.getFramerate());

        Joint* cur_joint = _skeleton.getByName("Beta:Spine1");
        std::deque<int> unvisited;
        unvisited.push_back(cur_joint->getID());
        std::vector<int> ID;
        while (!unvisited.empty()) { // BFS to get IDs of the upperbody, tree traversal
            cur_joint = _skeleton.getByID(unvisited.front());
            unvisited.pop_front();
            ID.push_back(cur_joint->getID());
            for (int i = 0; i < cur_joint->getNumChildren(); i++) {
                unvisited.push_back(cur_joint->getChildAt(i)->getID());
            }
        }

        Pose lowerPose;
        Pose upperPose;

        for (int i = 0; i < lower.getNumKeys(); i++) {
            lowerPose = lower.getKey(i);
            upperPose = upper.getKey(i + 120);

            for (int j = 0; j < ID.size(); j++) { // if part of the upperbody slerp
                lowerPose.jointRots[ID[j]] = glm::slerp(upperPose.jointRots[ID[j]], lowerPose.jointRots[ID[j]], alpha);
            }
            result.appendKey(lowerPose);
        }
        return result;
    }

    // reorients the motion
    Motion reorient(const Motion& motion, const vec3& pos, float heading)
    {
        Motion result;
        result.setFramerate(motion.getFramerate());

        Pose first = motion.getKey(0);
        Transform to_origin(glm::angleAxis(0.f, vec3(1, 0, 0)), -first.rootPos);
        Transform desired(angleAxis(heading, vec3(0, 1, 0)), pos);

        for (int i = 0; i < motion.getNumKeys(); i++) {
            Pose rest = motion.getKey(i);
            Transform orig(rest.jointRots[0], rest.rootPos);

            Transform newState = desired * to_origin * orig;

            rest.rootPos = newState.t();
            rest.jointRots[0] = newState.r();

            result.appendKey(rest);
        }

        return result;
    }

    Motion ComputeArmFreeze(const Motion& motion) {
        quat leftRot = eulerAngleRO(XYZ, radians(vec3(0, 60, -55)));
        quat rightRot = eulerAngleRO(XYZ, radians(vec3(0, -60, 55)));
        quat elbowRot = eulerAngleRO(XYZ, radians(vec3(0, 0, 0)));
        quat spineRot = eulerAngleRO(XYZ, radians(vec3(30, 0, 0)));

        Joint* leftArm = _skeleton.getByName("Beta:LeftArm");
        Joint* rightArm = _skeleton.getByName("Beta:RightArm");

        Joint* leftElbow = _skeleton.getByName("Beta:LeftForeArm");
        Joint* rightElbow = _skeleton.getByName("Beta:RightForeArm");

        Joint* spine = _skeleton.getByName("Beta:Spine");

        Motion result;
        result.setFramerate(motion.getFramerate());
        for (int i = 0; i < motion.getNumKeys(); i++) {
            Pose pose = motion.getKey(i);
            pose.jointRots[leftArm->getID()] = leftRot;
            pose.jointRots[rightArm->getID()] = rightRot;
            pose.jointRots[leftElbow->getID()] = elbowRot;
            pose.jointRots[rightElbow->getID()] = elbowRot;
            pose.jointRots[spine->getID()] = spineRot;
            result.appendKey(pose);
        }
        return result;
    }

    void update()
    {
        _time += dt() * 0.75;
        if (whichMotion == 1) {
            _reoriented.update(_skeleton, _time);
        }
        else if (whichMotion == 2) {
            _reoriented2.update(_skeleton, _time);
        }
        else if (whichMotion == 3) {
            _reoriented3.update(_skeleton, _time);
        }
    }

    void scene()
    {
        update();
        SkeletonDrawer drawer;
        setColor(vec3(1, 1, 0));
        drawer.draw(_skeleton, *this);   
        if (whichMotion == 2) {
            drawText("splice alpha: " + std::to_string(_alpha), 10, 15);
        }
        else if (whichMotion == 3) {
            drawText("blend alpha: " + std::to_string(_balpha), 10, 15);
        }
        setColor(vec3(1));
        drawCube(vec3(0, .1f, 0), vec3(5000, 1, 5000));
        
        Joint* head = _skeleton.getByName("Beta:Head");
        Transform headPos = head->getLocal2Global();

        push();
        setColor(vec3(1));
        transform(headPos);
        drawSphere(vec3(0, 25, 0), 65.0f); //head
        setColor(vec3(0));
        drawSphere(vec3(15, 45, 25), 10.0f); // eye
        drawSphere(vec3(-15, 45, 25), 10.0f); // eye
        setColor(vec3(1, 0, 0));
        drawSphere(vec3(0, 30, 32.5f), 15.0f); // nose
        setColor(vec3(0));
        drawSphere(vec3(2.5, 8.5, 25.0f), 10.0f);
        drawSphere(vec3(10, 10, 25), 10.0f);
        drawSphere(vec3(15, 15, 25), 10.0f);
        drawSphere(vec3(20, 20, 22.5), 10.0f);
        drawSphere(vec3(-2.5, 8.5, 25.0f), 10.0f);
        drawSphere(vec3(-10, 10, 25), 10.0f);
        drawSphere(vec3(-15, 15, 25), 10.0f);
        drawSphere(vec3(-20, 20, 22.5), 10.0f);
        pop();

        Transform topHatBrim(
            glm::angleAxis(glm::pi<float>() / 2, vec3(1, 0, 0)),
            vec3(0, 55.0f, 0),
            vec3(75, 75, 7.5f));

        Transform topHat(
            glm::angleAxis(glm::pi<float>() / 2, vec3(1, 0, 0)),
            vec3(0, 85.0f, 0),
            vec3(50.0f, 50.0f, 60.0f));

        setColor(vec3(0));
        push();
        transform(headPos * topHatBrim);
        drawCylinder(vec3(0), 1);
        pop();

        push();
        transform(headPos * topHat);
        drawCylinder(vec3(0), 1);
        pop();


        if (showWASD) {
            vec3 origin = _skeleton.getByName("Beta:Hips")->getGlobalTranslation() + vec3(0, 200, 0);

            setColor(vec3(0, 1, 0));
            drawEllipsoid(origin, origin + wasd[1], 10.0f); // w dir
            setColor(vec3(1, 0, 0));
            drawEllipsoid(origin, origin + wasd[2], 10.0f); // s dir 
            setColor(vec3(0, 0, 1));
            drawEllipsoid(origin, origin + wasd[3], 10.0f); // d dir
            setColor(vec3(1, 1, 0));
            drawEllipsoid(origin, origin + wasd[4], 10.0f); // a dir
        }

        
    }


    void keyUp(int key, int mods)
    {
        if (key == GLFW_KEY_LEFT)
        {
            _heading += pi<float>() / 8;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
           
            _time = 0;
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            _heading -= pi<float>() / 8;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
            _time = 0;
        }
        if (key == 'W')
        {
            _offset[2] += 25;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
            std::cout << _offset << std::endl;
            _time = 0;
        }
        else if (key == 'S')
        {
            _offset[2] -= 25;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
            _time = 0;
            std::cout << _offset << std::endl;
        }
        else if (key == 'A')
        {
            _offset[0] += 25;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
            _time = 0;
            std::cout << _offset << std::endl;
        }
        else if (key == 'D')
        {
            _offset[0] -= 25;
            if (whichMotion == 1) {
                _reoriented = reorient(_motion, _offset, _heading);
            }
            else if (whichMotion == 2) {
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
            _time = 0;
            std::cout << _offset << std::endl;
        }
        else if (key == '1')
        {
            if (whichMotion != 1) {
                _time = 0;
                _heading = 0;
                _offset[0] = 0;
                _offset[1] = _motion.getKey(0).rootPos[1];
                _offset[2] = 0;
                whichMotion = 1;
                _reoriented = reorient(_motion, _offset, _heading);
            }
        }
        else if (key == '2') {
            if (whichMotion != 2) {
                _time = 0;
                _heading = 0;
                _offset[0] = 0;
                _offset[1] = _lower.getKey(0).rootPos[1];
                _offset[2] = 0;
                whichMotion = 2;
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
        }
        else if (key == '3') {
            if (whichMotion != 3) {
                _time = 0;
                _heading = 0;
                _offset[0] = 0;
                _offset[1] = _rBlend.getKey(0).rootPos[1];
                _offset[2] = 0;
                whichMotion = 3;
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
        }
        if (key == GLFW_KEY_UP)
        {
            if (whichMotion == 2) {
                _alpha = std::min(_alpha + 0.05, 1.0);
                _splice = spliceUpperBody(_lower, _upper, _alpha);
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _balpha = std::min(_balpha + 0.05, 1.0);
                _rBlend = blend(_left, _right, _balpha);
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }
        }
        else if (key == GLFW_KEY_DOWN)
        {
            if (whichMotion == 2) {
                _alpha = std::max(_alpha - 0.05, 0.0);
                _splice = spliceUpperBody(_lower, _upper, _alpha);
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _balpha = std::max(_balpha - 0.05, 0.0);
                _rBlend = blend(_left, _right, _balpha);
                _reoriented3 = reorient(_rBlend, _offset, _heading);
            }

        }
        if (key == 'V') {
            showWASD = !showWASD;
        }
    }

    Skeleton _skeleton;
    Motion _motion;
    Motion _right;
    Motion _left;
    Motion _lower;
    Motion _upper;
    Motion _splice;
    Motion _rBlend;
    Motion _reoriented;
    Motion _reoriented2;
    Motion _reoriented3;
    vec3 _offset;
    float _heading;
    float _time;
    int whichMotion= 1;
    float _alpha = 0;
    float _balpha = 0.5f;
    std::vector<vec3> wasd;
    bool showWASD= false;
};

int main(int argc, char** argv)
{
    Unique8 viewer;
    viewer.run();
    return 0;
}

