#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "atkui/skeleton_drawer.h"
#include <deque>
#include <algorithm>
#include <string>

using namespace atk;
using namespace atkui;
using namespace glm;

class ASplice : public atkui::Framework
{
public:
   ASplice() : atkui::Framework(atkui::Perspective) {}
   virtual ~ASplice() {}

   void setup()
   {
      _alpha = 0;

      BVHReader reader;
      reader.load("../motions/Beta/walking.bvh", _skeleton, _lower);
      reader.load("../motions/Beta/gangnam_style.bvh", _skeleton, _upper);

      _splice = spliceUpperBody(_lower, _upper, _alpha);
   }

   Motion spliceUpperBody(const Motion& lower, const Motion& upper, float alpha)
   {
      Motion result;
      result.setFramerate(lower.getFramerate());

      Joint* cur_joint = _skeleton.getByName("Beta:Spine1");
      std::deque<int> unvisited;
      unvisited.push_back(cur_joint->getID());
      std::vector<int> ID;
      while(!unvisited.empty()) { // BFS to get IDs of the upperbody, tree traversal
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

   void scene()
   {  
      _splice.update(_skeleton, elapsedTime() * 0.5);
      SkeletonDrawer drawer;
      drawer.draw(_skeleton, *this);
      drawText("alpha: "+std::to_string(_alpha), 10, 15);
   }

   void keyUp(int key, int mods) 
   {
      if (key == GLFW_KEY_UP)
      {
         _alpha = std::min(_alpha+0.05, 1.0);
         _splice = spliceUpperBody(_lower, _upper, _alpha);
      }
      else if (key == GLFW_KEY_DOWN)
      {
         _alpha = std::max(_alpha-0.05, 0.0);
         _splice = spliceUpperBody(_lower, _upper, _alpha);
      }
   }

   Skeleton _skeleton;
   Motion _upper;
   Motion _lower;
   Motion _splice;
   float _alpha;
};

int main(int argc, char** argv)
{
   ASplice viewer;
   viewer.run();
   return 0;
}

