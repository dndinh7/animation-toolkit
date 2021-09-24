#include <iostream>
#include "atkui/framework.h"
using namespace glm;

class DrawCubic : public atkui::Framework
{
 public:
  DrawCubic() : atkui::Framework(atkui::Orthographic, 400, 400) {
  }

  virtual void setup() {
      float interval = 1.0f / (N + 1);
      pointsBernstein.push_back(B0);
      for (int i = 1; i <= N; i++) {
          float t = interval * i;
          pointsBernstein.push_back(bernsteinAlg(B0, B1, B2, B3, t));
      }
      pointsBernstein.push_back(B3);

      pointsCasteljau.push_back(B0);
      for (int i = 1; i <= N; i++) {
          float t = interval * i;
          pointsCasteljau.push_back(LERPALL(B0, B1, B2, B3, t));
      }
      pointsCasteljau.push_back(B3);
  }

  virtual void scene()
  {
    setColor(vec3(0,0,1));
    drawSphere(B0, 10);
    drawSphere(B3, 10);

    setColor(vec3(1,1,0));
    drawSphere(B1, 10);
    drawSphere(B2, 10);


    if (bernstein) { // bernstein curve is white
        setColor(vec3(1));
        for (int i = 0; i < pointsBernstein.size() - 1; i++) {
            drawLine(pointsBernstein[i], pointsBernstein[i + 1]);
        }
    }

    if (casteljau) { // casteljau curve is yellow
        setColor(vec3(1));
        for (int i = 0; i < pointsCasteljau.size() - 1; i++) {
            drawLine(pointsCasteljau[i], pointsCasteljau[i + 1]);
        }
    }

  }

  void keyUp(int key, int mod) {
    if (key == GLFW_KEY_1) {
      // draw Bernstein
        bernstein = true;
        casteljau = false;
        
    }
    else if (key == GLFW_KEY_2) {
      // draw Casteljau
        casteljau = true;
        bernstein = false;
    }
  }

  vec3 bernsteinAlg(const vec3& B0, const vec3& B1, const vec3& B2, const vec3& B3, float t) {
      vec3 point = (1.0f - t) * (1.0f - t) * (1.0f - t) * B0
          + 3 * t * (1.0f - t) * (1.0f - t) * B1
          + 3 * t * t * (1.0f - t) * B2
          + t * t * t * B3;
      return point;

  }

  vec3 LERP(const vec3& b1, const vec3& b2, float t) {
      return b1 * (1 - t) + b2 * t;
  }

  // This is basically de Casteljau's algorithm, but I named it LERPALL
  vec3 LERPALL(const vec3& b0, const vec3& b1, const vec3& b2, const vec3& b3, float t) {
      vec3 b01 = LERP(b0, b1, t);
      vec3 b11 = LERP(b1, b2, t);
      vec3 b21 = LERP(b2, b3, t);

      vec3 b02 = LERP(b01, b11, t);
      vec3 b12 = LERP(b11, b21, t);

      return LERP(b02, b12, t);
  }

 private:
  int N = 40; // number of intermediate points between B0 and B3
  bool bernstein= false; // if true then we draw bernstein
  bool casteljau= false; // if true then we draw casteljau
  vec3 B0 = vec3(100,  50, 0);
  vec3 B1 = vec3(150, 200, 0);
  vec3 B2 = vec3(250, 100, 0);
  vec3 B3 = vec3(300, 300, 0);
  std::vector<vec3> pointsBernstein;
  std::vector<vec3> pointsCasteljau;
};

int main(int argc, char **argv) {
  DrawCubic viewer;
  viewer.run();
}
