#ifndef SHAPE_H
#define SHAPE_H

#include "stdafx.h"

using namespace std;

struct Vector3 
{
private:
	float _x;
	float _y;
	float _z;

public:
	Vector3(float xPos, float yPos, float zPos) : _x(xPos), _y(yPos), _z(zPos) { }

	float x() { return _x; }
	float y() { return _y; }
	float z() { return _z; }
};

class Shape 
{
protected:
	Vector3 pos;
	Vector3 rot;
	Vector3 scale;

	GLuint shapePosVBO;
	GLuint shapeColourVBO;
	GLuint shapeTexCoordVBO;
	GLuint shapeIndicesVBO;

	static GLfloat vertices[];
	static GLubyte colours[];
	static float textureCoord[];
	static GLubyte vertexIndicies[];

public:
	Shape(Vector3, Vector3, Vector3);

	void Render();
};

#endif SHAPE_H