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
	   palette = { // Marielda-Winter in Hieron Color Palette: https://www.color-hex.com/color-palette/112186
		   vec3(255,192,239) / 255.0f,
		   vec3(255,178,122) / 255.0f,
		   vec3(254,229,188) / 255.0f,
		   vec3(116,167,230) / 255.0f,
		   vec3(85,79,165) / 255.0f
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
