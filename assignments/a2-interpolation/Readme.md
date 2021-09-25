# Readme

# Task 1: Gradient
![image](https://user-images.githubusercontent.com/72237791/134732843-6e9d7b9b-1ca4-4fff-8fc1-616e2981134d.png)

This is a grid of N by N squares, where the colors of the squares between the four corners are linearly interpolated. We see that yellow is more dominant near the NW square; red is more dominant near the SW square; cyan is more dominant near the NE square; fuchsia is more dominant near the SE square.

# Task 2: Draw Curve
![drawcurve](https://user-images.githubusercontent.com/72237791/134733953-6204b1cb-e0bc-4d63-adbc-73fe2fd8c70e.gif)

This gif shows the construction of the bezier curve in two different ways: Bernstein's curve polynomial and de Casteljau's algorithm. When "1" is pressed, the curve shown was created by Bernstein's curve polynomial and has a white color. When "2" is pressed, the curve shown was created by de Casteljau's algorithm and has a yellow color.

![drawcurve2](https://user-images.githubusercontent.com/72237791/134734795-b3d04adc-b352-4c84-a748-23af36a80b16.gif)

To adhere to the instructions, here is the animation where the curves are the same color, so that they are visually identical.

# Task 3: Cubic Motion

![cubicparticle](https://user-images.githubusercontent.com/72237791/134735712-04b75398-b0ac-426a-9a4d-ce338c53fb04.gif)

The gif shows the animation of a sphere moving along a cubic curve path. The sphere takes 5 seconds to get from the start of the curve to the end of the curve. Once the sphere reaches the end of curve, it will wrap around to the beginning to start traversing the curve again.

# Task 4: Screensaver

![screensaver](https://user-images.githubusercontent.com/72237791/134752521-885253db-c906-4063-b03f-a0dfa0dab84f.gif)

The gif shows an animation of a "screensaver" where we interpolate from one curve to another. We also store past curves to create a trailing effect. 

# Task 5: Throwing a Ball

![unique](https://user-images.githubusercontent.com/72237791/134752702-b40db58a-7d26-4dee-a75c-cc2c7361702f.gif)

My unique animation is trying to simulate throwing a ball and having it bounce 4 times before stopping. I naively implemented this using 4 segments. To throw a ball: press 'spacebar', and you can repeatedly throw them. The balls will disappear after the number of balls hits a certain threshold, in this case it's 50.
