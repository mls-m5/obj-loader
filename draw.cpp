#include "draw.h"
#include <math.h>
#include <vector>

#include "shaderprogram.h"



static GLfloat transformMatrix[16];
static double screenWidth, screenHeight;

inline void identityMatrix(GLfloat *matrix){
	for (int i = 0; i < 16; ++i){
		matrix[i] = 0;
	}
	for (int i = 0; i < 4; ++i){
		matrix[i + i*4] = 1;
	}
}

void modelTransform(unsigned int pointer, Vec p, double a, double scaleX, double scaleY){
	identityMatrix(transformMatrix);

	auto s = sin(a), c = cos(a);

	transformMatrix[0] = c * scaleX / screenWidth * 2;
	transformMatrix[1] = s * scaleX / screenWidth * 2;
	transformMatrix[4] = -s * scaleY / screenHeight * 2;
	transformMatrix[5] = c * scaleY / screenHeight * 2;

	transformMatrix[12] = p.x / screenWidth * 2 - 1.;
	transformMatrix[13] = p.y / screenHeight * 2 - 1.;
	transformMatrix[14] = p.z;

    glUniformMatrix4fv(pointer, 1, GL_FALSE, transformMatrix);
}

void resetTransform(unsigned int pointer){
	identityMatrix(transformMatrix);
    glUniformMatrix4fv(pointer, 1, GL_FALSE, transformMatrix);
}

void setModelTransform(float* m) {
	for (int i = 0; i < 16; ++i){
		transformMatrix[i] = m[i];
	}
}

void setDimensions(double width, double height){
	screenWidth = width;
	screenHeight = height;
}

bool initDrawModule(double width, double height) {

	setDimensions(width, height);
	return false;
}

void QuitDrawModule() {
}
