
#include "stdafx.h"
#include "Snowman.h"
#include "circle.h"

using namespace std;
using namespace CoreStructures;


Snowman::Snowman() {

	snowmanComponent = new Circle();

	// Load texture images
	bodyTexture = wicLoadTexture(wstring(L"Resources\\Textures\\snowman_body.jpg"));
	headTexture = wicLoadTexture(wstring(L"Resources\\Textures\\snowman_head.jpg"));

	// Load shaders that make up the snowmanShader program
	snowmanShader = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\texture_fragment_shader.txt"));

	// Setup uniform locations
	locT = glGetUniformLocation(snowmanShader, "T");
}



// Render snowman object.  x, y represent the position of the snowman's body, scale determines the scale of the snowman and orientation is the angle of the snowman (in degrees)
void Snowman::renderSnowman(float x, float y, float scale, float orientation) {

	// "Plug in" the snowman shader into the GPU
	glUseProgram(snowmanShader);


	// 1. Draw the main body

	// Create matrices based on input parameters
	GUMatrix4 bodyTransform = GUMatrix4::translationMatrix(x, y, 0.0f) *
		GUMatrix4::rotationMatrix(0.0f, 0.0f, orientation*gu_radian) *
		GUMatrix4::scaleMatrix(scale, scale, 1.0f);

	// Upload body transform to shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&bodyTransform);

	// Use the snow texture for the main body
	glBindTexture(GL_TEXTURE_2D, bodyTexture);

	// Draw the circle for the body
	snowmanComponent->render();



	// 2. draw head

	// - Setup the RELATIVE transformation from the centre of the body to where the head's origin will be
	// - Like the body, the head uses the same circle model so the origin will be in the middle of the head
	// - Offsets are calculated in terms of the parent object's modelling coordinates.  Any scaling, rotation etc. of the parent will be applied later in the matrix sequence.
	// - This makes relative modelling easier - we don't worry about what transforms have already been applied to the parent.
	GUMatrix4 body_to_head_transform = GUMatrix4::translationMatrix(0.0f, 1.25f, 0.0f) * GUMatrix4::scaleMatrix(0.65f, 0.65f, 1.0f);

	// Since we're working with a relative transform, we must multiply this with the parent objects's (that is, the body's) transform also
	// REMEMBER: THE RELATIVE TRANSFORM GOES LAST IN THE MATRIX MULTIPLICATION SO IT HAPPENS FIRST IN THE SEQUENCE OF TRANSFORMATIONS
	GUMatrix4 headTransform = bodyTransform * body_to_head_transform;

	// Upload the final head transform to the shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&headTransform);

	// Bind the head texture
	glBindTexture(GL_TEXTURE_2D, headTexture);

	// Draw the circle for the head
	snowmanComponent->render();
}
