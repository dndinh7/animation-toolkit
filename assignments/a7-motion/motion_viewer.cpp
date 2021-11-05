#include <iostream>
#include "atkui/framework.h"
#include "atk/toolkit.h"

using namespace glm;
using namespace atk;

class MotionViewer : public atkui::Framework
{
public:
   MotionViewer(std::string file) : atkui::Framework(atkui::Perspective), file(file) {
   }

   void setup() {
      BVHReader reader;
      reader.load(file, skeleton, motion);
      motion.update(skeleton, 0);
      
   }

   void scene() {
       if (time > motion.getDuration()) {
           time = 0;
      }
      motion.update(skeleton, time);
      if (time == motion.getDuration()) { //unless we actually land on the last frame, we will not have a current last frame
          currentFrame = motion.getNumKeys()-1;
      }
      else {
          currentFrame = motion.getKeyID(time);
      }

      if (!paused) {
          time += dt() * timeScale;
      }

      setColor(vec3(0,0,0.8));
      for (int i = 0; i < skeleton.getNumJoints(); i++) {
         Joint* joint = skeleton.getByID(i);
         if (joint->getParent() == 0) continue;

         vec3 p1 = joint->getGlobalTranslation();
         vec3 p2 = joint->getParent()->getGlobalTranslation();
         drawEllipsoid(p1, p2, 5);
      }

      drawText(paused? "Paused" : "Playing", 10, 15);
      drawText("Current frame: "+std::to_string(currentFrame), 10, 35);
      drawText("Time scale: "+std::to_string(timeScale), 10, 55);
   }

   virtual void keyUp(int key, int mods) {
       switch (key) {
       case GLFW_KEY_P:
           paused = !paused;
           break;
       case GLFW_KEY_0:
           time = 0;
           break;
       case GLFW_KEY_PERIOD:
           if (paused) {
               time += motion.getDeltaTime();
               if (time > motion.getDuration()) {
                   time = 0;
               }
           }
           break;
       case GLFW_KEY_COMMA:
           if (paused) {
               time -= motion.getDeltaTime();
               if (time < 0) {
                   time = motion.getDuration();
               }
           }
           break;
       case GLFW_KEY_RIGHT_BRACKET:
           if (timeScale < 2.0f) {
               timeScale += 0.10f;
           }
           break;
       case GLFW_KEY_LEFT_BRACKET:
           if (timeScale > 0.10f) {
               timeScale -= 0.10f;
           }
           break;
       }
   }

private:
   std::string file;
   Skeleton skeleton;
   Motion motion;

   float timeScale = 1.0f;
   int currentFrame = 0; 
   bool paused = false;
   float time = 0;
};


int main(int argc, char** argv) {
    std::string file;
    if (argc == 2) { // we only have one file argument
        file = argv[1];
    }
    else {
        file = "../motions/Beta/idle.bvh";
    }

    MotionViewer viewer(file);

   viewer.run();
}
