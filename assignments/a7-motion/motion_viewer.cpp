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

       if (!paused) {
           if (cmpFloats(time, motion.getDuration())) { // once we get to the end of the motion, we immediately loop back
               time = 0;
           } else if (time > motion.getDuration()) { // we stay at the last frame for one second regardless
               time = motion.getDuration();
           }
           else {
               time += dt() * timeScale;
           }
       }

       motion.update(skeleton, time);


      if (cmpFloats(time, motion.getDuration())) {
          currentFrame = motion.getNumKeys()-1;
      }
      else {
          currentFrame = motion.getKeyID(time);
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
               if (!(currentFrame == motion.getNumKeys()-1)) {
                   time += motion.getDeltaTime();
               }
               else if (currentFrame == motion.getNumKeys()-1) {
                   time = 0;
               }
               if (time > motion.getDuration()) {
                   time = motion.getDuration();
               }
           }
           break;
       case GLFW_KEY_COMMA:
           if (paused) {
               if (!(currentFrame == 0)) {
                   time -= motion.getDeltaTime();
               }
               else if (currentFrame == 0) {
                   time = motion.getDuration();
               }
               if (time < 0) {
                   time = 0;
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

   // will have an epsilon of .0005f
   bool cmpFloats(float a, float b) {
       return fabs(a - b) < .0005f;
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
        file = "../motions/Beta/jump.bvh";
    }

    MotionViewer viewer(file);

   viewer.run();
}
