#include "atk/toolkit.h"
#include "atkui/framework.h"
#include "cyclops.h"
#include <algorithm>
#include <string>

using namespace atk;
using namespace glm;
using namespace std;

class ALooker : public atkui::Framework {
public:
   ALooker() : atkui::Framework(atkui::Perspective) {}
   virtual ~ALooker() {}

   void setup() {
      Motion motion;
      BVHReader reader;
      reader.load("../motions/Beta/idle.bvh", _skeleton, motion);
      motion.update(_skeleton, 0);

      _drawer.color = vec3(1,0,0);
      _head = _skeleton.getByName("Beta:Head");
   }

   void reset(Joint* head) {
      head->setLocalRotation(IdentityQ);
      head->fk();
   }

   void lookAtTarget(Joint* head, const vec3& target) {
      // _target - globalPos of head
      // normalize that and put into z rotation matrix
      // x = (0, 1, 0) cross z vector
      // y = z cross x

       vec3 Z = normalize(target - head->getGlobalTranslation());
       vec3 X = normalize(cross(vec3(0, 1, 0), Z));
       vec3 Y = normalize(cross(X, Z));


       std::cout << "x: " << X << std::endl;
       std::cout << "y: " << Y << std::endl;
       std::cout << "z: " << Z << std::endl;
       

       mat3 lol;

       lol[0][0] = X.x;
       lol[1][0] = X.y;
       lol[2][0] = X.z;
       lol[0][1] = Y.x;
       lol[1][1] = Y.y;
       lol[2][1] = Y.z;
       lol[0][2] = Z.x;
       lol[1][2] = Z.y;
       lol[2][2] = Z.z;

       Transform desired(lol, head->getGlobalTranslation());

       desired = head->getLocal2Global().inverse() * desired;

       head->setLocal2Parent(head->getLocal2Parent() * desired);
       head->fk();
   }

   void scene() {  
      float r = 100;
      float angle = elapsedTime();
      _target = vec3(r * cos(angle), r * sin(angle) + r, 150);

      reset(_head);
      lookAtTarget(_head, _target);
      _drawer.draw(_skeleton, *this);

      vec3 globalHeadPos = _head->getGlobalTranslation();
      vec3 globalForward = _head->getGlobalRotation() * vec3(0,0,1);

      setColor(vec3(0,0,1));
      drawLine(globalHeadPos, globalHeadPos + 200.0f*globalForward);
      drawSphere(_target, 5);
   }

   Cyclops _drawer;
   Skeleton _skeleton;
   Joint* _head;
   vec3 _target;
};

int main(int argc, char** argv) {
   ALooker viewer;
   viewer.run();
   return 0;
}

