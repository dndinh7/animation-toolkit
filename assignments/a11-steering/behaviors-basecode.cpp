#include "behaviors.h"
#include "steerable.h"
#include <cmath>
#include <limits>

using namespace glm;
using namespace atk;


//--------------------------------------------------------------
// Shared properties for all behaviors
ABehavior::ABehavior(const char* name) : _name(name)
{
   // TODO: set good values
   setParam("MaxSpeed", 150);
   setParam("AgentRadius", 100);
   setParam("Threshold", 50);
}

//--------------------------------------------------------------
// Seek behavior

ASeek::ASeek() : ABehavior("Seek") 
{   
}

//
// Steer the actor TOWARDS a target at max speed
//
// @param actor: steerable character to move
// @return desired velocity
//
// @note: call actor.getPosition to get the actor's position
// @note: call getParam("MaxSpeed") to get the max speed
vec3 ASeek::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
    vec3 p = actor.getPosition();
    vec3 dir = (length(p - target) <= getParam("Threshold")) ? vec3(0) : normalize(target - p);
    return getParam("MaxSpeed")*dir;
}

//--------------------------------------------------------------
// Flee behavior

AFlee::AFlee() : ABehavior("Flee") 
{
}

//
// Steer the actor AWAY from a target at max speed
//
// @param actor: steerable character to move
// @return desired velocity
//
// @note: call actor.getPosition to get teh actor's position
// @note: call getParam("MaxSpeed") to get the max speed
vec3 AFlee::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& targetPos)
{
    vec3 p = actor.getPosition();
    vec3 dir = normalize(p - targetPos);
    return getParam("MaxSpeed") * dir;
}

//--------------------------------------------------------------
// Arrival behavior

AArrival::AArrival() : ABehavior("Arrival") 
{
   // TODO: Set good parameters
   setParam("kArrival", 1);
   setParam("TargetRadius", 250);
}

//
// Returns a velocity whose speed is proportional to the actors distance
// from the target
//
// @param actor: steerable character to move
// @return desired velocity
vec3 AArrival::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& targetPos)
{

    vec3 p = actor.getPosition();
    vec3 targetOffset = targetPos - p;
    float dist = length(targetOffset);
    targetOffset = (dist <= getParam("Threshold")) ? vec3(0) : targetOffset;
    float speed;
    if (dist <= getParam("TargetRadius")) {
        speed = (dist / getParam("TargetRadius")) * getParam("MaxSpeed");
    } else {
        speed = getParam("MaxSpeed");
    }
    return speed * (targetOffset / dist);
}

//--------------------------------------------------------------
// Departure behavior

ADeparture::ADeparture() : ABehavior("Departure") 
{
   setParam("InnerRadius", 100);
   setParam("OuterRadius", 250);
   setParam("kDeparture", 1);
}

//
// Calculate a repelent velocity based on the actor's 
// distance from the target
vec3 ADeparture::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& targetPos)
{
    vec3 p = actor.getPosition();
    vec3 targetOffset = (p - targetPos);
    float dist = length(targetOffset);
    float speed;
    if (dist > getParam("OuterRadius")) {
        speed = getParam("MaxSpeed");
    }
    else {
        speed = (getParam("InnerRadius") > dist) ? 
            (getParam("InnerRadius") / getParam("OuterRadius")) * getParam("MaxSpeed") : (dist / getParam("OuterRadius")) * getParam("MaxSpeed");
    }

   return speed*(targetOffset/dist);
}

//--------------------------------------------------------------
// Avoid behavior

AAvoid::AAvoid() : ABehavior("Avoid") 
{
   setParam("kAvoid", 1);
   setParam("danger", getParam("MaxSpeed"));
   setParam("radiusDanger", 125);
}

