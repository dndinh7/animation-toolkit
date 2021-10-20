#include "atkmath/matrix3.h"
#include "atkmath/quaternion.h"
#include <iostream>
#include <algorithm>
#include <math.h>

namespace atkmath {

Matrix3 xMatrix(const Vector3& angleRad) {
	float x = angleRad.mX;
	Matrix3 Rx(
		1, 0     ,  0,
		0, cos(x), -sin(x),
		0, sin(x),  cos(x));
	return Rx;
}

Matrix3 yMatrix(const Vector3& angleRad) {
	float y = angleRad.mY;
	Matrix3 Ry(
		cos(y), 0, sin(y),
		0, 1, 0,
		-sin(y), 0, cos(y));
	return Ry;
}

Matrix3 zMatrix(const Vector3& angleRad) {
	float z = angleRad.mZ;
	Matrix3 Rz(
		cos(z), -sin(z), 0,
		sin(z), cos(z) , 0,
		0     , 0      , 1);
	return Rz;
}

Vector3 Matrix3::toEulerAnglesXYZ() const
{
	float x;
	float y = asin(mM[0][2]);
	float z;
	if (mM[0][2] == 1) { // this means Y has a rotation of 90
		z = 0;
		x = atan2(mM[1][0], mM[1][1]);
	}
	else if (mM[0][2] == -1) { // Y has a rotation of -90
		z = 0;
		x = atan2(mM[2][1], mM[2][0]);
	}
	else {
		z = atan2(-mM[0][1], mM[0][0]);
		x = atan2(-mM[1][2], mM[2][2]);
	}
   return Vector3(x, y, z);
}

Vector3 Matrix3::toEulerAnglesXZY() const
{
	float x;
	float y;
	float z = asin(-mM[0][1]);
	if (mM[0][1] == 1) { // this means Z has a rotation of -90
		y = 0;
		x = atan2(-mM[2][0], -mM[1][0]);
	}
	else if (mM[0][1] == -1) { // this means Z has a rotation of 90
		y = 0;
		x = atan2(mM[2][0], mM[1][0]);
	}
	else {
		y = atan2(mM[0][2], mM[0][0]);
		x = atan2(mM[2][1], mM[1][1]);
	}
   return Vector3(x, y, z);
}

Vector3 Matrix3::toEulerAnglesYXZ() const
{
	float x = asin(-mM[1][2]);
	float y;
	float z;
	if (mM[1][2] == 1) { // this means X has a rotation of -90
		z = 0;
		y = atan2(-mM[2][0], -mM[2][1]);
	}
	else if (mM[1][2] == -1) { // this means X has a rotation of 90
		z = 0;
		y = atan2(mM[0][1], mM[0][0]);
	}
	else {
		y = atan2(mM[0][2], mM[2][2]);
		z = atan2(mM[1][0], mM[1][1]);
	}

	return Vector3(x, y, z);
}

Vector3 Matrix3::toEulerAnglesYZX() const
{
	float x;
	float y;
	float z = asin(mM[1][0]);
	if (mM[1][0] == 1 || mM[1][0] == -1) { // this means Z has a rotation of 90 or -90
		y = 0;
		x = atan2(mM[2][1], mM[2][2]);
	} 
	else {
		y = atan2(-mM[2][0], mM[0][0]);
		x = atan2(-mM[1][2], mM[1][1]);
	}
	return Vector3(x, y, z);
}

Vector3 Matrix3::toEulerAnglesZXY() const
{
	float x = asin(mM[2][1]);
	float y;
	float z;
	if (mM[2][1] == 1 || mM[2][1] == -1) { // this means X has a rotation of 90 or -90
		z = 0;
		y = atan2(mM[0][2], mM[0][0]);
	}
	else {
		z = atan2(-mM[0][1], mM[1][1]);
		y = atan2(-mM[2][0], mM[2][2]);
	}
	return Vector3(x, y, z);
}

Vector3 Matrix3::toEulerAnglesZYX() const
{
	float x;
	float y = asin(-mM[2][0]);
	float z;
	// indicates that the matrix has a Y rotation of -90
	if (mM[2][0] == 1) {
		z = 0; // set z to an arbitrary value, use 0 for ease
		x = atan2(-mM[0][1], -mM[0][2]); // if the matrix has a negative plug them in?
	} else if (mM[2][0] == -1) { // indicates that the matrix has a Y rotation of 90
		z = 0;
		x = atan2(mM[0][1], mM[0][2]);
	}
	else {
		z = atan2(mM[1][0], mM[0][0]);
		x = atan2(mM[2][1], mM[2][2]);
	}
   return Vector3(x, y, z);
}

void Matrix3::fromEulerAnglesXYZ(const Vector3& angleRad)
{
   *this = xMatrix(angleRad) * yMatrix(angleRad) * zMatrix(angleRad);
}

void Matrix3::fromEulerAnglesXZY(const Vector3& angleRad)
{
   *this = xMatrix(angleRad) * zMatrix(angleRad) * yMatrix(angleRad);
}

void Matrix3::fromEulerAnglesYXZ(const Vector3& angleRad)
{
   *this = yMatrix(angleRad) * xMatrix(angleRad) * zMatrix(angleRad);
}

void Matrix3::fromEulerAnglesYZX(const Vector3& angleRad)
{
   *this = yMatrix(angleRad) * zMatrix(angleRad) * xMatrix(angleRad);
}

void Matrix3::fromEulerAnglesZXY(const Vector3& angleRad)
{
   *this = zMatrix(angleRad) * xMatrix(angleRad) * yMatrix(angleRad);
}

void Matrix3::fromEulerAnglesZYX(const Vector3& angleRad)
{

   *this = zMatrix(angleRad) * yMatrix(angleRad) * xMatrix(angleRad);
}

void Matrix3::toAxisAngle(Vector3& axis, double& angleRad) const
{
	// TODO

	Quaternion q;
	q.fromMatrix(*this);
	q.toAxisAngle(axis, angleRad);

}

void Matrix3::fromAxisAngle(const Vector3& axis, double angleRad)
{
   // TODO

	Quaternion q;
	q.fromAxisAngle(axis, angleRad);
	
	*this= q.toMatrix();

}

}