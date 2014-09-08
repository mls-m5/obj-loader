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
	Vec coord, normal, tex;
};

class ModelObject {
public:
	ModelObject(std::string filename = "", bool recalculateNormals = true);
	virtual ~ModelObject();


	static void init();

	void load(std::string filename, bool recalculateNormals = true);
	void render();

protected:
	std::vector<Vertex> vertices;
	std::vector<Vec> convexHull;
	unsigned int texture;

	void calculateNormals(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);
};

