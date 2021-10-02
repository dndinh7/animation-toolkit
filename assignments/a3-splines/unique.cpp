#include "atkui/framework.h"
#include "interpolator_hermite.h"
#include "interpolator_catmullrom.h"
#include "interpolator_linear.h"
#include "spline.h"
using namespace glm;

class Square {
public:
    Square(float screenWidth, float screenHeight, int numVecs) : splinePath(new Spline()), splineColor(new Spline()), splineSize(new Spline())
    {

        splineSize->addInterpolationType("Linear", &linear);
        splinePath->addInterpolationType("Hermite", &hermite);
        splineColor->addInterpolationType("CatmullRom", &catmullrom);

        splinePath->setInterpolationType("Hermite");
        splineSize->setInterpolationType("Linear");
        splineColor->setInterpolationType("CatmullRom");

        for (int i = 0; i < numVecs; i++) {
            splinePath->appendKey(timeCount, vec3(agl::random(10, screenWidth - 10), agl::random(10, screenHeight - 10), 0));
            splineColor->appendKey(timeCount, vec3(agl::random(), agl::random(), agl::random()));
            splineSize->appendKey(timeCount, vec3(agl::random(15.0f, 80.0f), agl::random(10.0f, 50.0f), 0));
            timeCount++;
        }
        splinePath->computeControlPoints();
        splineSize->computeControlPoints();
        splineColor->computeControlPoints();
    }

    ~Square() { // since Spline has to live longer than in the scope of Square, we must delete it after Square goes out of scope
        delete splinePath;
        delete splineColor;
        delete splineSize;
    }

    Spline* splinePath;
    Spline* splineColor;
    Spline* splineSize;
    InterpolatorLinear linear;
    InterpolatorCatmullRom catmullrom;
    InterpolatorHermite hermite;
    int timeCount= 0; 
};


class Unique3 : public atkui::Framework {
public:
  Unique3() : atkui::Framework(atkui::Orthographic) {
  
      for (int i = 0; i < numSquares; i++) {
          // need to dynamically allocated Square objects, so they do not go out of scope and deconstruct itself
          squareCollection.push_back(new Square(width(), height(), numPointsInSpline)); 
      }
  }

  ~Unique3() { // after we close the viewer, the object will deconstruct
      for (int i = 0; i < squareCollection.size(); i++) {
          Square* cube = squareCollection.at(i);
          delete cube;
      }
  }

  virtual void setup() {
  }

  virtual void scene() {
      t = (forward) ? t + dt() : t - dt();
      if (t <= 0) {
          forward = true;
      }
      else if (t >= time) {
          forward = false;
      }

      for (int i = 0; i < squareCollection.size(); i++) {
          setColor(squareCollection[i]->splineColor->getValue(t));
          drawCube(squareCollection[i]->splinePath->getValue(t), squareCollection[i]->splineSize->getValue(t));
      }
      
  }

  

private:
    int numPointsInSpline = 8; // change this to change the path and the time to traverse all the points
    int numSquares = 25;
    std::vector<Square *> squareCollection;
    float time= numPointsInSpline-1; // time I want it to take for each square to finish the interpolations of every point
    bool forward= true;
    float t= 0; // this will be the t value to go through curves
};

int main(int argc, char** argv) {
    Unique3 viewer;
    viewer.run();
    return 0;
}

