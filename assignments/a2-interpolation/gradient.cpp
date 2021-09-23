#include <iostream>
#include "atkui/framework.h"
using namespace glm;

class Squares {
public:
	Squares(vec3 pos, vec3 color) : position{ pos }, color{ color } {


	}

	vec3 position;
	vec3 color;
};

class Gradient : public atkui::Framework {
 public:
  Gradient() : atkui::Framework(atkui::Orthographic) {
	  N = 10;

	  nwColor = vec3(1, 1, 0);
	  neColor = vec3(0, 1, 1);
	  swColor = vec3(1, 0, 0);
	  seColor = vec3(1, 0, 1);

	  cubeSize = vec3(width() / N, height() / N, 0);

	  cubes.reserve(N * N);

	  float px = 0;
	  float py = 0;
	  for (float i = 0; i < N; i++) {
		  px = (i / N) * (width()); // get x coordinate of the cube in ith row
		  for (float j= 0; j < N; j++) {
			  

			  float tx = px / width();
			  vec3 Cx0 = nwColor * (1 - tx) + neColor * tx;
			  vec3 Cx1 = swColor * (1 - tx) + seColor * tx;

			  py = (j / N) * (height()); // get y coordinate of the cube in the jth col
			  float ty = py / height();

			  vec3 finalColor = Cx0 * (1 - ty) + Cx1 * ty;
			  
			  cubes.push_back(Squares(vec3(px, py, 0), finalColor));

		  }

	  }
  }

  virtual void scene() {
	  for (const auto& square : cubes) {
		  setColor(square.color);
		  drawCube(square.position+cubeSize*0.5f, cubeSize); // offset positions by side length/2, so the square do not go off screen
	  }
  }
private:
	int N;

	vec3 cubeSize;
	vec3 nwColor;
	vec3 neColor;
	vec3 swColor;
	vec3 seColor;
	std::vector<Squares> cubes;
};

int main(int argc, char **argv) {
  Gradient viewer;
  viewer.run();
}
