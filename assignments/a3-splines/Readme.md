# Readme

# Task1
![linear](https://user-images.githubusercontent.com/72237791/135702004-833f7023-7bd9-41da-9278-c81ddb38e9ad.png)

The values I got with the linear interpolator.

# Task2
![catmullrom](https://user-images.githubusercontent.com/72237791/135702015-6d2bc653-c3ca-4aa4-89ae-69c604009c04.png)

The values I got with the catmullrom interpolator.

# Task3
![hermite](https://user-images.githubusercontent.com/72237791/135702025-aa830bd2-d81e-487c-a0e8-01cb87460ac8.png)

The values I got with the hermite interpolator.

# Task4
![spline1](https://user-images.githubusercontent.com/72237791/135702031-153fa6f4-a6a2-44f5-b6da-71e50f81f6b9.png)
![spline2](https://user-images.githubusercontent.com/72237791/135702034-7a5a3e93-819f-4b9e-a471-56f0dedfc4a2.png)

The values I got using getValue.

# Task5
https://user-images.githubusercontent.com/72237791/135702132-3f69a87c-a949-4484-8ee0-1e6ab4347d36.mp4

This short video shows the creation and modification of a spline, using all three interpolators.

Design Decisions:
Whenever we exit the control point view, the control points will be recalculated. Whenever we switch to a new curve, we will recalculate the control points. Also, I had a bug with deleting the first control point. I will strive to fix it.

# Task6
![unique](https://user-images.githubusercontent.com/72237791/135704162-3eea0d9e-4eba-416e-a776-2f3d014e8880.gif)

The animation is a bunch of squares moving around the screen based on Hermite curves, while changing colors based on Catmullrom curves, and changing sizes linearly. They move forward for 8 seconds, then backwards for 8 seconds in an infinite loop.
