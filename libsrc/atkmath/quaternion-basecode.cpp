#include "atkmath/quaternion.h"
#include "atkmath/matrix3.h"
#include "atkmath/vector3.h"

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
	return Matrix3::Identity;
}

void Quaternion::fromMatrix(const Matrix3& rot)
{
	// TODO
}

}