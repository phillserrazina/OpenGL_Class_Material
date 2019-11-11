
//
// Week 6 - Demo 01 - Retained mode rendering 01 (Packed Arrays)
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"


using namespace std;
using namespace CoreStructures;


// Globals

// Mouse input (rotation) example
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;


// Textures
GLuint myTexture = 0;




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

	255, 0, 0,
	255, 255, 0,
	0, 255, 0,
	0, 255, 255,
	0, 0, 255,
	128, 45, 200,
	0, 255, 0,
	255, 0, 255,
	255, 128, 255,
	255, 128, 128
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




// Function Prototypes

void init(int argc, char* argv[]);
void report_version(void);
void display(void);
void drawStar(void);
void drawStarPackedArray(void);
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

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Vertex Arrays Demo");

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
	report_version();


	// Initialise OpenGL...

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set viewplane size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	glLineWidth(4.0f);

	// Load demo texture...
	myTexture = fiLoadTexture("..\\..\\Common\\Resources\\Textures\\bumblebee.png");


	// Enable Vertex Arrays

	// Tell OpenGL to expect vertex position information from an array
	glEnableClientState(GL_VERTEX_ARRAY);

	// Tell OpenGL to expect vertex colour information from an array
	glEnableClientState(GL_COLOR_ARRAY);

	// Tell OpenGL to expect texture coordinate information from an array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}



// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	GUMatrix4 R = GUMatrix4::rotationMatrix(0.0, 0.0f, theta);
	glLoadMatrixf((GLfloat*)&R);

	drawStar();

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


// Draw a 5-pointed star using vertex arrays
void drawStarPackedArray(void) {

	glVertexPointer(2, GL_FLOAT, 0, starVertices);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, starColors);
	glTexCoordPointer(2, GL_FLOAT, 0, starTextureCoords);

	glDrawArrays(GL_LINE_LOOP, 0, 10);
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
