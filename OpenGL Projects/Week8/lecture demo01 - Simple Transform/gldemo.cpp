
//
// Week 9 - Demo 01 - Simple Transform
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"


using namespace std;
using namespace CoreStructures;


// Globals

// Mouse input (rotation) example
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;


// Shader program object
GLuint myShaderProgram;
GLuint locT; // location of "T" uniform variable in myShaderProgram


#pragma region Star model

// Packed vertex arrays for the star object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat starVertices[] = {

	0.0, 0.25f,
	0.1f, 0.1f,
	0.25f, 0.08f,
	0.15f, -0.05f,
	0.25f, -0.25f,
	0.0, -0.125f,
	-0.25f, -0.25f,
	-0.15f, -0.05f,
	-0.25f, 0.08f,
	-0.1f, 0.1f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte starColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255,
	0, 0, 255, 255,
	128, 45, 200, 255,
	0, 255, 0, 255,
	255, 0, 255, 255,
	255, 128, 255, 255,
	255, 128, 128, 255
};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float starTextureCoords[] = {

	0.5f, 1.0f,
	0.6f, 0.7f,
	1.0f, 0.7f,
	0.8f, 0.4f,
	0.95f, 0.0f,
	0.0f, 0.3f,
	-0.95f, 0.0f,
	-0.8f, 0.4f,
	-1.0f, 0.7f,
	-0.6f, 0.7f
};

// 4) Index Array - Store indices to star vertices - this determines the order the vertices are to be processed
static GLubyte starVertexIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

// VBOs and VAO for the star object
GLuint starPosVBO, starColourVBO, starTexCoordVBO, starIndicesVBO, starVAO;

#pragma endregion


// Function Prototypes

void init(int argc, char* argv[]);
void setupStarVAO(void);
void display(void);
void drawStar(void);
void drawStar_VAO(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);


int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	shutdownCOM();

	return 0;
}


void init(int argc, char* argv[]) {

	// Initialise COM so we can use Windows Imaging Component
	initCOM();

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Transformations Demo 01");

	// Register callback functions
	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseMove);


	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	} else {

		cout << "GLEW could not be initialised\n";
		throw;
	}


	// Example query OpenGL state (get version number)
	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";

	

	// Initialise OpenGL...

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glLineWidth(4.0f);

	// Load example shader
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));

	// Get uniform location of "T" variable in shader program (we'll use this in the display function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");

	// Setup star object using Vertex Buffer Objects (VBOs)
	setupStarVAO();
}


void setupStarVAO(void) {

	// Create and bind the vertex array object.  This remembers the vertex buffer object bindings below so we don't have to specify them at render time.
	glGenVertexArrays(1, &starVAO);
	glBindVertexArray(starVAO);

	// Setup the star position VBO
	glGenBuffers(1, &starPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, starPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(starVertices), starVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Setup the star colour VBO
	glGenBuffers(1, &starColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, starColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(starColors), starColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Setup the star texture coordinate VBO
	glGenBuffers(1, &starTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, starTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(starTextureCoords), starTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// Setup star vertex index array
	glGenBuffers(1, &starIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(starVertexIndices), starVertexIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}




// Main rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// "Plug in" our shader into the GPU pipeline - we're now in the driving seat!!!!!  Our shaders now intercept and process our vertices as part of the GPU rendering pipeline!
	glUseProgram(myShaderProgram);

	// Set the uniform matrix variable in the shader representing the transformation to apply to the star vertices
	GUMatrix4 T = GUMatrix4::translationMatrix(0.5f, 0.0f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);

	// Draw the star in the new position
	drawStar_VAO();



	// Setup a new matrix and draw a second star
	T = GUMatrix4::translationMatrix(-0.5f, 0.5f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);
	
	drawStar_VAO();



	// Present the new frame to the screen
	glutSwapBuffers();
}


// Draw a 5-pointed star using immediate mode rendering
void drawStar(void)
{
	glBegin(GL_LINE_LOOP);

	glColor3ub(255, 0, 0);
	glTexCoord2f(0.5f, 1.0f);
	glVertex2f(0.0, 0.25f);
	
	glColor3ub(255, 255, 0);
	glTexCoord2f(0.6f, 0.7f);
	glVertex2f(0.1f, 0.1f);
	
	glColor3ub(0, 255, 0);
	glTexCoord2f(1.0f, 0.7f);
	glVertex2f(0.25f, 0.08f);

	glColor3ub(0, 255, 255);
	glTexCoord2f(0.8f, 0.4f);
	glVertex2f(0.15f, -0.05f);
	
	glColor3ub(0, 0, 255);
	glTexCoord2f(0.95f, 0.0f);
	glVertex2f(0.25f, -0.25f);
	
	glColor3ub(128, 45, 200);
	glTexCoord2f(0.0f, 0.3f);
	glVertex2f(0.0f, -0.125f);
	
	glColor3ub(0, 255, 0);
	glTexCoord2f(-0.95f, 0.0f);
	glVertex2f(-0.25f, -0.25f);

	glColor3ub(255, 0, 255);
	glTexCoord2f(-0.8f, 0.4f);
	glVertex2f(-0.15f, -0.05f);

	glColor3ub(255, 128, 255);
	glTexCoord2f(-1.0f, 0.7f);
	glVertex2f(-0.25f, 0.08f);

	glColor3ub(255, 128, 128);
	glTexCoord2f(-0.6f, 0.7f);
	glVertex2f(-0.1f, 0.1f);

	glEnd();
}


// Draw star with retained-mode rendering using VBOs and VAOs
void drawStar_VAO(void) {

	glBindVertexArray(starVAO);

	glDrawElements(GL_LINE_LOOP, 10, GL_UNSIGNED_BYTE, (GLvoid*)0);
}



#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	if (mDown) {

		int dx = x - mouse_x;
		int dy = y - mouse_y;

		theta += (float)dy * (3.142f * 0.01f);

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) {

	if (key == 'r') {

		theta = 0.0f;
		glutPostRedisplay();
	}
}

#pragma endregion
