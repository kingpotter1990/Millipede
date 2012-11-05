#include "Utility.h"

namespace Util{

Eigen::Affine3f Perspective(const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar){

	GLfloat top   = tan(fovy*DegreesToRadians/2) * zNear;
    GLfloat right = top * aspect;

    Eigen::Affine3f c;
	c.setIdentity();
    c(0,0) = zNear/right;
    c(1,1) = zNear/top;
    c(2,2) = -(zFar + zNear)/(zFar - zNear);
    c(2,3) = -2.0f*zFar*zNear/(zFar - zNear);
    c(3,2) = -1.0f;
    return c;

}

#define _CRT_SECURE_NO_WARNINGS
// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "r");

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);
 
    return buf;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
    struct Shader {
	const char*  filename;
	GLenum       type;
	GLchar*      source;
    }  shaders[2] = {
	{ vShaderFile, GL_VERTEX_SHADER, NULL },
	{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
	Shader& s = shaders[i];
	s.source = readShaderSource( s.filename );
	if ( shaders[i].source == NULL ) {
	    std::cerr << "Failed to read " << s.filename << std::endl;
	    exit( EXIT_FAILURE );
	}

	GLuint shader = glCreateShader( s.type );
	glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
	glCompileShader( shader );

	GLint  compiled;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
	if ( !compiled ) {
	    std::cerr << s.filename << " failed to compile:" << std::endl;
	    GLint  logSize;
	    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
	    char* logMsg = new char[logSize];
	    glGetShaderInfoLog( shader, logSize, NULL, logMsg );
	    std::cerr << logMsg << std::endl;
	    delete [] logMsg;

	    exit( EXIT_FAILURE );
	}

	delete [] s.source;

	glAttachShader( program, shader );
    }

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
	std::cerr << "Shader program failed to link" << std::endl;
	GLint  logSize;
	glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
	char* logMsg = new char[logSize];
	glGetProgramInfoLog( program, logSize, NULL, logMsg );
	std::cerr << logMsg << std::endl;
	delete [] logMsg;

	exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    return program;
}

double getRand(){
//return a random number from 0~1
	return (rand()%1000)/999.0;
}

}