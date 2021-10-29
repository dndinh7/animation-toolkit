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


class Unique6 : public atkui::Framework {
public:
	Unique6() : atkui::Framework(atkui::Orthographic) {

	}

	virtual void setup() {
		std::cout << "hell" << std::endl;
	}

	virtual void scene() {

	}

};

int main(int argc, char** argv) {
	Unique6 viewer;
	viewer.run();
	return 0;
}

