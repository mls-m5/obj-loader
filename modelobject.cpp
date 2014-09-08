/*
 * ModelObject.cpp
 *
 *  Created on: 5 sep 2014
 *      Author: Mattias Larsson Sköld
 */

#include "modelobject.h"
#include "shaderprogram.h"
#include <fstream>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

static ShaderProgram *program;

struct {
	GLuint vertexPointer;
	GLuint normalPointer;
	GLuint matrix;

} programPointers;

ModelObject::ModelObject() {
}

ModelObject::~ModelObject() {
}

struct Face {
	int vi[4];
	int ni[4];
};

Face parseFace(std::string line){
	std::string label;
	Face face;

	std::stringstream ss(line);

	ss >> label;

	for (int i = 0; i < 4; ++i){
		string tmp;
		ss >> tmp;

		for (auto &it: tmp){
			if (it == '/'){
				it = ' ';
			}
		}

		std::stringstream ss2(tmp);
		ss2 >> face.vi[i] >> face.ni[i];
	}

	return face;
}


vec parseVec(std::string line){
	std::string label;
	vec v;
	std::stringstream ss(line);

	ss >> label >> v.x >> v.y >> v.z;
	return v;
}

Vec calculateTriangleNormal(Vec v1, Vec v2, Vec v3){
	Vec x1 = v2 - v1;
	Vec x2 = v3 - v1;
	return x1.cross(x2); //.normalize();
}

bool parseHullState(string line){
	stringstream ss(line);
	string label, name;
	const string hullKeyword = "hull";
	ss >> label >> name;

	for (int i = 0; i < hullKeyword.size(); ++i) {
		if (name[i] != hullKeyword[i]){
			return false;
		}
	}
	return true;
}

void ModelObject::calculateNormals() {
	//Calculate normals
	for (int i = 0; i < indices.size(); i += 4) {
		auto& v1 = vertices[indices[i]];
		auto& v2 = vertices[indices[i + 1]];
		auto& v3 = vertices[indices[i + 2]];
		auto& v4 = vertices[indices[i + 3]];
		Vec n = calculateTriangleNormal(v1.coord, v2.coord, v3.coord);
		v1.normal += n;
		v2.normal += n;
		v3.normal += n;
		n = calculateTriangleNormal(v1.coord, v3.coord, v4.coord);
		v1.normal += n;
		v3.normal += n;
		v4.normal += n;
	}
	for (auto& v : vertices) {
		v.normal.normalize();
	}
}

void ModelObject::load(std::string filename) {
	std::vector <vec> vert, hullvert;
	std::vector <unsigned int> vind, hull;

	bool readingHull = false;

	vert.reserve(100);
	hullvert.reserve(100);
	vind.reserve(100);
	hull.reserve(100);

	cout << "laddar " << filename << endl;

	std::ifstream file(filename);

	std::string line;

	if (file.is_open()) {
		cout << "filen är öppen" << endl;
	}
	else{
		cout << "filen är inte öppen!" << endl;
	}

	while (!file.eof()){
		getline(file, line);
		switch (line[0]) {
		case 'o':
			cout << "object " << line << endl;
			readingHull = parseHullState(line);
			cout << "hullstate " << readingHull << endl;
			break;
		case 'v':
		{
			auto v = parseVec(line);

			if (line[1] == 'n'){
//				norm.push_back(v);
			}
			else{
				vert.push_back(v);
			}

			if (readingHull){
				if (line[1] != 'n'){
					hull.push_back(vert.size()); //Note that vertex at that index is a hull vertex
					hullvert.push_back(v);
				}
			}

			break;
		}
		case 'f':
		{
			if (readingHull == false) {
				Face f = parseFace(line);

				for (int i = 0; i < 4; ++i){
					vind.push_back(f.vi[i] - 1);
				}
			}

			break;
		}

		case '#':
		default:
				break;
		}
	}

	//Remove vertices only used for convex hull
	for (int i = hull.size() - 1; i >= 0; --i){
		vert.erase(vert.begin() + hull[i]);
		for (auto &ind: vind){
			if (ind >= hull[i]){
				--ind;
			}
		}
	}

	vertices.resize(vert.size());
	for (int i = 0; i < vert.size(); ++i){
		vertices[i].coord = vert[i];
	}

	indices = vind;

	//Calculate normals
	calculateNormals();
	convexHull = hullvert;
}

void ModelObject::init() {
	std::fstream vertexProgramFile("vertex.sl");
	std::fstream fragmentProgramFile("fragment.sl");

	std::string vertex((std::istreambuf_iterator<char>(vertexProgramFile)),
	                 std::istreambuf_iterator<char>());
	std::string fragment((std::istreambuf_iterator<char>(fragmentProgramFile)),
	                 std::istreambuf_iterator<char>());
	program = new ShaderProgram(vertex, fragment);
	programPointers.vertexPointer = program->getAttribute("vPosition");
	programPointers.normalPointer = program->getAttribute("vNormal");
	programPointers.matrix = program->getUniform("mvp_matrix");

}

Vertex v = {{0,0,0},{0,0,0}};
int *index = {0};

void ModelObject::render() {
	static float angle = 0;
	glPushMatrix();
	glScalef(.2, .2, .2);
	angle ++;
	glRotatef(-45, 1, 0, 0);
	glRotatef(angle, 0,0,1);
	auto s = sin(angle / 180. * 3.1415);
	auto c = cos(angle / 180. * 3.1415);
//
//    glBegin(GL_QUADS);
//    glColor4f(1, 1, 1, 1);
//
//    Vec l(c, s, .1);
//    l.normalize();
//
//    for (int i: indices){
//    	auto &v = vertices[i];
//    	auto color = vertices[i].normal * l;
//
//    	if(v.normal.z > 0){
////    		glColor3f(v.normal.z * c, v.normal.z / 2 * s, v.normal.z / 2);
//
//    		glColor3f(color / 3., color / 3., color / 2.);
//    	}
//    	else{
//    		glColor3f(0, 0,0);
//    	}
//    	glVertex3dv(&v.coord.x);
//    }
//    glEnd();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightpos[] = {10. * c, -10. * s, 0., 0.};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	GLfloat lightcolor[] = {.1, .3, .1};
	glLightfv(GL_LIGHT0, GL_COLOR, lightcolor);

//	program->useProgram();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices[0].coord.x);
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices[0].normal.x);
    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &convexHull[0].x);
    glDrawArrays(GL_POINTS, 0, convexHull.size());
    glDisableClientState(GL_VERTEX_ARRAY);


//    glBegin(GL_LINES);
//    for (auto &v: vertices){
//    	vec v2 = {v.coord.x + v.normal.x, v.coord.y + v.normal.y, v.coord.z + v.coord.z};
//    	glVertex3dv(&v.coord.x);
//    	glVertex3dv(&v2.x);
//    }
//    glEnd();

//	glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices[0].coord.x);
//	glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices[0].normal.x);
//	program->useProgram();
//	modelTransform(programPointers.matrix, Vec(), 0, 1, 1);
//    glVertexAttribPointer(programPointers.vertexPointer, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &vertices[0].coord);
//    glEnableVertexAttribArray(programPointers.vertexPointer);
//
////    glVertex3f(0,0,0);
////    glVertex3f(.5,0,0);
////
////    glVertexAttribPointer(programPointers.nermalPointer, 4, GL_FLOAT, GL_FALSE, 0, gCometColors);
////    glEnableVertexAttribArray(shaderColorPointer);
////
//    glDrawArrays(GL_POINTS, 0, vertices.size());
//    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);

    glPopMatrix();
}
