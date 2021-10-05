#include "atkui/framework.h"
#include "atkmath/constants.h"
#include "atkmath/vector3.h"
#include "atkmath/matrix3.h"
#include <vector>
#include <iostream>
using namespace glm;
using atkmath::Matrix3;
using atkmath::Vector3;

class Shuriken {
public:
	Shuriken(const vec3& centerPos) : centerPos(centerPos) {
		
		posTri[0] = centerPos - vec3(0, 35.0f, 0); // bottom of the triangle
		angTri[0] = Vector3(half_pi<float>(), 0, 0);

		posTri[1] = centerPos + vec3(35.0f, 0, 0);
		angTri[1] = Vector3(half_pi<float>(), half_pi<float>(), 0);

		posTri[2] = centerPos - vec3(35.0f, 0, 0);
		angTri[2] = Vector3(half_pi<float>(), -half_pi<float>(), 0);

		posTri[3] = centerPos - vec3(0, -35.0f, 0);
		angTri[3] = Vector3(half_pi<float>(), pi<float>(), 0);
	
	}

	vec3 centerPos;
	vec3 sizeCenterCube = vec3(35.0f);
	float sizeCenterCirc = 30.0f;
	Vector3 angCenter= Vector3(0, 0, 0);
	vec3 posTri[4];
	Vector3 angTri[4];
};

class Unique4 : public atkui::Framework {
public:
	Unique4() : atkui::Framework(atkui::Orthographic),
		mXAngle(half_pi<float>()), mYAngle(0), mZAngle(0) {
		anglesRad = Vector3(mXAngle, mYAngle, mZAngle);
	}

	virtual void setup() {
		shurikens.push_back(Shuriken(vec3(width()/2, height()/2, 0)));

	}

	virtual void scene() {
		setColor(vec3(1));

		for (auto& s : shurikens) {
			push();
			translate(s.centerPos);
			rotate(s.angCenter[0], vec3(1,0,0));
			rotate(s.angCenter[1], vec3(0, 1, 0));
			rotate(s.angCenter[2], vec3(0, 0, 1));
			drawCube(vec3(0), s.sizeCenterCube);
			setColor(vec3(0));
			drawSphere(vec3(0, 0, 100), s.sizeCenterCirc);
			pop();


			setColor(vec3(1));
			for (int i = 0; i < 4; i++) {
				push();
				translate(s.posTri[i]);
				rotate(s.angTri[i][0], vec3(1, 0, 0));
				rotate(s.angTri[i][1], vec3(0, 1, 0));
				rotate(s.angTri[i][2], vec3(0, 0, 1));
				drawCone(vec3(0), s.sizeCenterCube.x);
				pop();
			}
		}
	}

private:
	std::vector<Shuriken> shurikens;
	Vector3 anglesRad;
	float mXAngle;
	float mYAngle;
	float mZAngle;
};

int main(int argc, char** argv) {
	Unique4 viewer;
	viewer.run();
   return 0;
}

