# CENG477 - HW3
An OpenGL program to render a given scene using vertex and fragment shaders to display a texture image as a height map to fly through the scene interactively by processing keyboard input.  

## Getting Started

Just clone the repo and run "make all".

### Prerequisites

This project depends on GLEW, GLFW3 and OpenGL. For openGL, graphic card vendors(NVIDIA, AMD, etc.) provide hardware specific drivers. GLUT can be installed from [here](https://www.opengl.org/resources/libraries/glut/). Lastly, GLEW can be installed from [here](http://glew.sourceforge.net/). After that one needs to include these static libraries, while doing that glfw3 needs to be included after the other two. For example:
```
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
```

## Running the tests

In the directory there are two images. One can run the test with any image as:
```
./hw3 <image_name>
```

## Built With

* [Make](https://www.gnu.org/software/make/) - Build and installation tool
* [GLUT](https://www.opengl.org/resources/libraries/glut/) -  Windowing application programming interface
* [GLEW](https://www.opengl.org/sdk/libs/GLEW/) - OpenGL extension library

## Authors

* **Egemen Berk Galatalı** - [axydor](https://github.com/axydor)

* **Cenk Cidecio** - [ccidecio](https://github.com/ccidecio)

## Acknowledgments

* This is a homework of the CENG477 - Computer Graphics class at the Middle East Technical University

