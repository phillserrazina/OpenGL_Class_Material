
#pragma once

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"


class Circle {

	// Variables to represent the VBO and VAO of the circle object
	GLuint circleVAO, vertexPositionVBO, texCoordVBO;

public:

	// Default constructor - setup circle with unit radius
	Circle();

	// Render circle - all relevant transformations are assumed to be setup before calling this function
	void render(void);

};
