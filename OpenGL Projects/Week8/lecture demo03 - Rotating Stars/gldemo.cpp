
//
// Week 9 - Demo 03 - Rotating stars
//

#include "stdafx.h"
#include <random>
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"


using namespace std;
using namespace CoreStructures;


// Variable we'll use to animate (rotate) the star field
float theta = 0.0f;


// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;


// Shader program object
GLuint myShaderProgram;

GLuint locT; // Location of "T" uniform variable
GLuint locStarFieldTransform; // Location of "starFieldTransform" uniform variable



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


// VBO and VAO IDs for the star object
GLuint starPosVBO, starColourVBO, starTexCoordVBO, starIndicesVBO, starVAO;

#pragma endregion



const int NUM_STARS = 2000;

// Star instance information
struct star_instance {

	// Matrices to model the position, scale and orienation of a star
	GUMatrix4		pos;
	GUMatrix4		scale;
	GUMatrix4		rot;
	GUMatrix4		T; // pos * rot * scale

} stars[NUM_STARS];


// Store actual scales for the stars here - this allows us to bias the scale of the stars towards a certain size.  Here there's more change of a star with size 0.1 than there is a larger star of 0.2 for example.
float scale_prob[10] = { 0.025f, 0.025f, 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.1f, 0.15f, 0.2f };



// Function Prototypes

void init(int argc, char* argv[]);
void setupStarVAO(void);
void setupStarInstances(void);
void update(void);
void display(void);
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
	glutCreateWindow("Transformations Demo 02");

	// Register callback functions
	glutIdleFunc(update);
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

	glLineWidth(2.0f);


	// Load example shaders
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));

	// Get location of uniform variables "T" and "starFieldTransform" in 'myShaderProgram'.  We'll need these to give the uniforms values at render time (see the display function below).
	locT = glGetUniformLocation(myShaderProgram, "T");
	locStarFieldTransform = glGetUniformLocation(myShaderProgram, "starFieldTransform");

	// Setup star object using Vertex Buffer Objects (VBOs)
	setupStarVAO();

	// Create star field
	setupStarInstances();
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


void setupStarInstances(void) {

	random_device rd;

	// Random number engine - seed with a PRN (pseudo-random number) from the random_device rd.  This mean we get a different sequence of numbers every time we run the program.  If you want the same sequence of random numbers every time you run the program replace rd() with a constant value.
	mt19937 mt(rd());


	// Define the ranges we want random numbers generated in

	// Stars lie in the [-2.25, +2.25] range on (x, y) plane
	auto positionRange = uniform_real_distribution<float>(-2.25f, 2.25f);

	// Scale stars range - generate a random index in the range [0, 9] to access the scale_prob array
	auto scaleProbRange = uniform_int_distribution<int>(0, 9);


	for (int i = 0; i<NUM_STARS; ++i) {

		// Setup random position
		float x = positionRange(mt);
		float y = positionRange(mt);

		// Get scale from scale_prob array (setup so bias to certain size range for the stars around 0.1)
		float scale = scale_prob[scaleProbRange(mt)];

		// Calculate orientation bassed on position
		float theta = atan2f(y, x) - (gu_pi / 2.0f);

		// Setup matrices based on variables created above
		stars[i].pos = GUMatrix4::translationMatrix(x, y, 0.0f);
		stars[i].scale = GUMatrix4::scaleMatrix(scale, scale, 1.0f);
		stars[i].rot = GUMatrix4::rotationMatrix(0.0f, 0.0f, theta);

		// Combine matices into a single (model) transform the the current star instance.
		stars[i].T = stars[i].pos * stars[i].rot * stars[i].scale;
	}
}



// Update (animation) function
void update(void) {

	// Update orientation of the whole star field
	theta += 0.005f;
	glutPostRedisplay();
}



// Rendering functions
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// "Plug in" our shader into the GPU pipeline - we're now in the driving seat!!!!!  Our shaders now intercept and process our vertices as part of the GPU rendering pipeline!
	glUseProgram(myShaderProgram);

	// Calculate transform for the whole star field and upload to the shader
	GUMatrix4 starFieldMatrix = GUMatrix4::translationMatrix(0.0f, -1.0f, 0.0f) * GUMatrix4::rotationMatrix(0.0f, 0.0f, theta);

	glUniformMatrix4fv(locStarFieldTransform, 1, GL_FALSE, (GLfloat*)&starFieldMatrix);

	// Draw multiple star instances
	for (int i = 0; i<NUM_STARS; ++i) {

		// Setup matrix "T" in shader to store the transform for the current star
		glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&(stars[i].T));

		// Draw star
		drawStar_VAO();
	}

	// Present the current frame to the screen
	glutSwapBuffers();
}



// Draw star using VBOs and VAOs
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
}

#pragma endregion
