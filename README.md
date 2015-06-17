# opengl-terrain

## Screenshots

![SunSet](https://github.com/jfperren/procedural-terrain/blob/master/saved_screenshots/sunset.png)
![Sunny](https://github.com/jfperren/procedural-terrain/blob/master/saved_screenshots/sunny.jpg)
![NightCall](https://github.com/jfperren/procedural-terrain/blob/master/saved_screenshots/nightcall.png)

## Compiling the project with Visual Studio

In CMake, set the location of the source code as `path/to/procedural-terrain`. Choose where to build the binaries, for example `path/to/procedural-terrain/bin`.
Click configure and specify `Visual Studio 12 2013 Win64` or later as the generator for this project. Then generate and you are ready to open `path/to/procedural-terrain/bin/Moutains.sln`.
Finally, set the working direcory (`right click on Moutains > properties > debugging`) as `../../src`.

## Some tips to use the project

+ W-A-S-D-SHIFT-SPACE are the keys to move around the terrain.
+ 5 different modes :
    - 1) FREE MODE : you can go around the terrain freely and go up and down using SHIFT and SPACE.
    - 2) FPS MODE : you can only use WASD keys and go around the terrain, the height will be the height of your noise.
    - 3) BEZIER MODE : display your Bezier curve on your terrain.
    - 4) BEZIER EDIT MODE : you can change the positions of the point that determines Bezier curves.
    - 5) INERTIA MODE : Same as FREE MODE but with inertia.
+ You can use tweakbar to change the terrain, make sure you are in NO_THEME (at the top of the tweakbar) if you want to change every parameters. 
