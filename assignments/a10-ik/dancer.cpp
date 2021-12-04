#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "cyclops.h"
#include "ik_controller.h"
#include <algorithm>
#include <string>

using namespace atk;
using namespace glm;
using namespace std;

class ADancer : public atkui::Framework
{
public:
   ADancer() : atkui::Framework(atkui::Perspective) {}
   virtual ~ADancer() {}

   void setup()
   {
      BVHReader reader;
      reader.load("../motions/Beta/idle.bvh", _skeleton, _motion);
      _drawer.color = vec3(1,0,0);
      _motion.update(_skeleton, 0);

      _lhandTargetOffset = _skeleton.getByName("Beta:LeftHand")->getGlobalTranslation() + vec3(0, 50, 15);
      _rhandTargetOffset = _skeleton.getByName("Beta:RightHand")->getGlobalTranslation() + vec3(0, 50, 15);
      _globHip = _skeleton.getRoot()->getGlobalTranslation() - vec3(3.0f, 7.5f, 0);
      _lfootOrig = _skeleton.getByName("Beta:LeftFoot")->getGlobalTranslation();
      _rfootOrig = _skeleton.getByName("Beta:RightFoot")->getGlobalTranslation();
      _lfootRot = _skeleton.getByName("Beta:LeftFoot")->getGlobalRotation();
      _rfootRot = _skeleton.getByName("Beta:RightFoot")->getGlobalRotation();
   }

   void update()
   {
      _motion.update(_skeleton, elapsedTime());

      IKController ik;
      // TODO: Your code here
      _skeleton.getRoot()->setLocalTranslation(_globHip + vec3(6.0f * sin(4.5f*elapsedTime()), 7.5f * abs(cos(4.5f * elapsedTime())), 0));
      _skeleton.fk();
      ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftFoot")->getID(), _lfootOrig, 0.001f);
      ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightFoot")->getID(), _rfootOrig, 0.001f);
      _skeleton.getByName("Beta:LeftFoot")->setLocalRotation(_skeleton.getByName("Beta:LeftFoot")->getLocalRotation() * inverse(_skeleton.getByName("Beta:LeftFoot")->getGlobalRotation()) * _lfootRot);
      _skeleton.getByName("Beta:RightFoot")->setLocalRotation(_skeleton.getByName("Beta:RightFoot")->getLocalRotation() * inverse(_skeleton.getByName("Beta:RightFoot")->getGlobalRotation()) * _rfootRot);
      _skeleton.fk();
      ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftHand")->getID(), _lhandTarget, 0.001f);
      ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightHand")->getID(), _rhandTarget, 0.001f);
   }

   void scene()
   {  
      update();
      _drawer.draw(_skeleton, *this);
      setColor(vec3(0,0,1));
      _lhandTarget = vec3(0, 42.5f * sin(4.0f*elapsedTime()), 0) + _lhandTargetOffset;
      _rhandTarget = vec3(0, 42.5f * sin(4.0f*elapsedTime() + 1.5f), 0) + _rhandTargetOffset;
      

      /*drawSphere(_lhandTarget, 10);
      drawSphere(_rhandTarget, 10);*/
   }

protected:
   Cyclops _drawer;
   Skeleton _skeleton;
   Motion _motion;
   vec3 _globHip;

   // Hand target positions
   vec3 _lhandTarget;
   vec3 _rhandTarget;
   vec3 _lhandTargetOffset;
   vec3 _rhandTargetOffset;

   // Original feet positions
   vec3 _lfootOrig;
   vec3 _rfootOrig;

   // Original Global Rotation
   quat _lfootRot;
   quat _rfootRot;
};

int main(int argc, char** argv)
{
   ADancer viewer;
   viewer.run();
   return 0;
}

