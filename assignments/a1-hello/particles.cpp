#include "atkui/framework.h"

using namespace glm;

class Dot {
public:
	Dot(float widthScreen, float heightScreen, vec3 direction) : position{ vec3(agl::random(0, widthScreen), agl::random(0, heightScreen), agl::random(-5.0f, 5.0f)) }, // add random z positions to have random overlapping
		color{vec3(0.3f + agl::random(-0.3f, 0.3f), 0.7f + agl::random(-0.3f, 0.3f), 0.7 + agl::random(-0.3f, 0.3f))},
		velocity{direction*agl::random(50.0f, 100.0f)} {
	}

	vec3 position;
	vec3 color;
	vec3 velocity; // the reason this is a float is because it is a scale that we will multiply this with the unit vector direction
};


class Particles : public atkui::Framework
{
 public:
  Particles() : atkui::Framework(atkui::Orthographic) {
  }

  virtual void setup() {
	  count = 200;
	  direction = agl::randomUnitVector();
	  ifHorizVelPos = direction.x > 0;
	  ifVertVelPos  = direction.y > 0;
	  widthScreen = width();
	  heightScreen = height();
	  collection.reserve(count); // reserve space for 100 particles
	  for (int i = 0; i < count; i++) {
		  collection.push_back(Dot(widthScreen, heightScreen, direction));
	  }
	  
  }

  virtual void scene() {

	  float radius = 30.0f;
	  for (auto& dot : collection) {
		  if (ifHorizVelPos) {
			  dot.position.x = (dot.position.x > widthScreen) ? 0 : dot.position.x + dot.velocity.x * dt();
		  }
		  else {
			  dot.position.x = (dot.position.x < 0) ? widthScreen : dot.position.x + dot.velocity.x * dt();
		  }
		  if (ifVertVelPos) {

			  dot.position.y = (dot.position.y > heightScreen) ? 0 : dot.position.y + dot.velocity.y * dt();
		  }
		  else {
			  dot.position.y = (dot.position.y < 0) ? heightScreen : dot.position.y + dot.velocity.y * dt();
		  }
		  setColor(dot.color);
		  drawSphere(dot.position, radius);
	  }
  }

private:
	std::vector<Dot> collection;
	vec3 direction;
	bool ifHorizVelPos;
	bool ifVertVelPos;
	int count; // this is the variable for how many dots we want on the screen
	float widthScreen;
	float heightScreen;
};


int main(int argc, char** argv) {
  Particles viewer;
  viewer.run();
  return 0;
}
