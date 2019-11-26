
//
// Week 11 - Demo 02 - Using Matrix Stacks
//

#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"
#include "Snowman.h"
#include <stack>


using namespace std;
using namespace CoreStructures;


// Globals

// Mouse input
float theta = 0.0f;
int mouse_x, mouse_y;
bool mDown = false;


// Matrix stack
stack<GUMatrix4> matrixStack;


//
// Demo model
//
float theta0 = 0.0f;
float theta1 = 0.0f;
float theta2 = 0.0f;
float theta1b = gu_radian * 45.0f;

const float quadLength = 0.4f;

GLuint rustTexture = 0;



// Function Prototypes

void init(int argc, char* argv[]);
void display(void);
void drawHierarchy(GUMatrix4& R);
void drawQuad(void);
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
	glutCreateWindow("Hierarchical Modelling 02 - Matrix Stacks");

	// Register callback functions
	glutDisplayFunc(display);
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutKeyboardFunc(keyDown); // Keyboard event handler


	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";
	}
	else {

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

	rustTexture = wicLoadTexture(wstring(L"Resources\\Textures\\rust1.jpg"));

	glEnable(GL_TEXTURE_2D);
}



// Rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GUMatrix4 R = GUMatrix4::identity();
	glLoadMatrixf((GLfloat*)&R);

	drawHierarchy(R);

	glutSwapBuffers();
}


// Draw example hierarchical object - pass in the current transform matrix so we can 'append' new transforms to this.
void drawHierarchy(GUMatrix4& R) {

	matrixStack.push(R);

	// Draw base base of arm

	R = R * GUMatrix4::rotationMatrix(0.0f, 0.0f, theta0);
	glLoadMatrixf((GLfloat*)&R);

	drawQuad();


	matrixStack.push(R);

	// Draw first segment

	R = R * GUMatrix4::translationMatrix(0.0f, quadLength, 0.0f) * GUMatrix4::rotationMatrix(0.0f, 0.0f, theta1);
	glLoadMatrixf((GLfloat*)&R);

	drawQuad();


	matrixStack.push(R);

	// Draw first branch
	R = R * GUMatrix4::translationMatrix(0.0f, quadLength, 0.0f) * GUMatrix4::rotationMatrix(0.0f, 0.0f, theta2);
	glLoadMatrixf((GLfloat*)&R);

	drawQuad();

	R = matrixStack.top();
	matrixStack.pop();


	matrixStack.push(R);

	// Draw second branch

	R = R * GUMatrix4::translationMatrix(0.0f, quadLength, 0.0f) * GUMatrix4::rotationMatrix(0.0f, 0.0f, theta1b);
	glLoadMatrixf((GLfloat*)&R);

	drawQuad();

	R = matrixStack.top();
	matrixStack.pop();


	R = matrixStack.top();
	matrixStack.pop();


	R = matrixStack.top();
	matrixStack.pop();
}



void drawQuad(void) {

	glBindTexture(GL_TEXTURE_2D, rustTexture);

	glBegin(GL_TRIANGLE_STRIP);

	glColor3ub(255, 255, 255);

	glTexCoord2f(0.4f, 1.0f);
	glVertex2f(-0.05f, 0.0f);

	glTexCoord2f(0.4f, 0.0f);
	glVertex2f(-0.05f, quadLength);

	glTexCoord2f(0.6f, 1.0f);
	glVertex2f(0.05f, 0.0f);

	glTexCoord2f(0.6f, 0.0f);
	glVertex2f(0.05f, quadLength);

	glEnd();
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

		theta0 += (float)dy * (3.142f * 0.05f);

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) {

	switch (key) {

	case 'r':
	case 'R':
		theta0 = 0.0f;
		glutPostRedisplay();
		break;

	case 'q':
		theta1 += gu_radian * 5.0f;
		glutPostRedisplay();
		break;

	case 'a':
		theta1 -= gu_radian * 5.0f;
		glutPostRedisplay();
		break;

	case 'w':
		theta2 += gu_radian * 5.0f;
		glutPostRedisplay();
		break;

	case 's':
		theta2 -= gu_radian * 5.0f;
		glutPostRedisplay();
		break;

	case 'e':
		theta1b += gu_radian * 5.0f;
		glutPostRedisplay();
		break;

	case 'd':
		theta1b -= gu_radian * 5.0f;
		glutPostRedisplay();
		break;
	}
}

#pragma endregion
