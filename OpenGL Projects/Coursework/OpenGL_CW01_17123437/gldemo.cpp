
//
// renders our scene made up of shapes using VBO's, multiple textures, translation and two shaders
//

#include "stdafx.h"
#include <stack>
#include <cmath>
#include <cstdlib>
#include <ctime>


using namespace std;
using namespace CoreStructures;


// Globals

GLuint locT; // location of "T" uniform variable in myShaderProgram
GLuint locT2;


#pragma region Textures
GLuint myGrassTexture = 0;
GLuint mySkyTexture = 1;
GLuint myRoadTexture = 2;
GLuint myRoadLineTexture = 3;
#pragma endregion

#pragma region Mouse Variables
// Mouse input (rotation) example
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;
#pragma endregion

#pragma region Shaders
GLuint myShaderProgram;				// Shader program object for applying textures to our shapes
GLuint myShaderProgramNoTexture;	// Second shader progam object for non textured shapes such as our rainbow star
#pragma endregion

//used to adjust length of segments in hierarchical cactus model
float segmentLength = 1;
//matrix stack used to store transformation matrices for hierarchical model (this allows the user to move back and forth around a complex hierarchical model more easily) 
stack<GUMatrix4> matrixStack;
//used by the rotation matrices setup in the renderCactus() function
float cactusTheta[3] = { 0, 0, 0 };

float carXPos = 0;
float carYPos = 0;
float carWheelRot = 0;

const int NUM_OF_CLOUDS = 10;
float cloudScales[NUM_OF_CLOUDS];
float cloudStartingX[NUM_OF_CLOUDS];
float cloudStartingY[NUM_OF_CLOUDS];

#pragma region Grass VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint grassPosVBO, grassColourVBO, grassTexCoordVBO, grassIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat grassVertices[] = {

	-1.0f, -0.5f,
	-1.0f, 0.75f,
	1.0f, -0.5f,
	1.0f, 0.75f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte grassColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255
	
};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float grassTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte grassVertexIndices[] = { 0, 1, 2, 3 };
#pragma endregion

#pragma region Sky VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint skyPosVBO, skyColourVBO, skyTexCoordVBO, skyIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat skyVertices[] = {

	-1.0, -0.3f,
	-1.0f, 0.5f,
	1.0f, -0.3f,
	1.0f, 0.5f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte skyColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255

};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float skyTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte skyVertexIndices[] = { 0, 1, 2, 3 };
#pragma endregion

#pragma region Road VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint roadPosVBO, roadColourVBO, roadTexCoordVBO, roadIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat roadVertices[] = {

	-1.0, -0.25f,
	-1.0f, 0.25f,
	1.0f, -0.25f,
	1.0f, 0.25f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte roadColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255

};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float roadTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte roadVertexIndices[] = { 0, 1, 2, 3 };
#pragma endregion

#pragma region Road Line VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint roadLinePosVBO, roadLineColourVBO, roadLineTexCoordVBO, roadLineIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat roadLineVertices[] = {

	-0.08, -0.03f,
	-0.095f, 0.03f,
	0.08f, -0.03f,
	0.065f, 0.03f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte roadLineColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255

};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float roadLineTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte roadLineVertexIndices[] = { 0, 1, 2, 3 };
#pragma endregion

