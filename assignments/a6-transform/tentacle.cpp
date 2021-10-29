#include "atkui/framework.h"
#include "atk/toolkit.h"
#include <deque>
using namespace atk;
using glm::vec3;

class Tentacle : public atkui::Framework {
public:
   Tentacle() : atkui::Framework(atkui::Perspective) {

   }
   virtual ~Tentacle() {}

   virtual void setup() {
      lookAt(vec3(200), vec3(0));

      Joint* root  = new Joint("root");
      root->setLocalTranslation(vec3(0,0,0));
      _tentacle.addJoint(root);

      Joint* joint1  = new Joint("joint1");
      joint1->setLocalTranslation(vec3(50, 0, 0));
      _tentacle.addJoint(joint1, root);

      // todo: add more joints
      Joint* joint2 = new Joint("joint2");
      joint2->setLocalTranslation(vec3(50, 0, 0));
      _tentacle.addJoint(joint2, joint1);

      Joint* joint3 = new Joint("joint3");
      joint3->setLocalTranslation(vec3(50, 0, 0));
      _tentacle.addJoint(joint3, joint2);

      Joint* joint4 = new Joint("joint4");
      joint4->setLocalTranslation(vec3(50, 0, 0));
      _tentacle.addJoint(joint4, joint3);


      _tentacle.fk(); // compute local2global transforms
   }

   virtual void scene()
   {
      // todo animate joints
      // hint: use glm::angleAxis(...) to compute quaternions for each joint
      

       float theta  = sin(1.25f*elapsedTime());
       float theta2 = .80f*sin(1.29f*elapsedTime() + .30f);
       float theta3 = .70f*sin(1.33f*elapsedTime() + .25f);
       float theta4 = .65f*sin(1.37f*elapsedTime());

       _tentacle.getRoot()->setLocalRotation(glm::angleAxis(theta + glm::pi<float>() / 2, vec3(0, 0, 1)));
       _tentacle.getByID(1)->setLocalRotation(glm::angleAxis(theta2, vec3(0, 0, 1)));
       _tentacle.getByID(2)->setLocalRotation(glm::angleAxis(theta3, vec3(0, 0, 1)));
       _tentacle.getByID(3)->setLocalRotation(glm::angleAxis(theta4, vec3(0, 0, 1)));

      _tentacle.fk(); // computes local2global transforms
      setColor(vec3(106.0f/255.0f,13.0f/255.0f,173.0f/255.0f));

      // todo: loop over all joints and draw

      for (int i = 0; i < _tentacle.getNumJoints(); i++) {
          Joint* cur_joint = _tentacle.getByID(i);
          if (cur_joint == _tentacle.getRoot()) {
              continue;
          }
          
          Joint* parent = cur_joint->getParent();
          vec3 globalParentPos = parent->getGlobalTranslation();
          vec3 globalPos = cur_joint->getGlobalTranslation();
          drawEllipsoid(globalParentPos, globalPos, 12.5f);
      }

   }

protected:
   Skeleton _tentacle;

};

int main(int argc, char** argv)
{
   Tentacle viewer;
   viewer.run();
} 

