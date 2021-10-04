#include "atkmath/matrix3.h"
#include "atkmath/quaternion.h"

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
   return Vector3();
}

Vector3 Matrix3::toEulerAnglesXZY() const
{
   return Vector3();
}

Vector3 Matrix3::toEulerAnglesYXZ() const
{
   return Vector3();
}

Vector3 Matrix3::toEulerAnglesYZX() const
{
   return Vector3();
}

Vector3 Matrix3::toEulerAnglesZXY() const
{
   return Vector3();
}

Vector3 Matrix3::toEulerAnglesZYX() const
{

   return Vector3();
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
}

void Matrix3::fromAxisAngle(const Vector3& axis, double angleRad)
{
   // TODO
   *this = Identity;
}

}