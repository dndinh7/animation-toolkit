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

      vec3 position;

      vec3 color;
      float size;

      for (int i = 0; i < 12; i++) {
          // 4x3 grid where the cells are size 100 and randomized
          position = vec3(agl::random(-200, 200), 0, agl::random(-150,150));
          color = vec3(0.80f + agl::random(0, 0.20f), 0.20f + agl::random(-0.20f, 0.10f), 0.20f + agl::random(-0.20f, 0.10f));
          size = agl::random(0.35f, 0.75f);

          devils.push_back(Devil(position, color, size));
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

