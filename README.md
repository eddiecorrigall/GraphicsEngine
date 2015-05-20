# GraphicsEngine

A simple C++ OpenGL engine written from scratch. No threading but does use main-loop to update "processes" and "resources".

## Screenshots

* [screenshot 01](screenshot_01.png)

## Features

Sound, camera, md2 models, linear-interpolated animation, motion blur, cel shading, polygon subdivision.

## Controls

* [ESC] Exits
* [Left], [Right] rotates camera
* w,a,s,d translates camera
* z toggles polygon subdivision
* x toggles linear interpolation
* c toggles shading
* m toggles motion blur

## Installation

2. Build binaries
	* make
3. Run
	* ./engine

### Build Requirements

GLM, SDL2, SDL2_image, SDL2_mixer, OpenGL2 (gl and glu).

Note: You may want to install ogg support for SDL2_mixer

```{r, engine='bash', count_lines}
apt-get update
apt-get install libgl-dev libglu-dev libglm-dev freeglut3-dev libglm-dev libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
```

### Build
```{r, engine='bash', count_lines}
make
```

## Run
```{r, engine='bash', count_lines}
./engine
```
