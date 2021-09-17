#include "atkui/framework.h"

using namespace glm;

class Dot {
public:
	Dot(float x, float y, float theta) : position{ vec3(x, y, 0) }, theta{theta} {
	}

	void circulate(float thetaRate, bool CW, float a) {
		theta = (CW) ? theta - thetaRate : theta + thetaRate; // if clockwise we subtract, CCW we add
		position = vec3(a * cos(theta), a * sin(theta), 0);
	}

	vec3 position;
private:
	
	float theta;
};

class CircleLayer {
public:
	CircleLayer(bool CW, float a, vec3 color) : CW{ CW }, a{ a }, color{ color } {
		float theta = 0;
		collection.reserve(36);
		for (int i = 0; i < 36; i++) {
			theta += 2*pi<float>() / 36; // divide the circle into 36ths
			collection.push_back(Dot(a * cos(theta), a * sin(theta), theta));
		}
	}

	void circulateAll(float thetaRate) {
		for (auto& dot : collection) {
			dot.circulate(thetaRate, CW, a);
		}
	}

	std::vector<Dot> collection; // this will be a collection of Dots
	vec3 color;
	
private:
	bool CW; // true if clockwise, false if counter-clockwise
	float a; // this will be the distance from the center of the screen
	

};

class Circles : public atkui::Framework
{
public:
   Circles() : atkui::Framework(atkui::Orthographic) {
   }

   virtual void setup() {
	   N = 18; // change N to change number of layers
	   a = 17.5f;
	   radius = 12.5f;
	   centerOffSet = vec3(0.5 * width(), 0.5 * height(), 0);
	   palette = { // fall sherbert color palette, https://www.color-hex.com/color-palette/112031
		   vec3(204,200,195) / 255.0f,
		   vec3(241,215,195) / 255.0f,
		   vec3(255,176,115) / 255.0f,
		   vec3(255,148,65) / 255.0f,
		   vec3(149,95,33) / 255.0f
	   };
	   concentricCircles.reserve(N);
	   for (int i = 0; i < N; i++) {
		   concentricCircles.push_back(CircleLayer(i % 2 == 0, a*i, palette[i%5]));
	   }
   }

   virtual void scene() {
	   thetaRate = 0.4f * dt();
	   for (auto& concentric : concentricCircles) {
		   concentric.circulateAll(thetaRate);
		   setColor(concentric.color);
		   for (auto& dot : concentric.collection) {
			   drawSphere(centerOffSet + dot.position, radius);
		   }
	   }
   }

private:
	std::vector<CircleLayer> concentricCircles;
	float radius; // this will be the radius for each individual sphere
	float thetaRate;
	float a;
	int N;
	vec3 centerOffSet;
	std::vector<vec3> palette;

};

int main(int argc, char** argv)
{
   Circles viewer;
   viewer.run();
   return 0;
}
