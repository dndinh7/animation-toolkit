#include "atkui/framework.h"
using namespace glm;

class Screensaver : public atkui::Framework {
 public:
  Screensaver() : atkui::Framework(atkui::Orthographic) {

  }

  void setup() {
	  curve1 = createCurve();
	  curve2 = createCurve();
	  current = curve1;

	  color1 = vec3(agl::random(), agl::random(), agl::random());
	  color2 = vec3(agl::random(), agl::random(), agl::random());
	  colorCur = color1;

	  T = 0.1f;

  }

  void scene() {
	  
	  t += dt();

	  if (t > 1) {
		  t = 0;
		  curve1 = current;
		  color1 = colorCur;
		  curve2 = createCurve();
		  color2 = vec3(agl::random(), agl::random(), agl::random());
	  }

	  curveInterpolate(current, curve1, curve2,  t);
	  colorInterpolate(colorCur, color1, color2, t);

	  timer += dt();
	  if (timer > T) {
		  timer = 0;
		  trails.push_back(current);
		  trailColor.push_back(colorCur);
	  }
	  if (trails.size() > maxTrails) {
		  trails.pop_front();
	  }


	  std::list<vec3>::iterator colorIT = trailColor.begin();
	  std::list<std::vector<vec3>>::iterator trailIT = trails.begin();
	  // Reference: https://stackoverflow.com/questions/19933537/iterate-through-two-stdlists-simultaneously top answer
	  // I looked up how to iterate through two lists at the same time
	  for (; colorIT != trailColor.cend() && trailIT != trails.cend(); ++colorIT, ++trailIT) {
		  setColor(*colorIT);
		  
		  std::vector<vec3>::iterator curve_it;
		  std::vector<vec3>& trailCurve = *trailIT;

		  // Reference: https://www.geeksforgeeks.org/nested-list-in-c-stl/
		  // How to iterate through list inside a list
		  for (curve_it = trailCurve.begin(); curve_it != trailCurve.end() - 1; curve_it++) {
			  drawLine(*curve_it, *(curve_it + 1));
		  }
	  }


	  for (int i = 0; i < current.size() - 1; i++) {
		  setColor(color1);
		  drawLine(curve1[i], curve1[i+1]);
		  setColor(colorCur);
		  drawLine(current[i], current[i + 1]);
		  setColor(color2);
		  drawLine(curve2[i], curve2[i + 1]);

	  }
  }


  // Each curve should have the same number of points in each of them
  void curveInterpolate(std::vector<vec3>& current, const std::vector<vec3>& curve1, const std::vector<vec3>& curve2, float t) {
	  for (int i = 0; i < current.size(); i++) {
		  current[i] = (1 - t) * curve1[i] + t * curve2[i];
	  }

  }

  void colorInterpolate(vec3& curColor, const vec3& color1, const vec3& color2, float t) {
	  curColor = (1 - t) * color1 + t * color2;
  }

  std::vector<vec3> createCurve() {
	  int N= 40; // number of intermediate points
	  std::vector<vec3> points;

	  // offset by 25.0 so we don't get too close to the border
	  vec3 B0 = vec3(agl::random(25.0f, width() - 25.0f), agl::random(25.0f, height() - 25.0f), 0);
	  vec3 B1 = vec3(agl::random(25.0f, width() - 25.0f), agl::random(25.0f, height() - 25.0f), 0);
	  vec3 B2 = vec3(agl::random(25.0f, width() - 25.0f), agl::random(25.0f, height() - 25.0f), 0);
	  vec3 B3 = vec3(agl::random(25.0f, width() - 25.0f), agl::random(25.0f, height() - 25.0f), 0);

	  float interval = 1.0f / (N + 1);
	  points.push_back(B0);
	  for (int i = 1; i <= N; i++) {
		  float t = interval * i;
		  points.push_back(LERPALL(B0, B1, B2, B3, t));
	  }
	  points.push_back(B3);

	  return points;
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
	std::vector<vec3> curve1;
	std::vector<vec3> curve2;
	std::vector<vec3> current;
	vec3 color1;
	vec3 color2;
	vec3 colorCur;
	float t = 00.f;
	std::list<std::vector<vec3>> trails;
	std::list<vec3> trailColor;
	float T; // T seconds in which we save the current curve into trails
	float timer = 0.0f; // timer for the trails interval
	int maxTrails= 50;
};

int main(int argc, char** argv) {
  Screensaver viewer;
  viewer.run();
}

