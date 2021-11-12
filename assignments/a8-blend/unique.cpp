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
    Unique8() : atkui::Framework(atkui::Perspective) {}
    virtual ~Unique8() {}

    void setup()
    {
        BVHReader reader;
        reader.load("../motions/Beta/standard_run.bvh", _skeleton, _motion);
        reader.load("../motions/Beta/walking.bvh", _skeleton, _lower);
        reader.load("../motions/Beta/samba_dancing.bvh", _skeleton, _upper);
        reader.load("../motions/Beta/right_strafe.bvh", _skeleton, _right);
        reader.load("../motions/Beta/left_strafe.bvh", _skeleton, _left);


        _heading = 0;
        _offset = vec3(0);
        _offset[1] = _motion.getKey(0).rootPos[1];
        _time = 0;

        _motion = ComputeArmFreeze(_motion);
        _reoriented = reorient(_motion, _offset, _heading);
        _splice = spliceUpperBody(_lower, _upper, _alpha);
        _rBlend = blend(_motion, _right, _balpha);
        
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

        std::cout << "this is m1 keys: " << m1.getNumKeys() << std::endl;
        std::cout << "this is m2 keys: " << m2.getNumKeys() << std::endl;
        std::cout << "this is blend keys: " << blend.getNumKeys() << std::endl;
        return blend;
    }

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
        drawer.draw(_skeleton, *this);   
        if (whichMotion == 2) {
            drawText("alpha: " + std::to_string(_alpha), 10, 15);
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
                std::cout << "hi" << std::endl;
                _reoriented3 = reorient(_rBlend, _offset, _heading);
                std::cout << "lol" << std::endl;
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
                _alpha = std::min(_alpha + 0.05, 1.0);
                _rBlend = blend(_motion, _right, _balpha);
            }
        }
        else if (key == GLFW_KEY_DOWN && whichMotion == 2)
        {
            if (whichMotion == 2) {
                _alpha = std::max(_alpha - 0.05, 0.0);
                _splice = spliceUpperBody(_lower, _upper, _alpha);
                _reoriented2 = reorient(_splice, _offset, _heading);
            }
            else if (whichMotion == 3) {
                _alpha = std::max(_alpha - 0.05, 0.0);
                _rBlend = blend(_motion, _right, _balpha);
            }

        }
    }

    Skeleton _skeleton;
    Skeleton _skeleton2;
    Motion _motion;
    Motion _right;
    Motion _left;
    Motion _lower;
    Motion _upper;
    Motion _splice;
    Motion _rBlend;
    Motion _lBlend;
    Motion _reoriented;
    Motion _reoriented2;
    Motion _reoriented3;
    Motion _reoriented4;
    vec3 _offset;
    float _heading;
    float _time;
    int whichMotion= 1;
    float _alpha = 0;
    float _balpha = 0.5f;
};

int main(int argc, char** argv)
{
    Unique8 viewer;
    viewer.run();
    return 0;
}

