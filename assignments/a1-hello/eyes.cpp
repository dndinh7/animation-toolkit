#include "atkui/framework.h"

using namespace glm;
class Eyes : public atkui::Framework
{
  public:
    Eyes() : atkui::Framework(atkui::Orthographic) {
    }

    virtual void setup() {
        sizeBig = 150.0f;
        sizeSmall = 35.0f;
        radiusOrbit = 45.0f;
    }

    virtual void scene() {
        // need to create 4 spheres
        // 2 are static and the other 2 are dynamic
        setColor(vec3(1, 1, 1));

        drawSphere(vec3(0.30 * width(), 0.5 * height(), -100), sizeBig);
        drawSphere(vec3(0.70 * width(), 0.5 * height(), -100), sizeBig);

        setColor(vec3(0, 0, 0));

        drawSphere(vec3(0.30 * width() + radiusOrbit * sin(elapsedTime()), 0.5 * height(), 0), sizeSmall);
        drawSphere(vec3(0.70 * width() + radiusOrbit * sin(elapsedTime()), 0.5 * height(), 0), sizeSmall);
    }
private:
    float sizeBig;
    float sizeSmall;
    float radiusOrbit;

};

int main(int argc, char** argv) {
  Eyes viewer;
  viewer.run();
  return 0;
}
