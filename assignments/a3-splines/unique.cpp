#include "atkui/framework.h"
#include "interpolator_hermite.h"
#include "interpolator_catmullrom.h"
#include "interpolator_linear.h"
#include "spline.h"
using namespace glm;

class Square {
public:
    Square(float screenWidth, float screenHeight, int numVecs)
    {
        splinePath = Spline();
        splineColor = Spline();
        splineSize = Spline();

        splineSize.addInterpolationType("Linear", &linear);
        splinePath.addInterpolationType("Hermite", &hermite);
        splineColor.addInterpolationType("CatmullRom", &catmullrom);

        for (int i = 0; i < numVecs; i++) {
            splinePath.appendKey(timeCount, vec3(agl::random(10, screenWidth - 10), agl::random(10, screenHeight - 10), 0));
            splineColor.appendKey(timeCount, vec3(agl::random(), agl::random(), agl::random()));
            splineSize.appendKey(timeCount, vec3(agl::random(10.0f, 50.0f), agl::random(10.0f, 50.0f), 0));
            timeCount++;
        }

        std::cout << splineColor.getValue(0) << std::endl;
        std::cout << splineSize.getValue(0) << std::endl;
        std::cout << splinePath.getValue(0) << std::endl;
        

        std::cout << splineColor.getValue(0) << std::endl;
        std::cout << splinePath.getValue(0) << std::endl;
        std::cout << splineSize.getValue(0) << std::endl;

    }

    Spline splinePath;
    Spline splineColor;
    Spline splineSize;
    InterpolatorLinear linear;
    InterpolatorCatmullRom catmullrom;
    InterpolatorHermite hermite;
    int timeCount= 0; 
};


class Unique3 : public atkui::Framework {
public:
  Unique3() : atkui::Framework(atkui::Orthographic) {
  
      for (int i = 0; i < numSquares; i++) {
          squareCollection.push_back(Square(width(), height(), numPointsInSpline));
      }
  }

  virtual void setup() {
      std::cout << squareCollection[0].splineColor.getValue(0) << std::endl;
  }

  virtual void scene() {
      /*std::cout << squareCollection[0].splineColor.getValue(0) << std::endl;
      std::cout << squareCollection[0].splinePath.getValue(0) << std::endl;
      std::cout << squareCollection[0].splineSize.getValue(0) << std::endl;*/
      float t = fmod(elapsedTime(), 5.0f);
      for (int i = 0; i < squareCollection.size(); i++) {
          setColor(squareCollection[i].splineColor.getValue(0));
          //drawCube(squareCollection[i].splinePath.getValue(t), squareCollection[i].splineSize.getValue(t));
      }
      
  }

  

private:
    int numPointsInSpline = 5;
    int numSquares = 1;
    int timeCount = 0;
    std::vector<Square> squareCollection;
    float time= 5; // time I want it to take for each square to finish the interpolations of every point

};

int main(int argc, char** argv) {
    Unique3 viewer;
    viewer.run();
    return 0;
}

