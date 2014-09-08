/*
 * modelobject.h
 *
 *  Created on: 5 sep 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <vector>
#include <string>

#include "draw.h"
//struct vec {
//	float x, y, z;
//};

struct Vertex{
	Vec coord, normal;
};

class ModelObject {
public:
	ModelObject();
	virtual ~ModelObject();


	static void init();

	void load(std::string filename);
	void render();

	std::vector<Vertex> vertices;
	std::vector<Vec> convexHull;
	std::vector<unsigned int> indices;

protected:
	void calculateNormals();
};

