/*
 * shaderProgram.cpp
 *
 *  Created on: 16 mar 2014
 *      Author: mattias
 */

#include "shaderprogram.h"




GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, 0);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
            	char *buf = new char[infoLen];
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, 0, buf);
                    debug_print("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    debug_print("Shader code: \n%s\n", pSource);
                    delete buf;
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}


GLuint createProgram(std::string pVertexSource, std::string pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource.c_str());
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource.c_str());
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
            	char *buf = new char[bufLength];
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, 0, buf);
                    debug_print("Could not link program:\n%s\n", buf);
                    delete buf;
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

ShaderProgram::ShaderProgram():
		gProgram(0){

}

void ShaderProgram::initProgram(std::string vertexCode, std::string fragmentCode) {
	gProgram = createProgram(vertexCode, fragmentCode);
}

GLint ShaderProgram::getUniform(char const* name) {
	GLint ret;
    ret = glGetUniformLocation(gProgram, name);

    checkGlError(name);
    return ret;
}

GLint ShaderProgram::getAttribute(char const* name) {
	GLint ret;
	ret = glGetAttribLocation(gProgram, name);

	checkGlError(name);

	return ret;
}

ShaderProgram::ShaderProgram(std::string vertexCode, std::string fragmentCode) {
	initProgram(vertexCode, fragmentCode);
}

ShaderProgram::~ShaderProgram() {
	if (gProgram){
		glDeleteProgram(gProgram);
	}
}

StandardShaderProgram::StandardShaderProgram(std::string vertexCode, std::string fragmentCode) :
		ShaderProgram(vertexCode, fragmentCode) {

	vertexPointer = getAttribute("vPosition");
	colorPointer = getAttribute("vColor");
	mvpMatrixPointer = getUniform("mvp_matrix");

}

void StandardShaderProgram::disable() {
	if (vertexPointer != -1){
		glEnableVertexAttribArray(vertexPointer);
	}
	if (colorPointer != -1){
		glEnableVertexAttribArray(colorPointer);
	}
}

void ShaderProgram::useProgram() {
	glUseProgram(gProgram);
}
