//Created by Franklin Fang on 3/30/2012

#ifndef FFFANG_UTILITY_H_
#define FFFANG_UTILITY_H_

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#  include <OpenGL/glu.h>
#else // windows
#  include <windows.h> 
#  include <GL/glew.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

#include <iostream>
#include "Eigen/Dense"

#ifndef DegreesToRadians
# define DegreesToRadians 0.01745329251994329576922222222222
#endif

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace Util{

Eigen::Affine3f Perspective(const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar);

GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

double getRand();

}

#endif