#pragma region Car VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint carPosVBO, carColourVBO, carTexCoordVBO, carIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat carVertices[] = {

	-0.2f, 0.0f,	// 0
	0.3f, 0.0f,		// 1 
	0.3f, 0.1f,		// 2
	0.15f, 0.1f,	// 3 
	0.15f, 0.25f,	// 4
	-0.1f, 0.25f,	// 5
	-0.2f, 0.07f,	// 6
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte carColors[] = {

	254, 226, 62, 255,
	254, 226, 62, 255,
	254, 226, 62, 255,
	254, 226, 62, 255,
	254, 226, 62, 255,
	254, 226, 62, 255,
	254, 226, 62, 255,

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte carVertexIndices[] = { 0, 1, 2, 3, 4, 5, 6 };
#pragma endregion

#pragma region Car Wheel VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint carWheelPosVBO, carWheelColourVBO, carWheelTexCoordVBO, carWheelIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat carWheelVertices[] = {

	-0.05f, 0.0f,	// 0
	0.05f, 0.0f,	// 1 
	0.1f, 0.1f,		// 2
	0.05f, 0.2f,	// 3 
	-0.05f, 0.2f,	// 4
	-0.1f, 0.1f,	// 5
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte carWheelColors[] = {

	0, 0, 0, 255,
	0, 0, 0, 255,
	0, 0, 0, 255,
	0, 0, 0, 255,
	0, 0, 0, 255,
	0, 0, 0, 255,

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte carWheelVertexIndices[] = { 0, 1, 2, 3, 4, 5 };
#pragma endregion

#pragma region Cloud VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint cloudPosVBO, cloudColourVBO, cloudTexCoordVBO, cloudIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat cloudVertices[] = {

	// BOTTOM
	-0.2f, 0.0f,		// 1
	-0.185f, -0.03f,	// 2
	-0.16f, -0.05f,		// 3
	-0.14f, -0.05f,		// 4
	-0.115f, -0.03f,	// 5
	-0.1f, 0.0f,		// 6

	-0.075f, -0.035f,	// 7
	-0.03f, -0.06f,		// 8
	0.03f, -0.06f,		// 9
	0.075f, -0.035f,	// 10

	0.1f, 0.0f,			// 11
	0.115f, -0.03f,		// 12
	0.14f, -0.05f,		// 13
	0.16f, -0.05f,		// 14
	0.185f, -0.03f,		// 15
	0.2f, 0.0f,			// 16

	// TOP
	0.185f, 0.07f,		// 17
	0.16f, 0.1f,		// 18
	0.14f, 0.1f,		// 19
	0.115f, 0.07f,		// 20
	0.1f, 0.04f,			// 21

	0.075f, 0.09f,		// 22
	0.03f, 0.16f,		// 23
	-0.03f, 0.16f,		// 24
	-0.075f, 0.09f,	// 25

	-0.1f, 0.04f,		// 26
	-0.115f, 0.07f,		// 27
	-0.14f, 0.1f,		// 28
	-0.16f, 0.1f,		// 29
	-0.185f, 0.07f,		// 30
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte cloudColors[] = {

	// BOTTOM
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,

	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,

	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,

	// TOP
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,

	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,

	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150,
	255, 255, 255, 150
};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte cloudVertexIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
										11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
										21, 22, 23, 24, 25, 26, 27, 28, 29 };
#pragma endregion

#pragma region Function Prototypes
void init(int argc, char* argv[]);

void setupGrassVBO(void);
void setupRoadVBO(void);
void setupSkyVBO(void);
void setupRoadLineVBO(void);
void setupCarVBO(void);
void setupCarWheelVBO(void);
void setupCloudVBO(void);

void report_version(void);
void display(void);

void drawTexturedQuadVBO_Grass(void);
void drawTexturedQuadVBO_Road(void);
void drawTexturedQuadVBO_Sky(void);
void drawTexturedQuadVBO_RoadLine(float);
void drawVBO_Car(void);
void drawVBO_CarWheel(float);
void drawVBO_Cloud(int);

void handlePositions(void);
void renderCar(void);
void renderRoad(void);
void renderScene(void);

//hierarchical modelling function prototypes
void renderCactus(GUMatrix4&);
void renderSmallCactusSegment(void);
void renderBigCactusSegment(void);

void update(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);

//void setupVBO(GLuint, GLuint, GLuint, GLuint, GLfloat[], GLubyte[], float[], GLubyte[]);
//
//void setupVAO(GLuint, GLuint, GLuint, GLuint, GLuint, GLfloat[], GLubyte[], float[], GLubyte[]);
//void drawVAO(GLubyte, int);
#pragma endregion

int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	// Shut down COM
	shutdownCOM();

	return 0;
}


