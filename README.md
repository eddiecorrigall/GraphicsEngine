# GraphicsEngine

A simple C++ OpenGL engine written from scratch.
Written without threading, uses main-loop to update processes.

## Features

* Camera
* MD2 models
* Motion blur
* Cel shading
* Polygon subdivision
* Linear interpolation

## Controls

* [ESC] Exits
* [Left], [Right] rotates camera
* w,a,s,d translates camera
* z toggles polygon subdivision
* x toggles linear interpolation
* c toggles shading
* m toggles motion blur

## Installation

 1. Check out Build Requirements
 2. Build:	> make
 3. Run:		> ./engine

## Build Requirements

GLM, SDL2, SDL2_image, SDL2_mixer, OpenGL2 (gl and glu).

Note: You may need to add ogg support for SDL2_mixer

1. Update system and packages
	* > sudo apt-get update
	* > sudo apt-get install checkinstall
	* > sudo apt-get install libgl-dev libglu-dev libglm-dev freeglut3-dev
2. Download the development packages
	* > wget SDL2.tar.gz
	* > wget SDL2_mixer.tar.gz
	* > wget SDL2_image.tar.gz
3. Extract
	* > tar -xvzf *.tar.gz
4. Move into the extracted folder
5. Compile
	* > ./configure
	* > make
6. Install
  * > sudo checkinstall
7. Check:
  * > dpkg -s SDL2
  * > dpkg -L SDL2
