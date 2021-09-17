#include "atkui/framework.h"

using namespace glm;

class DualCircle {
public:
    DualCircle(vec3 pivot, float theta, float centerRadius, bool CW, vec3 color) : pivot{ pivot }, spinTheta{ 0.0f }, spinRadius{ 10.0f }, centerTheta(theta), centerRadius{ centerRadius }, CW{ CW }, color1 { color } {
        position1 = pivot + vec3(spinRadius * cos(spinTheta), spinRadius * sin(spinTheta), 0);
        position2 = pivot + vec3(spinRadius * cos(spinTheta + pi<float>()), spinRadius * sin(spinTheta + pi<float>()), 0);

        // gets the complementary color of the first color
        float color_range = max(max(color1.x, color1.y), color1.z) + min(min(color1.x, color1.y), color.z);
        color2 = vec3(color_range - color1.x, color_range - color1.y, color_range - color.z);
    }

    // this updates the pivot to move around in a circle path
    void updatePivot(vec3 center, float centerThetaRate) {
        centerTheta += centerThetaRate;
        pivot = center + vec3(centerRadius * cos(centerTheta), centerRadius * sin(centerTheta), 0);

        // change the pivot, then move the two circles with it
        position1 = pivot + vec3(spinRadius * cos(spinTheta), spinRadius * sin(spinTheta), 0);
        position2 = pivot + vec3(spinRadius * cos(spinTheta + pi<float>()), spinRadius * sin(spinTheta + pi<float>()), 0);
    }

    // this spins the two circles around the pivot
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

class CircleLayer {
public:
    CircleLayer(int numCircles, float pivotRadius, vec3 center, std::vector<vec3> palette) : numCircles{ numCircles }, 
        pivotRadius{ pivotRadius }, center{ center } {
        float theta= 0.0f;
        vec3 pivot = center + vec3(pivotRadius * cos(theta), pivotRadius * sin(theta), 0);
        for (int i= 0; i < numCircles; i++) {
            level.push_back(DualCircle(pivot, theta, pivotRadius, i % 2 == 0, palette[i % 5])); // add created dot to layer
            theta += 2 * pi<float>() / numCircles;
            pivot = center + vec3(pivotRadius * cos(theta), pivotRadius * sin(theta), 0);
        }

    }

    std::vector<DualCircle> level;
private:
    int numCircles; // number of circles in the layer
    float pivotRadius; // distance from the center to the pivot circle
    vec3 center;
    
};

class Unique : public atkui::Framework {
 public:
  Unique() : atkui::Framework(atkui::Orthographic) {
  }

  virtual void setup() {
      N = 4; // change this number to change the number of layers
      center = vec3(0.5 * width(), 0.5 * height(), 0);
      pivotRadiusInterval = 0.0f;

      palette = { // Autumn heat Color Palette: https://www.color-hex.com/color-palette/112780
        vec3(244,234,4) / 255.0f,
        vec3(204,19,19) / 255.0f,
        vec3(122,4,4) / 255.0f,
        vec3(229,74,7) / 255.0f,
        vec3(246,120,65) / 255.0f
      };

      for (int i = 1; i < N+1; i++) {
          pivotRadiusInterval += 60.0f;
          collection.push_back(CircleLayer(i * 5, pivotRadiusInterval, center, palette)); // change what i is being multiplied by to change the number of circles in the layer

      }
  }

  virtual void scene() {
      thetaRate = 0.5f * dt();

      for (auto& layer : collection) {
          for (auto& dual : layer.level) {
              dual.updatePivot(center, thetaRate);
              dual.spin(dt());
              setColor(dual.color1);
              drawSphere(dual.position1, 15.0f);
              setColor(dual.color2);
              drawSphere(dual.position2, 15.0f);
          }
      }
  }
private:
    int N; // this will be the number of 
    float pivotRadiusInterval; // two spheres will orbit a point around a pivot point on the circumference of an inivisble circle
    float thetaRate; // this will change the speed at which the pivots revolve
    vec3  center; 
    std::vector<vec3> palette; // this will serve as the color palette
    std::vector<CircleLayer> collection;
};

int main(int argc, char** argv) {
  Unique viewer;
  viewer.run();
  return 0;
}

