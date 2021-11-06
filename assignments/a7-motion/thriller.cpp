#include <random>
#include <functional>
#include "atkui/framework.h"
#include "atk/toolkit.h"
#include "agl/renderer.h"
#include "devil.h"

using namespace atk;
using namespace glm;

class Thriller : public atkui::Framework
{
public:
   Thriller() : atkui::Framework(atkui::Perspective) {}
   virtual ~Thriller() {}

   virtual void setup() {
      BVHReader reader;
      reader.load("../motions/Warrok/WarrokThriller.bvh", _skeleton, _motion);

      vec3 position= vec3(-300, 0, -100);
      vec3 jitteredPosition;

      vec3 color;
      float size;

      for (int i = 0; i < 4; i++) {
          color = vec3(0.80f + agl::random(-0.20f, 0.20f), 0.20f + agl::random(-0.20f, 0.10f), 0.20f + agl::random(-0.20f, 0.10f));
          size = agl::random(0.35f, 0.75f);
          jitteredPosition = position + vec3(150 * i + agl::random(-35.0f, 35.0f), 0, agl::random(-20.0f, 20.0f));
          
          devils.push_back(Devil(jitteredPosition, color, size));
      }

      position = vec3(-300, 0, 0);

      for (int i = 0; i < 4; i++) {
          color = vec3(0.80f + agl::random(-0.20f, 0.20f), 0.20f + agl::random(-0.20f, 0.10f), 0.20f + agl::random(-0.20f, 0.10f));
          size = agl::random(0.35f, 0.75f);
          jitteredPosition = position + vec3(150 * i + agl::random(-35.0f, 35.0f), 0, agl::random(-20.0f, 20.0f));

          devils.push_back(Devil(jitteredPosition, color, size));
      }

      position = vec3(-300, 0, 100);

      for (int i = 0; i < 4; i++) {
          color = vec3(0.80f + agl::random(-0.20f, 0.20f), 0.20f + agl::random(-0.20f, 0.10f), 0.20f + agl::random(-0.20f, 0.10f));
          size = agl::random(0.35f, 0.75f);
          jitteredPosition = position + vec3(150 * i + agl::random(-35.0f, 35.0f), 0, agl::random(-20.0f, 20.0f));

          devils.push_back(Devil(jitteredPosition, color, size));
      }

   }

   virtual void scene() {
       if (!_paused) {
           time += dt();
           
       }
       _motion.update(_skeleton, time);

       for (auto& devil : devils) {
           devil.draw(_skeleton, *this);
       }
   }

   virtual void keyUp(int key, int mods) {
      if (key == 'P') _paused = !_paused;
   }

protected:
   Motion _motion;
   Skeleton _skeleton;
   bool _paused = false;
   std::vector<Devil> devils;
   Devil _devil;
   float time= 0.0f;
};

int main(int argc, char** argv) {
   Thriller viewer;
   viewer.run();
}

