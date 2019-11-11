
//
// Demo 01 - rotation using the mouse to rotate the bee. R key resets the bee back to the original position
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


// Function Prototypes

void init(int argc, char* argv[]);
void display(void);
void drawTexturedQuad(void);
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

	// Setup OpenGL window properties
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	// Create window
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Texture Mapping Demo");

	// Register display callback function
	glutDisplayFunc(display);

	// Register keyboard and mouse callback functions
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


	// Initialise OpenGL state

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set viewplane size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
	
	glMatrixMode(GL_MODELVIEW);


	// Load demo texture
	myTexture = fiLoadTexture("..\\..\\Common\\Resources\\Textures\\bumblebee.png");
}




// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	GUMatrix4 R = GUMatrix4::rotationMatrix(theta, theta, 0.0f);
	glLoadMatrixf((GLfloat*)&R);

	drawTexturedQuad();

	glutSwapBuffers();
}



void drawTexturedQuad(void) {

	glBindTexture(GL_TEXTURE_2D, myTexture);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-0.5f, -0.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-0.5f, 0.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(0.5f, -0.5f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(0.5f, 0.5f);

	glEnd();

	glDisable(GL_TEXTURE_2D);
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
