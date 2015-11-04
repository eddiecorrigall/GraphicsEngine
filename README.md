# GraphicsEngine

A simple C++ OpenGL engine written from scratch. No threading but does use main-loop to update "processes" and "resources".

## Screenshots

* [screenshot 01 - View of orge and dead knight](screenshot_01.png)
* [screenshot 02 - Vector debugging](screenshot_02.png)

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
* l toggles light vector debugging
* n toggles normal vector debugging
* v toggles view vector debugging (perpendicular to view so that it is visible)

## Installation

You may need to install ogg support for SDL2_mixer

### Build Requirements

OpenGL, GLM, SDL2, SDL2_image, SDL2_mixer, OpenGL2 (gl and glu).

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
## ACT files
This is a unique format designed by yours truely. It is a configuration file containing meta data on the model, texture and animations for a 3D resource.

| Line Number | Example | Format | Description |
| --- | --- | --- | --- |
| 0 | "data/orgo.md2" | [file path] | (Required) resource path of model |
| 1 | "data/orgo.bmp" | [file path] | (Required) resource path of texture |
| 2 to N | "IDLE 1 0 40" | [action] [loop] [frame offset] [number of frames] | (Required) IDLE action definition, whitespace separated parameters |

| Parameter | Description |
| --- | --- |
| [file path] | Relative path to the executing program (engine binary) |
| [action] | "IDLE", "RUN", "JUMP", "ATTACK", "WAVE", "DEAD" |
| [loop] | If set to "0" then animation is not looped, otherwise animation will loop |
| [frame offset] | non-negative integer |
| [number of frames] | positive integer |
