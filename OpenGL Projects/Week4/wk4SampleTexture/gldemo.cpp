
//
// Week 4 - Using textures in OpenGL
// Refer to Tutorial 4 on BB for help
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"


using namespace std;
using namespace CoreStructures;


// Globals

// Declare your Texture here using GLuint



// Function Prototypes

void init(int argc, char* argv[]);
void display(void);
void drawTexturedQuad(void);


int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	return 0;
}


void init(int argc, char* argv[]) {



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


	// Load demo texture. refer to 3.2 tutorial exercise for help
	myTexture = blah blah
}


// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawTexturedQuad();

	glutSwapBuffers();
}



void drawTexturedQuad(void) {

	
}


