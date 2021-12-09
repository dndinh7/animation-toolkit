#include "steerable.h"
#include "behaviors.h"

using namespace glm;
using namespace atk;

float ASteerable::kVelKv = 5.0; 
float ASteerable::kOriKv = 16.0;  
float ASteerable::kOriKp = 64.0;

// Given a desired velocity, veld, and dt, compute a transform holding 
// the new orientation and change in position
//
// rotation control: tau = I * (-Kv1 * thetaDot + Kp1 * (thetaD - theta))
// translation control: f = m * Kv0 * (vd - v)
void ASteerable::senseControlAct(const vec3& veld, float dt)
{
	enum { POS, ORI, VEL, AVEL };
	// Compute _vd and _thetad
	_vd = length(veld);
	_thetad = atan2(veld.x, veld.z);

	// compute _force and _torque
	_force = _mass * kVelKv * (_vd - _state[VEL]);

	_torque = _inertia * (-kOriKv * _state[AVEL] + kOriKp * (_thetad - _state[ORI]));


	// find derivative
	
	_derivative[POS] = _state[VEL];
	_derivative[ORI] = _state[AVEL];
	_derivative[VEL] = _force / _mass;
	_derivative[AVEL] = _torque / _inertia;

   // update state
	_state[POS]  += _derivative[POS]  * dt;
	_state[ORI]  += _derivative[ORI]  * dt;
	_state[VEL]  += _derivative[VEL]  * dt;
	_state[AVEL] += _derivative[AVEL] * dt;

   // compute global position and orientation and update _characterRoot
   quat rot = glm::angleAxis(_state[ORI], vec3(0,1,0));
   vec3 localPos(0,0,_state[POS]);

   _characterRoot.setT(rot * localPos + _characterRoot.t());
   _characterRoot.setR(rot); 
}

// randomize the colors, characters, and animation times here
void ASteerable::randomizeAppearance()
{
   // to randomize the walking animations, compute different initial values 
   // for _time

   // to randomize color, call _drawer.setColor

   // to randomize shape, compute random values for _drawer.setJointRadius
   // or randomly assign different drawers to have a mix of characters
}

