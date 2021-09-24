#include "atkui/framework.h"
using namespace glm;

/* Important design notes: each ball will be initailized at the same position, but will have different trajectories based
* on where the x-coordinate is on the first bounce.

*/
class Ball {
public:
    Ball(float screenWidth, float screenHeight, vec3 color) : color{ color } {
        createSeg(screenWidth, screenHeight);
    }

    void updateLifetime(float dt) {
        lifetime += dt;
    }

    // need to get parameter t by dividing lifetime by stop_time
    vec3 getPos(float stop_time) {
        // I will break the trajectory of the ball into 4 segments
        // The ball will first bounce after 8/16 of the stop_time interval
        // the ball will bounce a second time after 13/16 of the stop_time interval
        // the ball wil bounce a third time after 15/16 of the stop_time interval
        // the ball will stop bouncing after 1 of the stop_time interval

        float stop_first_segment = stop_time * 0.30f; // t = (lifetime - start) / (end - start)
        float stop_second_segment = stop_time * 0.60f; 
        float stop_third_segment = stop_time * 0.80f;


        float t;
        if (lifetime <= stop_first_segment) {
            t = lifetime / stop_first_segment;
            return LERPALL(firstSeg[0], firstSeg[1], firstSeg[2], firstSeg[3], t);
        }
        else if (lifetime <= stop_second_segment) {
            t = (lifetime - stop_first_segment) / (stop_second_segment - stop_first_segment);
            return LERPALL(secondSeg[0], secondSeg[1], secondSeg[2], secondSeg[3], t);
        }
        else if (lifetime <= stop_third_segment) {
            t = (lifetime - stop_second_segment) / (stop_third_segment - stop_second_segment);
            return LERPALL(thirdSeg[0], thirdSeg[1], thirdSeg[2], thirdSeg[3], t);
        }
        else if (lifetime <= stop_time) {
            t = (lifetime - stop_third_segment) / (stop_time - stop_third_segment);
            return LERPALL(fourthSeg[0], fourthSeg[1], fourthSeg[2], fourthSeg[3], t);
        }
        t = 1; // clamp position at the ending position
        LERPALL(fourthSeg[0], fourthSeg[1], fourthSeg[2], fourthSeg[3], t);
 


    }

    
    // Reference: http://nurbscalculator.in/
    // I used control points to emulate what trajectory I wanted
    // This method will create the first segment and call the function to create the second segment
    void createSeg(float screenWidth, float screenHeight) {
        vec3 B0_1 = vec3(0, screenHeight * 0.75f, 0);
        vec3 B3_1 = vec3(agl::random(screenWidth * 0.2f, screenWidth * 0.5f), radius, 0);

        // want to emulate a ball throw with the intermediate control points, so parabolic trajectory
        vec3 B1_1 = vec3(B3_1.x / 3, B0_1.y, 0); 
        vec3 B2_1 = vec3(B3_1.x, B0_1.y*0.75, 0); // I am trying to simulate acceleration by making two control points

        firstSeg[0] = B0_1;
        firstSeg[1] = B1_1;
        firstSeg[2] = B2_1;
        firstSeg[3] = B3_1;

        createSeg2(B3_1.x - B0_1.x, B0_1.y, B3_1);
    }


    // the endpoint of the first segment, will be the first control for the second control point
    // I will use the third of the height from the starting height as the height for the intermediate control points
    void createSeg2(float width_prev, float height, const vec3& B3_1) {
        vec3 B0_2 = B3_1;
        vec3 B3_2 = vec3(B0_2.x + width_prev * 0.50f, radius, 0); // should be on the floor again after the bounce

        vec3 B1_2 = vec3(B0_2.x + (B3_2.x - B0_2.x) * 0.25f, height / 3 + radius, 0);
        vec3 B2_2 = vec3(B0_2.x + (B3_2.x - B0_2.x) * 0.75f, height / 3 + radius, 0);

        secondSeg[0] = B0_2;
        secondSeg[1] = B1_2;
        secondSeg[2] = B2_2;
        secondSeg[3] = B3_2;

        createSeg3(B3_2.x - B0_2.x, B1_2.y, B3_2);
    }


