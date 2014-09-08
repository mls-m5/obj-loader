/*
 * shaderProgram.h
 *
 *  Created on: 16 mar 2014
 *      Author: mattias
 */

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else

#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <string>
#include "common.h"

class ShaderProgram {
public:
	ShaderProgram(ShaderProgram &s){
		this->gProgram = s.gProgram;
	}
	ShaderProgram();
	ShaderProgram(std::string vertexCode, std::string fragmentCode);
	void initProgram(std::string vertexCode, std::string fragmentCode);
	void loadVertexShader(char *code);
	void loadFragmentShader(char *code);
	void linkProgram();

	void useProgram();

	GLuint getProgram() { return gProgram; };
	GLint getUniform( char const* name );
	GLint getAttribute( char const* name );
	virtual ~ShaderProgram();

private:
	GLuint gProgram;
};


class StandardShaderProgram: public ShaderProgram {
public:

	GLuint vertexPointer;
	GLuint colorPointer;
	GLuint mvpMatrixPointer;

	StandardShaderProgram(std::string vertexCode, std::string fragmentCode);
	void disable();
};


static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    debug_print("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        debug_print("after %s() glError (0x%x)\n", op, error);
    }
}

#endif /* SHADERPROGRAM_H_ */