void init(int argc, char* argv[]) {

	srand(time(NULL));
	for (int i = 0; i < NUM_OF_CLOUDS; i++) {
		cloudScales[i] = (double)rand() / (RAND_MAX);
		cloudStartingX[i] = (double)rand() / (RAND_MAX) * 2.0 - 1.0;
		cloudStartingY[i] = (double)rand() / (RAND_MAX) / 2.0 + 0.4;
	}

	// Initialise COM so we can use Windows Imaging Component
	initCOM();

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1366, 768);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("OpenGL - Filipe Serrazina (17123437) Coursework 1");

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

	glLineWidth(9.0f);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load demo texture
	myGrassTexture = fiLoadTexture("grass.jpg");
	mySkyTexture = fiLoadTexture("Sky.png");
	myRoadTexture = fiLoadTexture("road.jpg");
	myRoadLineTexture = fiLoadTexture("road line.png");

	// Shader setup 
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	myShaderProgramNoTexture = setupShaders(string("Shaders\\notexture_vertex_shader.txt"), string("Shaders\\notexture_fragment_shader.txt"));

	// Get uniform location of "T" variable in shader program (we'll use this in the play function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");
	locT2 = glGetUniformLocation(myShaderProgramNoTexture, "T2");

	setupSkyVBO();
	setupGrassVBO();
	setupRoadVBO();
	setupRoadLineVBO();
	setupCarVBO();
	setupCarWheelVBO();
	setupCloudVBO();
}


void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

#pragma region VBO Setups
void setupGrassVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &grassPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &grassColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassColors), grassColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &grassTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassTextureCoords), grassTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &grassIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassVertexIndices), grassVertexIndices, GL_STATIC_DRAW);
}

void setupSkyVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &skyPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &skyColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyColors), skyColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &skyTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyTextureCoords), skyTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &skyIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyVertexIndices), skyVertexIndices, GL_STATIC_DRAW);
}

void setupRoadVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &roadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadVertices), roadVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &roadColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadColors), roadColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &roadTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadTextureCoords), roadTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &roadIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roadVertexIndices), roadVertexIndices, GL_STATIC_DRAW);
}

void setupRoadLineVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &roadLinePosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadLinePosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadLineVertices), roadLineVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &roadLineColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadLineColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadLineColors), roadLineColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &roadLineTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, roadLineTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadLineTextureCoords), roadLineTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &roadLineIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadLineIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roadLineVertexIndices), roadLineVertexIndices, GL_STATIC_DRAW);
}

void setupCarVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &carPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, carPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices), carVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &carColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, carColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(carColors), carColors, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &carIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(carVertexIndices), carVertexIndices, GL_STATIC_DRAW);
}

void setupCarWheelVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &carWheelPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, carWheelPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(carWheelVertices), carWheelVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &carWheelColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, carWheelColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(carWheelColors), carWheelColors, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &carWheelIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carWheelIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(carWheelVertexIndices), carWheelVertexIndices, GL_STATIC_DRAW);
}

void setupCloudVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &cloudPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cloudPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &cloudColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cloudColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudColors), cloudColors, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &cloudIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cloudVertexIndices), cloudVertexIndices, GL_STATIC_DRAW);
}
#pragma endregion

// Example rendering functions

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);

	renderScene();
	
	//sets up current matrix as the identity matrix (when a matrix is multiplied by the identity matrix, it remains unchanged)
	//GUMatrix4 currentMatrix = GUMatrix4::identity();

	//loads currentMatrix as the current transformation matrix being used by the fixed-function pipeline
	//glLoadMatrixf((GLfloat*)&currentMatrix);

	//calls the "renderCactus" function with the current matrix as a parameter
	//renderCactus(currentMatrix);

	glDisable(GL_BLEND);
	glutSwapBuffers();
}

#pragma region Render Helpers
void handlePositions() {
	if (carXPos <= -1.5f) carXPos = 1.2f;
	else if (carXPos >= 1.3f) carXPos = -1.4f;

	if (carYPos > 0.2f) carYPos = 0.2f;
	else if (carYPos < -0.19f) carYPos = -0.19f;

	for (int i = 0; i < NUM_OF_CLOUDS; i++)
		if (cloudStartingX[i] >= 1.2f) cloudStartingX[i] = -1.3f;
}

void renderCar() {
	drawVBO_Car();
	drawVBO_CarWheel(-0.1f);
	drawVBO_CarWheel(0.15f);
	drawVBO_CarWheel(0.25f);
}