// If an actor is near an obstacle, avoid adds either a tangential or
// normal response velocity
//  Obstacles are in getWorld()->getObstacle(i) and have class type AObstacle
vec3 AAvoid::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& targetPos)
{   
    vec3 p = actor.getPosition();
    vec3 lineOfSight;
    vec3 toTarget = targetPos - p;

    std::vector<vec3> circPos;
    std::vector<float> circRad;

    if (length(actor.getVelocity()) == 0) {
        lineOfSight = normalize(actor.getRotation() * vec3(0, 0, 1)) * getParam("danger");
    }
    else {
        lineOfSight = normalize(actor.getVelocity()) * getParam("danger");
    }

    //vec3 upLOS  = p + getParam("radiusDanger") * normalize(actor.getRotation() * vec3(1, 0, 0));
    //vec3 lowLOS = p - getParam("radiusDanger") * normalize(actor.getRotation() * vec3(1, 0, 0));
    
    vec3 closestObstacle = vec3(std::numeric_limits<float>::max());
    bool intersect = false;

    for (int i = 0; i < world.getNumObstacles(); i++) {
        vec3 actToCirc = world.getObstacle(i).position - p;
        float circRad = world.getObstacle(i).radius;


        float angle = acos((dot(toTarget, actToCirc)) / (length(toTarget) * length(actToCirc)));

        float distToLine = sin(angle) * length(actToCirc);
        std::cout << "dist: " << distToLine << std::endl;
        std::cout << "rad: " << circRad << std::endl;
        if (distToLine <= circRad) {
            intersect = true;
            if (length(actToCirc) < length(closestObstacle - p)) {
                closestObstacle = world.getObstacle(i).position;
            }
        }
    }


    AFlee fleer = AFlee::AFlee();

    if (intersect) return fleer.calculateDesiredVelocity(actor, world, closestObstacle);

    ASeek seeker = ASeek::ASeek();

    return seeker.calculateDesiredVelocity(actor, world, targetPos);
}
//--------------------------------------------------------------
// Wander behavior

AWander::AWander() : ABehavior("Wander")
{
   setParam("kWander", 1);
   setParam("center", 250);
   setParam("wanderStrength", 100);
   setParam("r1", 0.5);
   setParam("angle", 0);
}

// Wander returns a velocity whose direction changes randomly (and smoothly)
vec3 AWander::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
    vec3 p = actor.getPosition();
    vec3 circlePos;
    if (length(actor.getVelocity()) == 0) {
        circlePos = p + normalize(actor.getRotation() * vec3(0, 0, 1)) * getParam("center");
    }
    else {
        circlePos = p + normalize(actor.getVelocity()) * getParam("center");
    }

    setParam("angle", getParam("angle") + agl::random(-getParam("r1"), getParam("r1")));
    
    vec3 jitVelPos = circlePos + getParam("wanderStrength") * vec3(sin(getParam("angle")), 0, cos(getParam("angle")));

    return normalize(jitVelPos - p) * getParam("MaxSpeed");
}

//--------------------------------------------------------------
// Separation behavior

ASeparation::ASeparation() : ABehavior("Separation")
{
   setParam("Neighborhood", 200);
   setParam("kSeparation", 1);
}


// Separation tries to maintain a constant distance between all agents
// within the neighborhood of the agent
// getWorld()->getAgent(i) returns the ith agent in the environment
// you can check whether two agents are the same with ==
vec3 ASeparation::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
    vec3 desired = vec3(0);
    vec3 p = actor.getPosition();
    for (int i = 0; i < world.getNumAgents(); i++) {
        if (!(world.getAgent(i) == actor)) {

            vec3 pi = world.getAgent(i).getPosition();
            if (length(p - pi) <= getParam("Neighborhood")) {

                desired += (p - pi) / (length(p - pi) * length(p - pi));

            }
        }
    }

    if (length(desired) == 0) return vec3(0);

    return normalize(desired) * getParam("MaxSpeed");;
}


//--------------------------------------------------------------
// Cohesion behavior

ACohesion::ACohesion() : ABehavior("Cohesion")
{
   setParam("Neighborhood", 1);
   setParam("kCohesion", 1);
}

// Cohesion moves actors towards the center of a group of agents
vec3 ACohesion::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
    return vec3(0,0,0);
}

//--------------------------------------------------------------
// Alignment behavior
AAlignment::AAlignment() : ABehavior("Alignment")
{
   setParam("Neighborhood", 1);
   setParam("kAlignment", 1);
}

// Alignment returns an average velocity of all active agents
vec3 AAlignment::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
    return vec3(0);
}

//--------------------------------------------------------------
// Flocking behavior
AFlocking::AFlocking() : ABehavior("Flocking")
{
}

// Flocking combines separation, cohesion, and alignment
vec3 AFlocking::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
   return vec3(0,0,0);
}

//--------------------------------------------------------------
// Leader
ALeader::ALeader() : ABehavior("Leader")
{
   setParam("CSeparation", 1);
   setParam("CCohesion", 1);
   setParam("CAlignment", 1);
   setParam("CArrival", 1);
}

// You need to find the leader, who is always the first agent in agents
// If the actor is the leader, move towards the target; otherwise, 
// follow the leader as a flock
vec3 ALeader::calculateDesiredVelocity(const ASteerable& actor,
   const AWorld& world, const vec3& target)
{
   return vec3(0,0,0);
}


