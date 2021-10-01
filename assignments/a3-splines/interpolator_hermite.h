#ifndef _interpolator_hermite_
#define _interpolator_hermite_

#include <string>
#include <vector>
#include <Eigen/Dense>
#include "interpolator.h"
#include <iostream>

//--------------------------------
// Hermite 
//--------------------------------

class InterpolatorHermite : public Interpolator
{
public:
    InterpolatorHermite() : Interpolator("Hermite"), mClampDir(glm::vec3(1,0,0)) {}

    virtual glm::vec3 interpolate(int segment, double u) const {
       // todo: your code here
        glm::vec3 p0 = getControlPoint(segment * 2);
        glm::vec3 s0 = getControlPoint(segment * 2 + 1); // denotes slope of p0
        glm::vec3 p1 = getControlPoint(segment * 2 + 2);
        glm::vec3 s1 = getControlPoint(segment * 2 + 3); // denotes slope of p1

        float t = (float)u;

        glm::vec3 p = p0 * (1 - 3 * t * t + 2 * t * t * t) +
            s0 * (t * (1 - t) * (1 - t)) +
            s1 * (t * t * t - t * t) +
            p1 * (3 * t * t - 2 * t * t * t);

       return p;
    }

    virtual void computeControlPoints(const std::vector<glm::vec3>& keys) {
       // todo: your code here
        // p(t)= H03()
        // want to find the slopes then solve in polynomial for interpolate
        if (keys.size() == 0 || keys.size() == 1) {
            clearControlPoints();
        }
        else {
            clearControlPoints();

            int numPoints = keys.size();

            int j = 0;
            for (int i= 0; i< numPoints*2; i++) {
                if (i % 2 == 0) {
                    mCtrlPoints.push_back(keys[j]);
                    j++;
                }
                else {
                    mCtrlPoints.push_back(glm::vec3(0)); // this will serve as temporary values
                }
            }

            Eigen::MatrixXd p(numPoints, 3); 
            Eigen::MatrixXd A(numPoints, numPoints);
            Eigen::MatrixXd pPrime(numPoints, 3);

            for (int i = 0; i < numPoints; i++) {
                glm::vec3 v;
                glm::vec3 p0;
                glm::vec3 p1;


                if (i == 0) {
                    if (isClamped()) {
                        v = getClampDirection();
                    }
                    else { // natural
                        p0 = getControlPoint(i * 2);
                        p1 = getControlPoint((i + 1) * 2);
                        v = 3.0f * (p1 - p0);
                    }
                } else if (i == numPoints - 1) {
                    if (isClamped()) {
                        v = getClampDirection();
                    }
                    else { // natural
                        p0 = getControlPoint((i - 1) * 2);
                        p1 = getControlPoint(i * 2);
                        v = 3.0f * (p1 - p0);
                    }
                }
                else {
                    p0 = getControlPoint((i - 1) * 2);
                    p1 = getControlPoint((i + 1) * 2);
                    v = 3.0f * (p1 - p0);
                }                

                p(i, 0) = v[0];
                p(i, 1) = v[1];
                p(i, 2) = v[2];
            }

            /*for (int i = 0; i < numPoints; i++) {
                for (int j = 0; j < 3; j++) {
                    std::cout << p(i, j) << " ";
                }
                std::cout << "\n" << std::endl;
            }*/

            // initializing A without completing the 1st and last row
            for (int i = 1; i < numPoints-1; i++) {
                for (int j = 0; j < numPoints; j++) {
                    if (i == j) {
                        A(i, j) = 4; // all 4s on the diagonal
                    }
                    else if (i + 1 == j || i == j + 1) { // 1's on both sides of the diagonal
                        A(i, j) = 1;
                    }
                    else {
                        A(i, j) = 0;
                    }
                }
            }

            // now to initialize the first and last row of A based on if clamped or natural
            if (isClamped()) {
                for (int j = 0; j < numPoints; j++) {
                    A(0, j) = (j == 0) ? 1 : 0; // want slope of p0 to equal v, so we have to set (0, 0) to 1
                    A(numPoints-1, j) = (j == numPoints-1) ? 1 : 0; // want slope of p_n-1 to equal v, so set (n-1, n-1) to 1
                }
            }
            else { // natural end points
                // initialize first row of A matrix
                for (int j = 0; j < numPoints; j++) {
                    if (j == 0) {
                        A(0, j) = 2;
                    }
                    else if (j == 1) {
                        A(0, j) = 1;
                    }
                    else {
                        A(0, j) = 0;
                    }
                }

                // finish last row of A matrix
                for (int j = 0; j < numPoints; j++) {
                    if (j == numPoints - 2) {
                    A(numPoints - 1, j) = 1;
                    }
                    else if (j == numPoints - 1) {
                    A(numPoints - 1, j) = 2;
                    }
                    else {
                        A(numPoints - 1, j) = 0;
                    }
                }
            }

            pPrime = A.inverse() * p;
            glm::vec3 pSlope;

            for (int i = 0; i < numPoints; i++) {
                pSlope= glm::vec3(pPrime(i, 0), pPrime(i, 1), pPrime(i, 2));
                editControlPoint(2 * i + 1, pSlope);
            }
        }


    }

    void setClamped(bool c) { mIsClamped = c; }
    bool isClamped() const { return mIsClamped; }

    void setClampedDirection(const glm::vec3& v) { mClampDir = v; }
    glm::vec3 getClampDirection() const { return mClampDir; }

private:
    bool mIsClamped = false;
    glm::vec3 mClampDir;
};

#endif