void renderRoad() {
	drawTexturedQuadVBO_Road();		// Draw the road

	// Set up road lines variables
	int numOfRoadLines = 10;
	float currentPos = 1.0f;

	// Draw the road lines
	for (int i = 0; i < numOfRoadLines; i++) {
		drawTexturedQuadVBO_RoadLine(currentPos);
		currentPos -= 0.25f;
	}
}

void renderScene() {
	handlePositions();

	drawTexturedQuadVBO_Sky();		// Draw the sky
	drawTexturedQuadVBO_Grass();	// Draw the grass
	renderRoad();					// Draw the road
	renderCar();					// Draw the car
	for (int i = 0; i < NUM_OF_CLOUDS; i++)
		drawVBO_Cloud(i);
}
#pragma endregion

#pragma region Draw VBOs
void drawTexturedQuadVBO_Grass(void) {
	
	glUseProgram(myShaderProgram);

	// Move our grass shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, -0.5f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)
	
	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myGrassTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);
	
	glBindBuffer(GL_ARRAY_BUFFER, grassPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, grassColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, grassTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);

	// use to force disable our shaderprogram
 	// glUseProgram(0);

}

void drawTexturedQuadVBO_Sky(void) {
	
	glUseProgram(myShaderProgram);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, 0.5f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mySkyTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, skyPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, skyColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, skyTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyIndicesVBO);


	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
}

void drawTexturedQuadVBO_Road(void) {

	glUseProgram(myShaderProgram);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, -0.3f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myRoadTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, roadPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, roadColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, roadTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadIndicesVBO);


	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
}

void drawTexturedQuadVBO_RoadLine(float posIndex) {

	glUseProgram(myShaderProgram);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(posIndex, -0.3f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myRoadLineTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, roadLinePosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, roadLineColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, roadLineTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadLineIndicesVBO);


	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
}

