#include "atkmath/quaternion.h"
#include "atkmath/matrix3.h"
#include "atkmath/vector3.h"
#include <iostream>

namespace atkmath {

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, double t)
{
	// TODO
	return Quaternion(1,0,0,0);
}

void Quaternion::toAxisAngle(Vector3& axis, double& angleRad) const
{
	// TODO
	angleRad = acos(mW)*2;

	axis = Vector3(mX / sin(angleRad * 0.5), mY / sin(angleRad * 0.5), mZ / sin(angleRad * 0.5));


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
	float vx_2 = 0.25 * (rot.m11 - rot.m22 - rot.m33 + 1);
	float vy_2 = 0.25 * (-rot.m11 + rot.m22 - rot.m33 + 1);
	float vz_2 = 0.25 * (-rot.m11 - rot.m22 + rot.m33 + 1);
	float w_2 = 0.25 * (rot.m11 + rot.m22 + rot.m33 + 1);

	// choose the biggest of the squared terms
	float biggest = std::max(std::max(std::max(vx_2, vy_2), vz_2), w_2);

	float vx;
	float vy;
	float vz;
	float w;

	if (biggest == vx_2) {
		vx = sqrt(vx_2);
		w = (rot.m32 - rot.m23) / (4 * vx);
		vy = (rot.m21 - rot.m12) / (4 * vx);
		vz = (rot.m13 + rot.m31) / (4 * vx);

	}
	else if (biggest == vy_2) {
		vy = sqrt(vy_2);
		w = (rot.m13 - rot.m31) / (4 * vy);
		vx = (rot.m21 - rot.m12) / (4 * vy);
		vz = (rot.m23 + rot.m32) / (4 * vy);
	}
	else if (biggest == vz_2) {
		vz = sqrt(vz_2);
		vy = (rot.m23 + rot.m32) / (4 * vz);
		w = (rot.m21 - rot.m12) / (4 * vz);
		vx = (rot.m13 - rot.m31) / (4 * vz);
	}
	else { // this is the case where w_2 is the largest
		w = sqrt(w_2);
		vz = (rot.m21 - rot.m12) / (4 * w);
		vy = (rot.m13 - rot.m31) / (4 * w);
		vx = (rot.m32 - rot.m23) / (4 * w);
	}

	mX = vx;
	mY = vy;
	mZ = vz;
	mW = w;
	normalize();
}


}