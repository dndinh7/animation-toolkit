#include "atkui/framework.h"

using namespace glm;

class DualCircle {
public:
    DualCircle(vec3 pivot, float theta, float centerRadius, bool CW, vec3 color) : pivot{ pivot }, spinTheta{ 0.0f }, spinRadius{ 25.0f }, centerTheta(theta), centerRadius{ centerRadius }, CW{ CW }, color1 { color } {
        position1 = pivot + vec3(spinRadius * cos(spinTheta), spinRadius * sin(spinTheta), 0);
        position2 = pivot + vec3(spinRadius * cos(spinTheta + pi<float>()), spinRadius * sin(spinTheta + pi<float>()), 0);

        float color_range = max(max(color1.x, color1.y), color1.z) + min(min(color1.x, color1.y), color.z);
        color2 = vec3(color_range - color1.x, color_range - color1.y, color_range - color.z);
    }

    void updatePivot(vec3 center, float centerThetaRate) {
        centerTheta += centerThetaRate;
        pivot = center + vec3(centerRadius * cos(centerTheta), centerRadius * sin(centerTheta), 0);


        position1 = pivot + vec3(spinRadius * cos(spinTheta), spinRadius * sin(spinTheta), 0);
        position2 = pivot + vec3(spinRadius * cos(spinTheta + pi<float>()), spinRadius * sin(spinTheta + pi<float>()), 0);
    }

    void spin(float dt) {
        float spinRate = 4.0f * dt;
        if (CW) {
            spinTheta -= spinRate;
        }
        else {
            spinTheta += spinRate;
        }
        
        position1 = pivot + vec3(spinRadius * cos(spinTheta), spinRadius * sin(spinTheta), 0);
        position2 = pivot + vec3(spinRadius * cos(spinTheta + pi<float>()), spinRadius * sin(spinTheta + pi<float>()), 0);
    }

//private:
    bool  CW; // if true then dual circle spins clockwise and if false then dual circle and if false then they will spin counter clockwise
    vec3  pivot; // this will be the pivot where the two circles spin
    float centerRadius; // this will be the distance that the pivot is from the center
    float spinRadius; // this will be the distance from which each circle is fro the pivot
    float centerTheta; // this will the angle the pivot is from the center
    float spinTheta; // this will be the angle at which the circles are from the center
    vec3  position1; // position of the first circle
    vec3  position2; // position of the second circle
    vec3  color1; // color of the first circle
    vec3  color2; // color of the second circle (this color will be complementary to the first color)

    // Reference: https://stackoverflow.com/questions/3054873/programmatically-find-complement-of-colors
    // I used the most upvoted answer to get the conjecture for complementary RGB colors
};

class Unique : public atkui::Framework {
 public:
  Unique() : atkui::Framework(atkui::Orthographic) {
  }

  virtual void setup() {
      N = 10;
      center = vec3(0.5 * width(), 0.5 * height(), 0);
      theta = 0.0f;
      pivotRadius = 200.0f;

      palette = { // colorful accents color palette https://www.color-hex.com/color-palette/82201
        vec3(206,46,175) / 255.0f,
        vec3(42,183,195) / 255.0f,
        vec3(129,30,148) / 255.0f,
        vec3(238,240,41) / 255.0f,
        vec3(68,197,48) / 255.0f
      };

      vec3 pivot = center + vec3(pivotRadius * cos(theta), pivotRadius * sin(theta), 0);

      for (int i = 0; i < N; i++) {
          collection.push_back(DualCircle(pivot, theta, pivotRadius, i % 2 == 0, palette[i % 5]));
          theta += 2 * pi<float>() / N;
          pivot = center + vec3(pivotRadius * cos(theta), pivotRadius * sin(theta), 0);
      }
  }

  virtual void scene() {
      thetaRate = 0.5f * dt();


      setColor(vec3(1));
      for (auto& dual : collection) {
          dual.updatePivot(center, thetaRate);
          dual.spin(dt());
          setColor(dual.color1);
          drawSphere(dual.position1, 30.0f);
          setColor(dual.color2);
          drawSphere(dual.position2, 30.0f);
      }
  }
private:
    int N; // this will be the number of 
    float pivotRadius; // two spheres will orbit a point around a pivot point on the circumference of an inivisble circle
    float theta;
    float thetaRate;
    vec3  center;
    std::vector<vec3> palette;
    std::vector<DualCircle> collection;
};

int main(int argc, char** argv) {
  Unique viewer;
  viewer.run();
  return 0;
}

