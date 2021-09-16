#include "atkui/framework.h"

using namespace glm;

class Look : public atkui::Framework {
 public:
  Look() : atkui::Framework(atkui::Orthographic) {
  }

  virtual void setup() {
    _mouseX = width() * 0.5;
    _mouseY = height() * 0.5;
    pivotLeftPupil = vec3(0.30 * width(), 0.5 * height(), 0);
    pivotRightPupil = vec3(0.70 * width(), 0.5 * height(), 0);
    theta = atan2(_mouseX, _mouseY);
    a = 42.5f;
  }

  virtual void scene() {

    setColor(vec3(1));

    drawSphere(vec3(0.30 * width(), 0.5 * height(), -100), 150.0f);
    drawSphere(vec3(0.70 * width(), 0.5 * height(), -100), 150.0f);

    setColor(vec3(0));

    vec3 pivot = vec3(_mouseX, _mouseY, 0) - pivotLeftPupil;
    theta = atan2(pivot.y, pivot.x);

    drawSphere(pivotLeftPupil + a*vec3(cos(theta), sin(theta), 0), 35.0f);

    pivot = vec3(_mouseX, _mouseY, 0) - pivotRightPupil;
    theta = atan2(pivot.y, pivot.x);

    drawSphere(pivotRightPupil + a * vec3(cos(theta), sin(theta), 0), 35.0f);



    if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
        vec2 mousePos = mousePosition();
        mouseMove(mousePos.x, mousePos.y);
        theta = atan2(_mouseX, _mouseY);
    }
    vec3 target = vec3(_mouseX, _mouseY, 0);
    setColor(vec3(1,0,0));
    drawSphere(target, 7.5);



  }

  void mouseMove(int x, int y) {
    _mouseX = x;
    _mouseY = height() - y;
  }

  
 private:
  int _mouseX;
  int _mouseY;
  vec3 pivotLeftPupil;
  vec3 pivotRightPupil;
  float theta;
  float a; // this will be the distance from the pivot
};

int main(int argc, char** argv) {
  Look viewer;
  viewer.run();
  return 0;
}