    void createSeg3(float width_prev, float height, const vec3& B3_2) {
        vec3 B0_3 = B3_2;
        vec3 B3_3 = vec3(B0_3.x + width_prev * 0.50f, radius, 0);

        vec3 B1_3 = vec3(B0_3.x + (B3_3.x - B0_3.x) * 0.25f, height / 3 + radius, 0);
        vec3 B2_3 = vec3(B0_3.x + (B3_3.x - B0_3.x) * 0.75f, height / 3 + radius, 0);

        thirdSeg[0] = B0_3;
        thirdSeg[1] = B1_3;
        thirdSeg[2] = B2_3;
        thirdSeg[3] = B3_3;

        createSeg4(B3_3.x - B0_3.x, B1_3.y, B3_3);
    }

    void createSeg4(float width_prev, float height, const vec3& B3_3) {
        vec3 B0_4 = B3_3;
        vec3 B3_4 = vec3(B0_4.x + width_prev * 0.50f, radius, 0);

        vec3 B1_4 = vec3(B0_4.x + (B3_4.x - B0_4.x) * 0.25f, height / 3 + radius, 0);
        vec3 B2_4 = vec3(B0_4.x + (B3_4.x - B0_4.x) * 0.75f, height / 3 + radius, 0);

        fourthSeg[0] = B0_4;
        fourthSeg[1] = B1_4;
        fourthSeg[2] = B2_4;
        fourthSeg[3] = B3_4;
    }

    vec3 LERP(const vec3& b1, const vec3& b2, float t) {
        return b1 * (1 - t) + b2 * t;
    }

    vec3 LERPALL(const vec3& b0, const vec3& b1, const vec3& b2, const vec3& b3, float t) {
        vec3 b01 = LERP(b0, b1, t);
        vec3 b11 = LERP(b1, b2, t);
        vec3 b21 = LERP(b2, b3, t);

        vec3 b02 = LERP(b01, b11, t);
        vec3 b12 = LERP(b11, b21, t);

        return LERP(b02, b12, t);
    }

    float getRadius() {
        return radius;
    }

    vec3 getColor() {
        return color;
    }

private:
    float radius = 20.0f; // determines size of ball
    vec3  position;
    vec3  color;
    float lifetime= 0.0f; // how long the ball has since been created
    vec3  firstSeg[4]; // indices represent the control points
    vec3  secondSeg[4];
    vec3  thirdSeg[4];
    vec3  fourthSeg[4];

};

class Unique2 : public atkui::Framework {
public:
    Unique2() : atkui::Framework(atkui::Orthographic) {
    }

    virtual void setup() {
        
        palette = { // Beach Color Palette: https://www.color-hex.com/color-palette/895
        vec3(150,206,180) / 255.0f,
        vec3(255,238,173) / 255.0f,
        vec3(255,111,105) / 255.0f,
        vec3(255,204,92) / 255.0f,
        vec3(136,216,176) / 255.0f
        };

    }

    virtual void scene() {
        // inspiration for the motion https://easings.net/en#easeOutBounce
        // use bezier curve to simulate an ease out bounce
        // want the first two points to be high to low
        // then when it bounces, the two endpoints should have same y, but farther x, but control points up
        // N = 4 bounces
        // all points will be on the floor except for the intial point, offset by the radius of the ball

        if (balls.size() > max_balls) {
            balls.pop_front();
        }

        for (auto& ball : balls) {
            ball.updateLifetime(dt());
            setColor(ball.getColor());
            drawSphere(ball.getPos(stop_time), ball.getRadius());
        }

    }

    void keyUp(int key, int mod) {
        // each time we press space, the numBallTotal will increment by 1 even if a ball disappears
        if (key == GLFW_KEY_SPACE) {
            numBallTotal++;
            balls.push_back(Ball(width(), height(), palette[numBallTotal%5]));
        }
    }

private:
    float stop_time = 3.0f; // the ball will stop moving after 3 seconds
    int max_balls = 50;
    std::list<Ball> balls;
    std::vector<vec3> palette;
    int numBallTotal= 0;

};

int main(int argc, char** argv) {
    Unique2 viewer;
    viewer.run();
    return 0;
}

