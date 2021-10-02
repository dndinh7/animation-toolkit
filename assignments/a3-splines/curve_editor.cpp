#include "curve_editor.h"
#include <string>
#include <iostream>
#include <algorithm>
using glm::vec3;

CurveEditor::CurveEditor() : 
  atkui::Framework(atkui::Orthographic), 
  mSpline(),
  mRadius(10.0),
  mSelected(-1),
  mButtonState(0),
  mMode(ADD), 
  mShowControlPoints(false) {

  mSpline.addInterpolationType("Linear", &mLinear);
  mSpline.addInterpolationType("Hermite", &mHermite);
  mSpline.addInterpolationType("Catmull-Rom", &mCatmullRom);

  mHermite.setClamped(true);
  mHermite.setClampedDirection(vec3(100,0,0));
}

CurveEditor::~CurveEditor() {
}

void CurveEditor::setup() {
  listControls();
}

void CurveEditor::scene() {
    drawState();
    // todo: your code here


    // we have to draw it differently based on type of interpolator

    // draw the keypoints first




    // NOTE: will not be able to edit in control point mode if there are no control points
    if (mShowControlPoints && mSpline.getNumControlPoints() != 0 && mSpline.getInterpolationType() != "Linear") {
        if (mSpline.getInterpolationType() == "Catmull-Rom") {
            for (int i = 0; i < mSpline.getNumControlPoints(); i++) {
                if (i % 3 == 0) {
                    setColor(vec3(0, 0, 1));
                }
                else {
                    setColor(vec3(1, 1, 0));
                }
                vec3 controlPoint = mSpline.getControlPoint(i);
                drawSphere(controlPoint, 15.0f);
            }
        }
        else if (mSpline.getInterpolationType() == "Hermite") { // hermite
            if (mShowControlPoints && mSpline.getNumControlPoints() != 0) {
                for (int i = 0; i < mSpline.getNumControlPoints(); i++) {
                    vec3 controlPoint = mSpline.getControlPoint(i);
                    if (i % 2 == 0) {
                        setColor(vec3(0, 0, 1));
                        drawSphere(controlPoint, 15.0f);
                    }
                    else {
                        setColor(vec3(1, 1, 0));
                        // since this is slope, we add it to its keypoint
                        drawSphere(controlPoint + mSpline.getControlPoint(i - 1), 15.0f);
                    }
                }
            }
        }
    }
    else { // this will be linear interpolation
        for (int i = 0; i < mSpline.getNumKeys(); i++) {
            setColor(vec3(0, 0, 1));
            vec3 keyPoint = mSpline.getKey(i);
            drawSphere(keyPoint, 15.0f);
        }
    }


    // draws the blue lines between key points
    if (mSpline.getNumKeys() > 1) {
        // I want 30 points between each segment
        int numPoints = (mSpline.getNumKeys()) * 30;
        float interval = (mSpline.getDuration() - mSpline.getTime(0)) / (float)numPoints;
        
        std::vector<vec3> points(numPoints+1);
        for (int n = 0; n < numPoints + 1; n++) {
            points[n] = mSpline.getValue(interval * n);
        }
        setColor(vec3(0, 0, 1));
        for (int i = 0; i < numPoints; i++) {
            drawLine(points[i], points[i + 1]);
        }
    }

    // draws the yellow line depending on which type of curve
    if (mShowControlPoints && mSpline.getNumControlPoints() != 0) {
        // if catmullrom, then we draw a yellow line from control point to control point
        setColor(vec3(1, 1, 0));
        if (mSpline.getInterpolationType() == "Catmull-Rom") {
            for (int i = 0; i < mSpline.getNumControlPoints() - 1; i++) {
                drawLine(mSpline.getControlPoint(i), mSpline.getControlPoint(i + 1));
            }
        }

        if (mSpline.getInterpolationType() == "Hermite") {
            for (int i = 0; i < mSpline.getNumControlPoints(); i+=2) {
                vec3 kp = mSpline.getControlPoint(i);
                vec3 slope = mSpline.getControlPoint(i + 1);
                drawLine(kp, kp+slope);
            }
        }
    }


}

int timeCount = 0; // this will be a new count for the time parameter of adding a key
// there was a bug where if you deleted multiple things, then added more points, then there would be duplicate times for points
// so I will always have distinct times for each point

void CurveEditor::addPoint(const vec3& p) {
  //std::cout << "Add key: " << p << std::endl;
  mSpline.appendKey(timeCount, p);
  timeCount++;
  mSpline.computeControlPoints();
}

void CurveEditor::deletePoint(int key) {
  mSpline.deleteKey(key);
  mSpline.computeControlPoints();
}

void CurveEditor::drawState() {
  int th = 25;
  switch (mMode) {
    case ADD: drawText("Mode: ADD", 5, th); break;
    case EDIT: drawText("Mode: EDIT", 5, th); break;
    case REMOVE: drawText("Mode: DELETE", 5, th); break;
  }
  th += renderer.textHeight() + 5; 
  drawText("Type: "+mSpline.getInterpolationType() + 
      ((mSpline.getInterpolationType()=="Hermite") ? ((mHermite.isClamped()) ? " (clamped)" : " (natural)") : ""), 5, th);
  
  th += renderer.textHeight() + 5;
  
  if (mShowControlPoints) {
      drawText("View: Control Points", 5, th);
  }
  else {
      drawText("View: Key Points", 5, th);
  }
   
}