void drawVBO_Car() {

	glUseProgram(myShaderProgramNoTexture);
	
	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(carXPos, -0.3f + carYPos, 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	glBindBuffer(GL_ARRAY_BUFFER, carPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, carColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_POLYGON, 7, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

void drawVBO_CarWheel(float x) {

	glUseProgram(myShaderProgramNoTexture);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(x + carXPos, -0.35f + carYPos, 0.0f) * GUMatrix4::scaleMatrix(0.35f, 0.5f, 0.0f) * GUMatrix4::rotationMatrix(0.0f, 0.0f, carWheelRot);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	glBindBuffer(GL_ARRAY_BUFFER, carWheelPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, carWheelColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, carWheelIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE, (GLvoid*)0);
}

void drawVBO_Cloud(int cloudIndex) {

	glUseProgram(myShaderProgramNoTexture);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(cloudStartingX[cloudIndex], cloudStartingY[cloudIndex] + sin(cloudStartingX[cloudIndex] * 30) / 20, 0.0f) *
					GUMatrix4::scaleMatrix(cloudScales[cloudIndex], cloudScales[cloudIndex], 0.0f);
	glUniformMatrix4fv(locT2, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	glBindBuffer(GL_ARRAY_BUFFER, cloudPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cloudColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_POLYGON, 30, GL_UNSIGNED_BYTE, (GLvoid*)0);
}
#pragma endregion

void renderCactus(GUMatrix4& currentMatrix)
{
	//glScalef() scales down the current matrix by a factor of 100 - this compensates for the fact that we are using a -100 to +100 coordinate scheme instead of a -1 to +1 scheme
	//this is necessary because the fixed function pipeline falsely believes we are using a -1 to +1 coordinate system
	//this is because gluOrtho2D simply applies a projection matrix to "zoom out" the camera - it does not make other parts of the pipeline aware of this

	//Note: the order of translation and rotation depends on the value of our current transformation matrix
	//If we have just drawn a horizontal cactus segment, the rotation applied by the current matrix will cause our coordinates contained in the translation matrix to
	//be interpreted incorrectly e.g. a positive y-coordinate will be interpreted as a negative x-coordinate if we have previously rotated a quarter-turn anticlockwise
	//To overcome this, we need to correct the rotation BEFORE applying our translation matrix

	matrixStack.push(currentMatrix);	//stores identity matrix on stack

	currentMatrix = currentMatrix * GUMatrix4::translationMatrix(-0.6, -0.3, 0) * GUMatrix4::rotationMatrix(0, 0, cactusTheta[0]);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderBigCactusSegment();   //draws first segment
	matrixStack.push(currentMatrix);	//stores first transformation matrix on stack

	currentMatrix = currentMatrix * GUMatrix4::translationMatrix(0, segmentLength / 100, 0) * GUMatrix4::rotationMatrix(0, 0, gu_pi / 2);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderSmallCactusSegment();	   //draws second segment
	matrixStack.push(currentMatrix);	//stores second transformation matrix on stack

	currentMatrix = currentMatrix * GUMatrix4::rotationMatrix(0, 0, -gu_pi / 2) * GUMatrix4::translationMatrix(-segmentLength / 100 / 1.5, 0, 0) * GUMatrix4::rotationMatrix(0, 0, -cactusTheta[1]);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderBigCactusSegment();
	matrixStack.push(currentMatrix); //stores third transformation matrix on stack

	//pops third and second matrices from the stack and makes transformation matrix 1 the current matrix
	matrixStack.pop();
	matrixStack.pop();
	currentMatrix = matrixStack.top();

	currentMatrix = currentMatrix * GUMatrix4::translationMatrix(0, segmentLength / 100, 0);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderBigCactusSegment();
	matrixStack.push(currentMatrix);

	currentMatrix = currentMatrix * GUMatrix4::translationMatrix(0, segmentLength / 100, 0);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderBigCactusSegment();
	matrixStack.push(currentMatrix);

	currentMatrix = currentMatrix *  GUMatrix4::rotationMatrix(0, 0, -gu_pi / 2);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderSmallCactusSegment();
	matrixStack.push(currentMatrix);

	currentMatrix = currentMatrix * GUMatrix4::rotationMatrix(0, 0, gu_pi / 2) * GUMatrix4::translationMatrix(segmentLength / 100 / 1.5, 0, 0) * GUMatrix4::rotationMatrix(0, 0, -cactusTheta[2]);
	glLoadMatrixf((GLfloat*)&currentMatrix);
	glScalef(0.01, 0.01, 0.01);
	renderBigCactusSegment();
	matrixStack.push(currentMatrix);

	matrixStack.pop();
	matrixStack.pop();
	matrixStack.pop();
	matrixStack.pop();
	matrixStack.pop();
	matrixStack.pop();

	glLoadMatrixf((GLfloat*)&GUMatrix4::identity());
	glScalef(0.01, 0.01, 0.01);
}
void renderSmallCactusSegment(void)
{
	glBegin(GL_POLYGON);
		glColor3ub(59, 181, 14);
		glVertex2f(0.95f, 0);
		glVertex2f(1.0f, 0);
		glVertex2f(1.0f, segmentLength / 1.5);
		glVertex2f(0.95f, segmentLength / 1.5);
	glEnd();
}
void renderBigCactusSegment(void)
{
	glBegin(GL_POLYGON);
		glColor3ub(59, 181, 14);
		glVertex2f(0.95f, 0);
		glVertex2f(1.0f, 0);
		glVertex2f(1.0f, segmentLength);
		glVertex2f(0.95f, segmentLength);
	glEnd();
}

// square movement -------------------------------------------------------------
// update is called every frame
void update(void) {
	for (int i = 0; i < NUM_OF_CLOUDS; i++)
		cloudStartingX[i] += 0.001f;
	// Redraw the screen
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

	if (key == '+')
		segmentLength += 0.2;
	else if (key == '-')
		segmentLength -= 0.2;

	if (key == 'a' || key == 'A') {
		carXPos -= 0.02f;
		carWheelRot += 0.7f;
	}
	else if (key == 'd' || key == 'D') {
		carXPos += 0.02f;
		carWheelRot -= 0.7f;
	}

	if (key == 'w' || key == 'W')
		carYPos += 0.02f;
	else if (key == 's' || key == 'S')
		carYPos -= 0.02f;

	if (key == 'r') {

		theta = 0.0f;
		glutPostRedisplay();
	}
}

#pragma endregion
