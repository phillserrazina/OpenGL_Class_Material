
//
// Week 7 - Demo 02 - Vertex Array Objects
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


// Vertex Buffer Object IDs for the star object
GLuint starPosVBO, starColourVBO, starTexCoordVBO, starIndicesVBO;

// New ID - store the Vertex ARRAY Object for the star
GLuint starVAO;


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


// Function Prototypes

void init(int argc, char* argv[]);
void setupStarVAO(void);
void report_version(void);
void display(void);
void drawStar(void);
void drawStar_VAO(void);
void update(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);


int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	// Shut down COM
	shutdownCOM();

	return 0;
}


void init(int argc, char* argv[]) {

	// Initialise COM so we can use Windows Imaging Component
	initCOM();

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(4, 4);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Vertex Array Objects");

	// Register callback functions
	glutDisplayFunc(display);
	glutIdleFunc(update);
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
	report_version();

	// Report maximum number of vertex attributes
	GLint numAttributeSlots;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributeSlots);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << numAttributeSlots << endl;


	// Initialise OpenGL...

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glLineWidth(4.0f);

	// Shader setup - more on this next week!!!
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));


	// Setup star object using Vertex Buffer Objects (VBOs)
	setupStarVAO();
}


void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
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

	// setup the star texture coordinate VBO
	glGenBuffers(1, &starTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, starTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(starTextureCoords), starTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);

	// setup star vertex index array
	glGenBuffers(1, &starIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(starVertexIndices), starVertexIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}




// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawStar_VAO();

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


void drawStar_VAO(void) {

	// "Plug in" our shader into the GPU pipeline - we're now in the driving seat!!!!!  Our shaders now intercept and process our vertices as part of the GPU rendering pipeline!
	glUseProgram(myShaderProgram);

	// Bind star VAO and draw
	glBindVertexArray(starVAO);
	glDrawElements(GL_LINE_LOOP, 10, GL_UNSIGNED_BYTE, (GLvoid*)0);
}


void update(void) {

	glutPostRedisplay();
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