void CurveEditor::listControls() {

  std::cout << "Key controls:" << std::endl;
  std::cout << "0: Linear" << std::endl;
  std::cout << "1: Catmull-Rom" << std::endl;
  std::cout << "2: Hermite" << std::endl;
  std::cout << "c: Toggle control points" << std::endl;
  std::cout << "h: clamp hermite on/off" << std::endl;
  std::cout << "a: add mode" << std::endl;
  std::cout << "e: edit mode" << std::endl;
  std::cout << "d: delete mode" << std::endl;
  std::cout << "SPACEBAR: clear" << std::endl;
}

void CurveEditor::mouseMotion(int pX, int pY, int dx, int dy) {
  if (mMode != EDIT) return;
  if (mSelected == -1) return;
  
  if (mButtonState == GLFW_MOUSE_BUTTON_LEFT) {
    vec3 p = vec3(pX, height() - pY, 0);
    if (mShowControlPoints) {
      if (mSpline.getInterpolationType() == "Hermite" && mSelected % 2 == 1) {
        mSpline.editControlPoint(mSelected, p - mSpline.getControlPoint(mSelected - 1));
      } else {
        mSpline.editControlPoint(mSelected, p); // catmullRom
      }
    }
    else mSpline.editKey(mSelected, p); // I will edit every keypoint regardless
  }
}

int CurveEditor::pickPoint(int x, int y) {
  vec3 tmp = vec3(x, y, 0);

  int num = mSpline.getNumKeys();
  if (mShowControlPoints) {
    num = mSpline.getNumControlPoints();
  }

  // check data points -- needs to be done before ctrl points
  for (int i = 0; mSelected == -1 && i < num; i++) {
    vec3 pt;
    if (mShowControlPoints) {
      pt = mSpline.getControlPoint(i);
      if (mSpline.getInterpolationType() == "Hermite" && i % 2 == 1) {
        pt = pt + mSpline.getControlPoint(i-1);
      }
    } else {
      pt = mSpline.getKey(i);
    }

    if (length(tmp - pt) < mRadius) {
      return i;
    }
  }

  return -1;
}

void CurveEditor::mouseUp(int pButton, int state) {
  if (mButtonState == GLFW_MOUSE_BUTTON_LEFT) {
    mSelected = -1;
  }
}

void CurveEditor::mouseDown(int pButton, int state) {
  mButtonState = pButton;
  glm::vec2 p = mousePosition();
  int pX = (int) p.x;
  int pY = (int) p.y;

  if (mButtonState == GLFW_MOUSE_BUTTON_LEFT) {
    if (mMode == ADD) {
      vec3 tmp(p, 0);
      // guard against adding multiple copies of the same point
      // (multiple mouse events may be triggered for the same point)
      bool add = true;
      if (mSpline.getNumKeys() > 0) {
        vec3 lastKey = mSpline.getKey(mSpline.getNumKeys() - 1);
        float d = length(lastKey-tmp);
        if (d < 10) add = false;
      }

      if (add) {
        if (!mShowControlPoints) addPoint(vec3(pX, height()-pY, 0));
      }

    } else if (mMode == REMOVE) {
      mSelected = pickPoint(pX, height() - pY);
      if (mSelected != -1 && !mShowControlPoints) deletePoint(mSelected);

    } else if (mMode == EDIT) {
      mSelected = pickPoint(pX, height() - pY);
      std::cout << "SELECTED: " << mSelected << std::endl;
      mouseMotion(pX, pY, 0, 0);
    }
  }

}

void CurveEditor::keyUp(int pKey, int mods) {
  if (pKey == 'p') {
    for (int i = 0; i < mSpline.getNumKeys(); i++) {
      std::cout << mSpline.getKey(i) << std::endl;
    }
  } else if (pKey == '1') {
    mSpline.setInterpolationType("Linear");
    mSpline.computeControlPoints();

  } else if (pKey == '2') {
    mSpline.setInterpolationType("Catmull-Rom");
    mSpline.computeControlPoints();

  } else if (pKey == '3') {
    mSpline.setInterpolationType("Hermite");
    mSpline.computeControlPoints();

  } else if (pKey == 'A') {
    mMode = ADD;

  } else if (pKey == 'D') {
    mMode = REMOVE;

  } else if (pKey == 'E') {
    mMode = EDIT;

  } else if (pKey == 'C') { // pressing 'C' will recompute the control points, so the edits we've made to the controls points disappear
      if (mShowControlPoints) {
          if (mSpline.getInterpolationType() == "Catmull-Rom") {
              for (int i = 0; i < mSpline.getNumControlPoints(); i+=3) {
                  mSpline.editKey(i / 3, mSpline.getControlPoint(i));
              }
          }
          else if (mSpline.getInterpolationType() == "Hermite") {
              for (int i = 0; i < mSpline.getNumControlPoints(); i += 2) {
                  mSpline.editKey(i / 2, mSpline.getControlPoint(i));
              }
          }
      }
    mShowControlPoints = !mShowControlPoints;

  } else if (pKey == GLFW_KEY_SPACE) {
    mSpline.clear();
    timeCount = 0;

  } else if (pKey == 'H') {
    mHermite.setClamped(!mHermite.isClamped());
    mSpline.computeControlPoints();
  } 

}

int main(int argc, char** argv)
{
	CurveEditor viewer;
	viewer.run();
}
