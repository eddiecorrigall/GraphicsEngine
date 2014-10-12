GraphicsEngine
==============

A C++ OpenGL engine.

# Requirements

  SDL2, SDL2_mixer, SDL2_image, OpenGL2 (gl, glu, glut).

# Installation

  Note: You may need to add support for ogg

  sudo apt-get update
  sudo apt-get install checkinstall
  sudo apt-get install libgl-dev libglu-dev libglm-dev freeglut3-dev

Download the development packages:

  wget SDL2.tar.gz
  wget SDL2_mixer.tar.gz
  wget SDL2_image.tar.gz

Extract:

  tar -xvzf *.tar.gz

Compile:

  ./configure
  make

Install:

  sudo checkinstall

Check:

  dpkg -s SDL2
  dpkg -L SDL2
