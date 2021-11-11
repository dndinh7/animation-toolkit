#include <iostream>
#include <Eigen/Dense>
#include "agl/aglm.h"

using glm::vec2;
using glm::vec3;

int main()
{
    vec2 p0(0,0); // Z will just be zero
    vec2 p1(1,2);
    vec2 p2(3,3);
    vec2 p3(6,3);
    vec2 p4(8,0);

    // Solve the system of linear equations
    Eigen::MatrixXd A(5, 5);
    Eigen::MatrixXd p(5, 2);
    Eigen::MatrixXd pPrime(5, 2); // slopes for each control point

    // natural endpoints
    A(0,0) =  2; A(0,1) = 1; A(0,2) = 0; A(0,3) = 0; A(0,4) = 0;
    A(1,0) =  1; A(1,1) = 4; A(1,2) = 1; A(1,3) = 0; A(1,4) = 0;
    A(2,0) =  0; A(2,1) = 1; A(2,2) = 4; A(2,3) = 1; A(2,4) = 0;
    A(3,0) =  0; A(3,1) = 0; A(3,2) = 1; A(3,3) = 4; A(3,4) = 1;
    A(4,0) =  0; A(4,1) = 0; A(4,2) = 0; A(4,3) = 1; A(4,4) = 2;

    vec2 v0 = 3.0f *(p1 - p0);
    vec2 v1 = 3.0f *(p2 - p0);
    vec2 v2 = 3.0f *(p3 - p1);
    vec2 v3 = 3.0f *(p4 - p2);
    vec2 v4 = 3.0f *(p4 - p3);
    p(0,0) = v0[0]; p(0,1) = v0[1];
    p(1,0) = v1[0]; p(1,1) = v1[1]; 
    p(2,0) = v2[0]; p(2,1) = v2[1]; 
    p(3,0) = v3[0]; p(3,1) = v3[1]; 
    p(4,0) = v4[0]; p(4,1) = v4[1]; 

    std::cout << "this is p" << std::endl;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2; j++) {
            std::cout << p(i, j) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "this is A" << std::endl;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            std::cout << A(i, j) << " ";
        }
        std::cout << std::endl;
    }

    pPrime = A.inverse() * p;
    for (int i = 0; i < 5; i++)
    {
        std::cout << "slope " << i << " = " << pPrime(i,0) << " " << pPrime(i,1) << std::endl;
    }


    vec3 p_0(137, 121, 0); // Z will just be zero
    vec3 p_1(221, 313, 0);
    vec3 p_2(398, 118, 0);

    Eigen::MatrixXd A2(3, 3);
    Eigen::MatrixXd p22(3, 3);
    Eigen::MatrixXd pPrime2(3, 3);

    vec3 v_0 = 3.0f * (p_1 - p_0);
    vec3 v_1 = 3.0f * (p_2 - p_0);
    vec3 v_2 = 3.0f * (p_2 - p_1);

    // natural end points
    A2(0, 0) = 2; A2(0, 1) = 1; A2(0, 2) = 0;
    A2(1, 0) = 1; A2(1, 1) = 4; A2(1, 2) = 1;
    A2(2, 0) = 0; A2(2, 1) = 1; A2(2, 2) = 2;

    p22(0, 0) = v_0[0]; p22(0, 1) = v_0[1]; p22(0, 2) = v_0[2];
    p22(1, 0) = v_1[0]; p22(1, 1) = v_1[1]; p22(1, 2) = v_1[2];
    p22(2, 0) = v_2[0]; p22(2, 1) = v_2[1]; p22(2, 2) = v_2[2];

    pPrime2 = A2.inverse() * p22;
    std::cout << "this is p22" << std::endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::cout << p22(i, j) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "this is A2" << std::endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::cout << A2(i, j) << " ";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < 3; i++)
    {
        std::cout << "slope " << i << " = " << pPrime2(i, 0) << " " << pPrime2(i, 1) << " " << pPrime2(i, 2) << std::endl;
    }
    


}


