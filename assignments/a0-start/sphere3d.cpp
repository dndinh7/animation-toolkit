#include "atkui/framework.h"

using glm::vec3;

class Sphere3D : public atkui::Framework {
 public:
  Sphere3D() : atkui::Framework(atkui::Perspective) {
      position = vec3(0);
      velocity = vec3(0);
  }

  virtual void scene() {
    // colors are RGB triplets in range [0,1]
    setColor(vec3(0,1,0));

    // draw a sphere at center of the world
    float radius = 50.0;
    drawSphere(vec3(0), radius);
    position += velocity * dt();

    renderer.translate(position);
    
  }

  void keyUp(int key, int mods) {
      if (key == GLFW_KEY_SPACE) {
          velocity = agl::randomUnitVector() * 30.0f;
      }
      if (key == GLFW_KEY_R) {
          position = vec3(0);
          velocity = vec3(0);
      }
  }
private:
    vec3 position;
    vec3 velocity;
};

int main(int argc, char** argv)
{
  Sphere3D viewer;
  viewer.run();
  return 0;
}
