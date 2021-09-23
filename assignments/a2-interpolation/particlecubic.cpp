#include "atkui/framework.h"
using namespace glm;

class ParticleCubic : public atkui::Framework {
 public:
  ParticleCubic() : atkui::Framework(atkui::Orthographic) {
  }

  void setup() {
      float interval = 1.0f / (60 + 1);
      points.push_back(B0);
      for (int i = 1; i <= 60; i++) {
          float t = interval * i;
          points.push_back(LERPALL(B0, B1, B2, B3, t));
      }
      points.push_back(B3);
  }

  void scene() {
    float t= fmod(elapsedTime(), 5.0f) / 5.0f;

    position = LERPALL(B0, B1, B2, B3, t);

    setColor(vec3(1,1,0));
    drawSphere(position, 10);

    setColor(vec3(0,1,1));
    for (int i = 0; i < points.size() - 1; i++) {
        drawLine(points[i], points[i + 1]);
    }
  }

  vec3 LERP(const vec3& b1, const vec3& b2, float t) {
      return b1 * (1 - t) + b2 * t;
  }

  vec3 LERPALL(const vec3& b0, const vec3& b1, const vec3& b2, const vec3& b3, float t) {
      vec3 b01 = LERP(b0, b1, t);
      vec3 b11 = LERP(b1, b2, t);
      vec3 b21 = LERP(b2, b3, t);

      vec3 b02 = LERP(b01, b11, t);
      vec3 b12 = LERP(b11, b21, t);

      return LERP(b02, b12, t);
  }

private:
    vec3 B0 = vec3(width()*0.25f, height()*0.20f, 0);
    vec3 B1 = vec3(width()/3, height()*0.6f, 0);
    vec3 B2 = vec3(width()*2/3, height()*0.4f, 0);
    vec3 B3 = vec3(width()*.75, height()*0.80f, 0);
    vec3 position;
    std::vector<vec3> points;
};


int main(int argc, char** argv) {
  ParticleCubic viewer;
  viewer.run();
}

