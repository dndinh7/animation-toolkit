#include "atkmath/quaternion.h"
#include "atkmath/matrix3.h"
#include "atkmath/vector3.h"
#include <iostream>
#include <algorithm>
#include <cmath>


using namespace std;


namespace atkmath {

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, double t)
{
	// TODO
	double prod1 = Quaternion::Dot(q0, q1);
	double angleRad;

	// angle distance is big, so use q2
	if (prod1 < 0) {
		Quaternion q2 = -q1;

		prod1 = Quaternion::Dot(q0, q2);

		// clamp
		if (prod1 > 1) {
			prod1 = 1;
		}

		angleRad = acos(prod1);

		if (angleRad == 0.0) return q0;

		return sin(angleRad * (1 - t)) * q0 / sin(angleRad) + sin(angleRad * t) * q2 / sin(angleRad);
	}

	// clamp
	if (prod1 > 1) {
		prod1 = 1;
	}

	angleRad = acos(prod1);

	if (angleRad == 0.0) return q0;

	return sin(angleRad * (1 - t)) * q0 / sin(angleRad) + sin(angleRad * t) * q1 / sin(angleRad);
}

void Quaternion::toAxisAngle(Vector3& axis, double& angleRad) const
{
	// TODO

	angleRad = 2 * acos(mW);
	float mag_v = sqrt(mX*mX + mY*mY + mZ*mZ);
	// this condition equates to no rotation or an identity
	if (mag_v == 0) {
		axis = Vector3(1, 0, 0);
	}
	else {
		axis = Vector3(mX / mag_v, mY / mag_v, mZ / mag_v);
		axis.normalize();
	}
	
}

void Quaternion::fromAxisAngle (const Vector3& axis, double angleRad)
{
	// TODO
	mX = (double) sin(angleRad * 0.5) * axis.x();
	mY = (double) sin(angleRad * 0.5) * axis.y();
	mZ = (double) sin(angleRad * 0.5) * axis.z();
	mW = (double) cos(angleRad * 0.5);
	normalize();
}

Matrix3 Quaternion::toMatrix () const
{
	// TODO

	float a11 = 1 - 2 * (mY * mY + mZ * mZ);
	float a12 = 2 * (mX * mY - mW * mZ);
	float a13 = 2 * (mX * mZ + mW * mY);
	float a21 = 2 * (mX * mY + mW * mZ);
	float a22 = 1 - 2 * (mX * mX + mZ * mZ);
	float a23 = 2 * (mY * mZ - mW * mX);
	float a31 = 2 * (mX * mZ - mW * mY);
	float a32 = 2 * (mY * mZ + mW * mX);
	float a33 = 1 - 2 * (mY * mY + mX * mX);

	return Matrix3(a11, a12, a13,
		a21, a22, a23,
		a31, a32, a33);
}

void Quaternion::fromMatrix(const Matrix3& rot)
{
	// TODO
	float vx_2 = 0.25 * (rot[0][0] - rot[1][1] - rot[2][2] + 1);
	float vy_2 = 0.25 * (-rot[0][0] + rot[1][1] - rot[2][2] + 1);
	float vz_2 = 0.25 * (-rot[0][0] - rot[1][1] + rot[2][2] + 1);
	float w_2 = 0.25 * (rot[0][0] + rot[1][1] + rot[2][2] + 1);

	// choose the biggest of the squared terms
	float biggest = std::max(std::max(std::max(vx_2, vy_2), vz_2), w_2);

	float vx;
	float vy;
	float vz;
	float w;

	if (biggest == vx_2) {
		vx = sqrt(vx_2);
		w = (rot[2][1] - rot[1][2]) / (4 * vx);
		vy = (rot[0][1] + rot[1][0]) / (4 * vx);
		vz = (rot[0][2] + rot[2][0]) / (4 * vx);

	}
	else if (biggest == vy_2) {
		vy = sqrt(vy_2);
		w = (rot[0][2] - rot[2][0]) / (4 * vy);
		vx = (rot[0][1] + rot[1][0]) / (4 * vy);
		vz = (rot[1][2] + rot[2][1]) / (4 * vy);
	}
	else if (biggest == vz_2) {
		vz = sqrt(vz_2);
		vy = (rot[1][2] + rot[2][1]) / (4 * vz);
		w = (rot[1][0] - rot[0][1]) / (4 * vz);
		vx = (rot[0][2] + rot[2][0]) / (4 * vz);
	}
	else { // this is the case where w_2 is the largest
		w = sqrt(w_2);
		vz = (rot[1][0] - rot[0][1]) / (4 * w);
		vy = (rot[0][2] - rot[2][0]) / (4 * w);
		vx = (rot[2][1] - rot[1][2]) / (4 * w);
	}

	mX = vx;
	mY = vy;
	mZ = vz;
	mW = w;
	normalize();
}


}