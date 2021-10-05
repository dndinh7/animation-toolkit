#include "atkui/framework.h"
#include "atkmath/constants.h"
#include "atkmath/vector3.h"
#include "atkmath/matrix3.h"
#include <deque>
#include <iostream>
using namespace glm;
using atkmath::Matrix3;
using atkmath::Vector3;


// will have branches for us to throw shurikens at
// will randomly spawn in the second half of the screen (the right half)
// and will only go to a center height
class Branch {
public:
	Branch(float width, float height) {
		size = vec3(35.0f, agl::random(50.0f, 100.0f), 20); // will have varying heights
		maxHeight = agl::random(250, height - 100); // this is the height that the branch will go to

		pos = vec3(agl::random(width / 2, width), (maxHeight/2 + size.y/2) + (maxHeight/2 + size.y)*-cos(theta), 0); // hide this under the screen
	}

	void move(float dt) {
		if (theta < 2*pi<float>()) theta += 2 * pi<float>() / 2 * dt; // want the branch to go up and down in 2 seconds
		pos = vec3(pos.x, (maxHeight/2 + size.y/2) + (maxHeight/2 + size.y) * -cos(theta), 0);
	}

	vec3 getCenter() {
		return pos; // will return the center we seek to throw shuriken at
	}

	vec3 getSize() {
		return size;
	}

	// if off screen, theta= 360, then we should pop the object
	bool shouldDelete() {
		return theta > 2 * pi<float>();
	}


private:
	vec3 pos;
	vec3 size;
	float theta= 0;
	float maxHeight;

};

class Shuriken {
public:
	Shuriken(const vec3& centerPos) : centerPos(centerPos) {
		sizeCenterCirc = sizeCenterCube.x - 5.0f;
		initialPos = centerPos;

		thetaTri[0] = -half_pi<float>(); // bottom triangle
		thetaTri[1] = 0; // right triangle
		thetaTri[2] = pi<float>(); // left triangle
		thetaTri[3] = half_pi<float>();

		for (int i = 0; i < 4; i++) {
			posTri[i] = centerPos + vec3(sizeCenterCube.x*cos(thetaTri[i]), sizeCenterCube.x*sin(thetaTri[i]), 0);
			angTri[i] = Vector3(half_pi<float>(), thetaTri[i]+half_pi<float>(), 0);
		}
	
	}

	void move(const vec3& branchPos, float dt) {
		if (!hit) {
			if (time < 1) {
				time += dt;
			}
			else {
				time = 1;
				hit = true;
			}

			// want the shuriken to hit the branch at its apex (1 second)
			centerPos = (1 - time) * initialPos + time * (branchPos-vec3(sizeCenterCube.x, 0, 0));

			for (int i = 0; i < 4; i++) {
				posTri[i] = centerPos + vec3(sizeCenterCube.x * cos(thetaTri[i]), sizeCenterCube.x * sin(thetaTri[i]), 0);
				angTri[i] = Vector3(half_pi<float>(), thetaTri[i] + half_pi<float>(), 0);
			}
		}
		else {
			centerPos = branchPos - vec3(sizeCenterCube.x, 0, 0);

			for (int i = 0; i < 4; i++) {
				posTri[i] = centerPos + vec3(sizeCenterCube.x * cos(thetaTri[i]), sizeCenterCube.x * sin(thetaTri[i]), 0);
				angTri[i] = Vector3(half_pi<float>(), thetaTri[i] + half_pi<float>(), 0);
			}
		}
	}

	void spin(float rateAngle) {
		if (!hit) {
			// want to change the center as well
			angCenter += Vector3(0, 0, rateAngle);

			for (int i = 0; i < 4; i++) {
				thetaTri[i] += rateAngle;
				posTri[i] = centerPos + vec3(sizeCenterCube.x * cos(thetaTri[i]), sizeCenterCube.x * sin(thetaTri[i]), 0);
				angTri[i] = Vector3(half_pi<float>(), thetaTri[i] + half_pi<float>(), 0);
			}
		}
	}

	vec3 centerPos;
	vec3 initialPos;
	vec3 sizeCenterCube = vec3(22.5f); // change these two 
	float sizeCenterCirc;
	Vector3 angCenter= Vector3(0, 0, 0);
	vec3 posTri[4];
	float thetaTri[4];
	Vector3 angTri[4];
	bool hit= false; // when this is true, then we stop spinning
	float time= 0; 
};

class Unique4 : public atkui::Framework {
public:
	Unique4() : atkui::Framework(atkui::Orthographic) {
	}

	virtual void setup() {

		background(vec3(1));
		shurikens.push_back(Shuriken(vec3(0, height() / 2, 0)));
		branches.push_back(Branch(width(), height()));
		


	}

	virtual void scene() {
		timeSinceLastSpawn += dt();
		if (timeSinceLastSpawn >= spawnTime) {
			timeSinceLastSpawn = 0;
			spawnTime = 0.5f;
			shurikens.push_back(Shuriken(vec3(0, agl::random(0, height()), 0)));
			branches.push_back(Branch(width(), height()));
		}

		for (auto& b : branches) {
			push();
			setColor(vec3(.588, .294, 0));
			b.move(dt());
			translate(b.getCenter());
			drawCube(vec3(0, 0, 100), b.getSize());
			pop();
		}

		

		for (int i = 0; i < shurikens.size(); i++) {
			shurikens[i].spin(3.0*pi<float>()*dt()); // change the constant to change how fast the shurikens spins
			shurikens[i].spin(3.0*pi<float>()*dt()); // change the constant to change how fast the shurikens spins
			shurikens[i].move(branches[i].getCenter(), dt());
			push();
			translate(shurikens[i].centerPos);
			rotate(shurikens[i].angCenter[0], vec3(1,0,0));
			rotate(shurikens[i].angCenter[1], vec3(0, 1, 0));
			rotate(shurikens[i].angCenter[2], vec3(0, 0, 1));
			setColor(vec3(0));
			drawCube(vec3(0), shurikens[i].sizeCenterCube);
			setColor(vec3(1));
			drawSphere(vec3(0, 0, 50), shurikens[i].sizeCenterCirc);
			pop();


			setColor(vec3(0));
			for (int j = 0; j < 4; j++) {
				push();
				translate(shurikens[i].posTri[j]);
				rotate(shurikens[i].angTri[j][0], vec3(1, 0, 0));
				rotate(shurikens[i].angTri[j][1], vec3(0, 1, 0));
				rotate(shurikens[i].angTri[j][2], vec3(0, 0, 1));
				drawCone(vec3(0), shurikens[i].sizeCenterCube.x);
				pop();
			}
		}

		// there should always be at least one alive branch and shuriken, due to their construction
		if (branches[0].shouldDelete()) {
			branches.pop_front();
			shurikens.pop_front();
		}
	}

private:
	std::deque<Shuriken> shurikens;
	std::deque<Branch> branches;
	float timeSinceLastSpawn = 0;
	float spawnTime = 0.5f;
};

int main(int argc, char** argv) {
	Unique4 viewer;
	viewer.run();
   return 0;
}

