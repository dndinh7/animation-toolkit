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
   }

   void update()
   {
      _motion.update(_skeleton, elapsedTime());

      IKController ik;
      // TODO: Your code here
      ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:LeftHand")->getID(), _lhandTarget, 0.001f);
      std::cout << "left target: " << _lhandTarget << std::endl;
      std::cout << "left hand: " << _skeleton.getByName("Beta:LeftHand")->getGlobalTranslation() << std::endl;
      //ik.solveIKAnalytic(_skeleton, _skeleton.getByName("Beta:RightHand")->getID(), _rhandTarget, 0.001f);

   }

   void scene()
   {  
      update();
      _drawer.draw(_skeleton, *this);
      setColor(vec3(0,0,1));
      _lhandTarget = vec3(0, 42.5f * sin(2.4f*elapsedTime()), 0) + _lhandTargetOffset;
      _rhandTarget = vec3(0, 42.5f * sin(2.4f*elapsedTime() + 1.0f), 0) + _rhandTargetOffset;

      drawSphere(_lhandTarget, 10);
      drawSphere(_rhandTarget, 10);
   }

protected:
   Cyclops _drawer;
   Skeleton _skeleton;
   Motion _motion;

   // Hand target positions
   vec3 _lhandTarget;
   vec3 _rhandTarget;
   vec3 _lhandTargetOffset;
   vec3 _rhandTargetOffset;
};

int main(int argc, char** argv)
{
   ADancer viewer;
   viewer.run();
   return 0;
}

