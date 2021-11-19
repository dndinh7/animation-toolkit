#include "atkui/framework.h"
#include "atkui/skeleton_drawer.h"
#include "atk/toolkit.h"
#include <iostream>
#include <cmath>

using namespace glm;
using namespace std;
using namespace atk;

class WASDSimple : public atkui::Framework
{
public:
  WASDSimple() : atkui::Framework(atkui::Perspective) {}

  virtual void setup()
  {
    BVHReader reader;
    reader.load("../motions/Beta/walking.bvh", _skeleton, _walk);
    _drawer.showAxes = true;
    pos = vec3(_walk.getKey(0).rootPos.x, 0, _walk.getKey(0).rootPos.z);
    _walk.update(_skeleton, 0);

    globalPos = _skeleton.getByName("Beta:Head")->getGlobalTranslation();
    
    globalLookPos = _skeleton.getByName("Beta:Spine1")->getGlobalTranslation();
  }

  virtual void scene()
  {
    update();
    _drawer.draw(_skeleton, *this);

    // draw heading
    vec3 p = _skeleton.getRoot()->getGlobalTranslation();
    p[1] = 10; // set height close above the floor

    setColor(vec3(0, 1, 1));
    push();
    translate(p);
    rotate(_heading, vec3(0, 1, 0));
    translate(vec3(0,0,25));
    scale(vec3(2, 2, 75));
    drawCylinder(vec3(0), 1.0f);
    pop();
  }

  virtual void update()
  {
    _walk.update(_skeleton, elapsedTime());

    // TODO: Your code here
    Pose pose= _skeleton.getPose();
    
    pose.jointRots[0] = glm::angleAxis(_heading, vec3(0, 1, 0));
    pose.rootPos = pos + vec3(0, pose.rootPos.y, 0);
    _skeleton.setPose(pose);

    pos += vec3(speed*sin(_heading), 0, speed*cos(_heading));
    globalPos += vec3(speed * sin(_heading), 0, speed * cos(_heading));
    globalLookPos += vec3(speed * sin(_heading), 0, speed * cos(_heading));
    
    vec3 globalPosCam = vec3(globalPos.x, globalPos.y,
        globalPos.z) + vec3(-200 * sin(_heading), 75, -200 * cos(_heading));
    
    // TODO: Override the default camera to follow the character
    lookAt(globalPosCam, globalLookPos, vec3(0, 1, 0));

    // update heading when key is down
    if (keyIsDown('D')) _heading -= 0.05;
    if (keyIsDown('A')) _heading += 0.05;
  }

protected:
  float _heading= 0.0f;
  vec3 pos;
  float speed= 0.5f;

  vec3 globalPos;
  vec3 globalLookPos;
  bool change = true;

  Motion _walk;
  Skeleton _skeleton;
  atkui::SkeletonDrawer _drawer;
};

int main(int argc, char **argv)
{
  WASDSimple viewer;
  viewer.run();
}
