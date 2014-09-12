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
using std::istringstream;

static ShaderProgram *program;

struct {
	GLuint vertexPointer;
	GLuint normalPointer;
	GLuint matrix;

} programPointers;

ModelObject::ModelObject(string filename, bool recalculateNormals):
	texture(0) {
	if (!filename.empty()){
		load(filename, recalculateNormals);
	}
}

ModelObject::~ModelObject() {
}

struct Face {
	int vi[4];
	int ni[4];
	int ti[4];
};

int toInt(string input){
	istringstream ss(input);
	int ret;
	ss >> ret;
	return ret;
}

Face parseFace(std::string line){
	std::string label;
	Face face;

	istringstream ss(line);

	ss >> label;

	for (int i = 0; i < 4; ++i){
		string tmp;
		ss >> tmp;

		if (tmp.find('/') == string::npos){
			face.vi[i] = toInt(tmp);
			face.ti[i] = 0;
			face.ni[i] = 0;
		}
		else {
			istringstream ss2(tmp);

			string tmp2;
			if(std::getline(ss2, tmp2, '/')){
				face.vi[i] = toInt(tmp2);
			}
			else{
				face.vi[i] = 0;
			}
			std::getline(ss2, tmp2, '/');
			if(!tmp2.empty()){
				face.ti[i] = toInt(tmp2);
			}
			else {
				face.ti[i] = 0;
			}
			std::getline(ss2, tmp2, '/');
			if(!tmp2.empty()){
				face.ni[i] = toInt(tmp2);
			}
			else {
				face.ni[i] = 0;
			}
		}
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
	istringstream ss(line);
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

void ModelObject::calculateNormals(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
	//Calculate normals
	for (int i = 0; i < indices.size(); i += 4) {
		auto& v1 = vertices[indices[i] - 1];
		auto& v2 = vertices[indices[i + 1] - 1];
		auto& v3 = vertices[indices[i + 2] - 1];
		auto& v4 = vertices[indices[i + 3] - 1];
		auto n = calculateTriangleNormal(v1.coord, v2.coord, v3.coord);
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

//Dummy function to create texture
GLuint loadTexture(std::string filename){
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width = 128, height = 128;
	unsigned char data [width * height * 3];
	for (int i = 0; i < width * height * 3; ++i){
		data[i] = i % 255;
	}

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}

void ModelObject::load(std::string filename, bool recalculateNormals) {
	std::vector <vec> vert, hullvert, tex, norm;
	std::vector <Face> faces;
	std::vector <unsigned int> vind, nind, tind, hullind;
	std::vector <Vertex> tempVertices;

	bool readingHull = false;

	vert.reserve(100);
	hullvert.reserve(100);
	vind.reserve(100);
	norm.reserve(100);
	hullind.reserve(100);
	nind.reserve(100);
	tind.reserve(100);
	faces.reserve(100);

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
				norm.push_back(v);
			}
			else if(line[1] == 't'){
				tex.push_back(v);
			}
			else{
				vert.push_back(v);
			}

			if (readingHull){
				if (line[1] != 'n' && line[1] != 't'){ //Ignoring normal and texture vertices
//					hullind.push_back(vert.size()); //Note that vertex at that index is a hull vertex
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
					vind.push_back(f.vi[i]);
					nind.push_back(f.ni[i]);
					tind.push_back(f.ti[i]);
				}
				faces.push_back(f);
			}

			break;
		}

		case '#':
		default:
				break;
		}
	}

	//Remove vertices only used for convex hull
//	for (int i = hullind.size() - 1; i >= 0; --i){
//		vert.erase(vert.begin() + hullind[i]);
//		for (auto &ind: vind){
//			if (ind >= hullind[i]){
//				--ind;
//			}
//		}
////		for (auto &ind: tind){
////			if (ind >= hullind[i]){
////				--ind;
////			}
////		}
//	}

	tempVertices.resize(vert.size());
	for (int i = 0; i < vert.size(); ++i){
		tempVertices[i].coord = vert[i];
	}

//	indices = vind;

	if (recalculateNormals){
		//Calculate normals
		calculateNormals(tempVertices, vind);
	}


	vertices.resize(tind.size());

	for (int i = 0; i < vind.size(); ++i){
		auto vi = vind[i];
		auto ti = tind[i];
		auto ni = nind[i];
		Vertex v = tempVertices[vi - 1];
		if (ti > 0){
			v.tex = tex[ti - 1];
		}
		if (!recalculateNormals){
			if (ni > 0){
				v.normal = norm[ni - 1];
			}
		}
		vertices.push_back(v);
	}

	convexHull = hullvert;

	texture = loadTexture("apa.png");
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
	if (texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	GLfloat lightpos[] = {10. * c, -10. * s, 0., 0.};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	GLfloat lightcolor[] = {.1, .3, .0, 0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor);

//	program->useProgram();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), &vertices[0].coord.x);
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), &vertices[0].normal.x);
    glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), &vertices[0].tex.x);
//    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);
    glDrawArrays(GL_QUADS, 0, vertices.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, sizeof(Vec), &convexHull[0].x);
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
