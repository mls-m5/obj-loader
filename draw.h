/*
 * draw.h
 *
 *  Created on: 19 aug 2014
 *      Author: mattias
 */

#ifndef DRAW_H_
#define DRAW_H_



//struct vec {
//	vec(double x, double y, double z): x(x), y(y), z(z) {}
//	vec(double x, double y): x(x), y(y), z(0) {}
//	vec(): x(0), y(0), z(0) {}
//	double x; double y; double z;
//};

#include "vec.h"
typedef Vec vec;

enum DrawStyle {
	DRAW_STYLE_FILLED,
	DRAW_STYLE_LINES
};

bool initDrawModule(double width, double height);
void QuitDrawModule();

//void modelTransform(unsigned int pointer, Vec p, double a, double scale = 1);
void modelTransform(unsigned int pointer, Vec p, double a, double scaleX, double scaleY);
void setModelTransform(float *m);
void resetTransform(unsigned int poniter);
void setCam(Vec p, double a);
void drawSquare(Vec p, double a, double scaleX, double scaleY, DrawStyle);
void drawElipse(Vec p, double a, double scaleX, double scaleY, DrawStyle);
void drawGraph(Vec p, double a, double sx, double sy, float *v, int size);
void setDimensions(double width, double height);



#endif /* DRAW_H_ */
