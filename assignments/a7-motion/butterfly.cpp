#include <iostream>
#include "atkui/framework.h"
#include "atk/toolkit.h"

using namespace glm;
using namespace atk;

class Butterfly : public atkui::Framework
{
public:
   Butterfly() : atkui::Framework(atkui::Perspective) {
   }

   void setup() {
      Joint* body = new Joint("Body");
      body->setLocalTranslation(vec3(100,200,0));
      body->setLocalRotation(glm::angleAxis(glm::radians<float>(45.0f), vec3(0,1,0)));
      skeleton.addJoint(body);

      Joint* lwing = new Joint("LWing");
      lwing->setLocalTranslation(vec3(0.1,0,0)*100.0f);
      skeleton.addJoint(lwing, body);

      Joint* rwing = new Joint("RWing");
      rwing->setLocalTranslation(vec3(-0.1,0,0)*100.0f);
      skeleton.addJoint(rwing, body);

      skeleton.fk();
   }

   void scene() {
       theta += dt();
      Joint* body = skeleton.getByName("Body");
      body->setLocalTranslation(vec3(100 * sin(elapsedTime()), 100+75*sin(elapsedTime()), 100 * cos(elapsedTime())));
      body->setLocalRotation(glm::angleAxis(theta, vec3(0, 1, 0)));

      Joint* lwing = skeleton.getByName("LWing");
      lwing->setLocalRotation(glm::angleAxis(sin(elapsedTime()), vec3(0,0,1)));

      Joint* rwing = skeleton.getByName("RWing");
      rwing->setLocalRotation(glm::angleAxis(-sin(elapsedTime()), vec3(0,0,1))); 


      skeleton.fk();


      // local axes
      vec3 origin= vec3(0);
      vec3 xAxis = vec3(100, 0, 0);
      vec3 yAxis = vec3(0, 100, 0);
      vec3 zAxis = vec3(0, 0, 100);


      // attach geometry to skeleton 
      Transform B = body->getLocal2Global(); 
      Transform LT = lwing->getLocal2Global(); 
      Transform RT = rwing->getLocal2Global(); 

      origin = B.transformPoint(origin);
      xAxis = B.transformPoint(xAxis);
      yAxis = B.transformPoint(yAxis);
      zAxis = B.transformPoint(zAxis);

      // draw body
      Transform bodyGeometry(
         glm::angleAxis(glm::pi<float>()*0.5f, vec3(1,0,0)), // rotation
         vec3(0), vec3(25, 200, 25)); // position, scale

      Transform lwingGeometry1(
          glm::angleAxis(glm::pi<float>() * 0.25f, vec3(0, 1, 0)),
         vec3(-100,0,-50), 
         vec3(120,20,200));

      Transform lwingGeometry2(
          glm::angleAxis(glm::pi<float>() * -0.25f, vec3(0, 1, 0)),
          vec3(-100, 0, 50),
          vec3(120, 20, 200));

      Transform rwingGeometry1(
          glm::angleAxis(glm::pi<float>() * 0.25f, vec3(0, 1, 0)),
          vec3(100, 0, 50),
          vec3(120, 20, 200));

      Transform rwingGeometry2(
          glm::angleAxis(glm::pi<float>() * -0.25f, vec3(0, 1, 0)),
          vec3(100, 0, -50),
          vec3(120, 20, 200));

      setColor(vec3(0.4, 0.4, 0.8));
      push();
      transform(B * bodyGeometry);
      drawSphere(vec3(0), 1);
      pop();

      setColor(vec3(1, 153.0f/255.0f, 51.0f/255.0f));
      push();
      transform(LT * lwingGeometry1);
      drawSphere(vec3(0), 1);
      pop();

      setColor(vec3(0));
      push();
      transform(LT * lwingGeometry2);
      drawSphere(vec3(0), 1);
      pop();

      setColor(vec3(0));
      push();
      transform(RT * rwingGeometry1);
      drawSphere(vec3(0), 1);
      pop();

      setColor(vec3(1, 153.0f / 255.0f, 51.0f / 255.0f));
      push();
      transform(RT * rwingGeometry2);
      drawSphere(vec3(0), 1);
      pop();

      if (show) {
          setColor(vec3(1, 0, 0));
          drawEllipsoid(origin, xAxis, 10);
          setColor(vec3(0, 1, 0));
          drawEllipsoid(origin, yAxis, 10);
          setColor(vec3(0, 0, 1));
          drawEllipsoid(origin, zAxis, 10);
      }
   }

   virtual void keyUp(int key, int mods) {
       if (key == 'A') {
           show = !show;
       }
   }

private:
   Skeleton skeleton;
   float theta = 0;
   bool show= false;

};


int main(int argc, char** argv) {
   Butterfly viewer;
   viewer.run();
}
