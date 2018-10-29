# PA8: Collisions with Bullet - UNDER DEVELOPMENT

## Group
This is the main repository for the code. The group members include
  * Jordan Andrieu
  * Deev Patel
  * Braeden Richards

## Dependency Instructions
This project uses OpenGL 3.3. Newer versions should work, but nothing is guaranteed. <br> 
In addition to OpenGL, this project requires installation of the following libaries: 
 * [ASSIMP](https://github.com/assimp/assimp/wiki) 
 * [Bullet](https://github.com/bulletphysics/bullet3)
 * [GLEW](http://glew.sourceforge.net/)
 * [GLM](http://glm.g-truc.net/0.9.7/index.html)
 * [Magick++](http://www.imagemagick.org/Magick%2B%2B/).
 * [SDL2](https://wiki.libsdl.org/Tutorials), 

### Ubuntu 18.04
```bash
 sudo apt-get install libassimp-dev libbullet-dev libglew-dev libglm-dev libmagick++-dev libsdl2-dev 
```

## Building and Running - CMake Instructions
The building of the project is done using CMake.
```bash
mkdir build
cd build
cmake ..
make
./PA08
```

## Keyboard Controls
IMPORTANT: For controls to work as intended, ensure the main window (not menu window) is selected. <br>
* Keyboard controls
  * m: toggle (open/close) IMGUI menu window
* Exiting
  * ESC: Will properly close all windows and exit program

## Configuration File
This program makes use of configuration files to initalize various parameters. The configuration file can be loaded with the '-l' command line flag. By default, the "launch/DefaultConfig.txt" file is loaded. The data in the file must appear in the order shown. You should only change the values. Adding new lines/variables may cause errors.

## IMGUI Menu System
There is a menu system that runs in a separate window. The menu system be started and closed with the 'm' key. <br>

## Shaders
The GLSL source code for the vertex and fragment shaders must be located under the "shaders/" directory. <br>
By default, the vertex shader code is in "vertShader.vert" and the fragment shader code is in "fragShader.frag". You can specify the name of the files via the configuration. See the example file given.

## Command Line Arguments
The following command line arguments can be used to change the default behavior of the project
  * -l: load configuration file

The default behavior can be achieved with the following arguments
```bash
-l launch/DefaultConfig.txt
```

Full Usage
```bash
./PA08 -l launch/DefaultConfig.txt
```