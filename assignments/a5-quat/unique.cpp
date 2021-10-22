#include "atkui/framework.h"
#include "atkmath/constants.h"
#include "atkmath/vector3.h"
#include "atkmath/matrix3.h"
#include "atkmath/quaternion.h"
#include <string>
#include <deque>
#include <iostream>
using namespace glm;
using atkmath::Matrix3;
using atkmath::Vector3;

class Electrons {
public:
	Electrons(int numElectrons, float radius, const vec3& centerPos) : num(numElectrons), radius(radius), center(centerPos) {
		numRings = (numElectrons > 2) ? 2 : 1; // we only consider the first 10 atoms
		
		if (numRings == 2) {
			angInterval1 = pi<float>(); // the first ring only has 2 electrons
			angInterval2 = 2 * pi<float>() / (numElectrons - 2);

			for (int i = 0; i < 2; i++) {
				posRing1.push_back(vec3(radius * cos(angInterval1 * i), radius * sin(angInterval1 * i), 0) + centerPos);
			}

			for (int i = 0; i < numElectrons - 2; i++) {
				posRing2.push_back(vec3(2* radius * cos(angInterval2 * i), 2 * radius * sin(angInterval2 * i), 0) + centerPos);
			}
		}
		else {
			angInterval1 = 2 * pi<float>() / (numElectrons);
			angInterval2 = 0;
			for (int i = 0; i < numElectrons; i++) {
				posRing1.push_back(vec3(radius*cos(angInterval1*i), radius*sin(angInterval1*i), 0) + centerPos);
			}
		}

	}

	void orbit(float theta) {
		for (int i = 0; i < posRing1.size(); i++) {
			posRing1[i] = vec3(radius * cos(angInterval1 * i + theta * 2), radius * sin(angInterval1 * i + theta * 2), 0) + center;
		}

		for (int i = 0; i < posRing2.size(); i++) {
			posRing2[i] = vec3(radius * 2 * cos(angInterval2 * i + theta * 2), radius * sin(angInterval2 * i + theta * 2), 0) + center;
		}
	}

	int num;
	int numRings;
	std::vector<vec3> posRing1;
	std::vector<vec3> posRing2;
	float angInterval1;
	float angInterval2;
	float radius;
	vec3 center;
};

class Atom {
public:
	Atom(int atomicNum, const vec3& centerPos) : atomicNum(atomicNum), centerPos(centerPos), e(atomicNum, 100.0f, centerPos) {
		name();
	}

	void name() {
		switch (atomicNum) {
		case 1:
			element = "Hydrogen";
			break;
		case 2:
			element = "Helium";
			break;
		case 3:
			element = "Lithium";
			break;
		case 4:
			element = "Beryllium";
			break;
		case 5:
			element = "Boron";
			break;
		case 6:
			element = "Carbon";
			break;
		case 7:
			element = "Nitrogen";
			break;
		case 8:
			element = "Oxygen";
			break;
		case 9:
			element = "Flourine";
			break;
		case 10:
			element = "Neon";
			break;
		}
	}

	void changeAtom(int atomicNumNew) {
		atomicNum = atomicNumNew;
		name();
		e = Electrons(atomicNum, 100.0f, centerPos);
	}

	void updateCenterPos(const vec3& newPos) {
		centerPos = newPos;
		e.center = newPos;
	}


	Electrons e;
	int atomicNum;
	std::string element;
	float radiusInterval= 75.0f;
	vec3 centerPos;
};


class Unique5 : public atkui::Framework {
public:
	Unique5() : atkui::Framework(atkui::Orthographic), atom(10, vec3(width()/2, height()/2, 0)) {
		
	}

	virtual void setup() {
		newAxes();

		_mouseX = width() / 2;
		_mouseY = height() / 2;
	}

	virtual void scene() {

		// Query if mouse is down, then if is, we can get the mouse position and call mouseMove
		if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)) {
			vec2 mousePos = mousePosition();
			mouseMove(mousePos.x, mousePos.y);
			atom.updateCenterPos(vec3(_mouseX, _mouseY, 0));
		}

		theta += thetaRate * dt();
		push();
		setColor(vec3(1, 0, 0));
		translate(atom.centerPos);
		drawSphere(vec3(0), 50.0f);
		pop();

		atom.e.orbit(theta);
		for (int i = 0; i < atom.e.posRing1.size(); i++) {
			push();
			setColor(vec3(1, 1, 0));

			translate(vec3(_mouseX, _mouseY, 0));
			rotate(theta, rAxis1);
			
			translate(atom.e.posRing1[i]);
			
			drawSphere(vec3(-_mouseX, -_mouseY, 0), 30.0f);
			
			pop();
		}

		for (int i = 0; i < atom.e.posRing2.size(); i++) {
			push();
			setColor(vec3(1, 1, 0));

			translate(vec3(_mouseX, _mouseY, 0));
			rotate(theta, rAxis2);
			translate(atom.e.posRing2[i]);

			drawSphere(vec3(-_mouseX, -_mouseY, 0), 30.0f);
			pop();
		}

		setColor(vec3(1));
		translate(vec3(atom.centerPos.x, atom.centerPos.y, 500));
		
		drawText(atom.element + " Atom", 10, 25);

	}

	void keyUp(int key, int mods) {

		switch (key) {
		case GLFW_KEY_1:
			if (atom.element != "Hydrogen") {
				atom.changeAtom(1);
			}
			break;
		case GLFW_KEY_2:
			if (atom.element != "Helium") {
				atom.changeAtom(2);
			}
			break;
		case GLFW_KEY_3:
			if (atom.element != "Lithium") {
				atom.changeAtom(3);
			}
			break;
		case GLFW_KEY_4:
			if (atom.element != "Beryllium") {
				atom.changeAtom(4);
			}
			break;
		case GLFW_KEY_5:
			if (atom.element != "Boron") {
				atom.changeAtom(5);
			}
			break;
		case GLFW_KEY_6:
			if (atom.element != "Carbon") {
				atom.changeAtom(6);
			}
			break;
		case GLFW_KEY_7:
			if (atom.element != "Nitrogen") {
				atom.changeAtom(7);
			}
			break;
		case GLFW_KEY_8:
			if (atom.element != "Oxygen") {
				atom.changeAtom(8);
			}
			break;
		case GLFW_KEY_9:
			if (atom.element != "Fluorine") {
				atom.changeAtom(9);
			}
			break;
		case GLFW_KEY_0:
			if (atom.element != "Neon") {
				atom.changeAtom(10);
			}
			break;
		}
		newAxes();
	}

	void mouseMove(int x, int y) {
		_mouseX = x;
		_mouseY = height() - y;
	}

	void newAxes() {
		Vector3 a1 = Vector3(agl::random(), agl::random(), 0);
		a1.normalize();
		rAxis1 = vec3(a1.x(), a1.y(), 0);
		a1 = Vector3(agl::random(), agl::random(), 0);
		a1.normalize();
		rAxis2 = vec3(a1.x(), a1.y(), 0);
	}


	float _mouseX;
	float _mouseY;
	Atom atom;
	vec3 rAxis1;
	vec3 rAxis2;
	float theta = 0;
	float thetaRate = 2.5f;
};

int main(int argc, char** argv) {
	Unique5 viewer;
	viewer.run();
   return 0;
}

