
//
// renders our scene made up of shapes using VBO's, multiple textures, translation and two shaders
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

GLuint locT; // location of "T" uniform variable in myShaderProgram
GLuint locT2;


#pragma region Textures
GLuint myGrassTexture = 0;
GLuint mySkyTexture = 1;
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

#pragma region Grass VAO Variables
// Vertex Buffer Object IDs for the ground texture object
GLuint grassPosVBO, grassColourVBO, grassTexCoordVBO, grassIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat grassVertices[] = {

	-1.0, -0.1f,
	-1.0f, 1.0f,
	1.0f, -0.1f,
	1.0f, 1.0f
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

	-1.0, -0.1f,
	-1.0f, 1.0f,
	1.0f, -0.1f,
	1.0f, 1.0f
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

#pragma region Function Prototypes
void init(int argc, char* argv[]);

void setupGrassVBO(void);
void setupSkyVBO(void);

void report_version(void);
void display(void);

void drawTexturedQuadVBO_Grass(void);
void drawTexturedQuadVBO_Sky(void);

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

	// Initialise COM so we can use Windows Imaging Component
	initCOM();

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1366, 768);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Ground and Sky Texturing using VBO");

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
	glClearColor(0.2f, 0.2f, 0.8f, 0.0f);

	glLineWidth(9.0f);

	// Load demo texture
	myGrassTexture = fiLoadTexture("grass.jpg");
	mySkyTexture = fiLoadTexture("sky.jpg");

	// Shader setup 
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	myShaderProgramNoTexture = setupShaders(string("Shaders\\notexture_vertex_shader.txt"), string("Shaders\\notexture_fragment_shader.txt"));

	// Get uniform location of "T" variable in shader program (we'll use this in the play function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");
	locT2 = glGetUniformLocation(myShaderProgramNoTexture, "T2");

	setupGrassVBO();
	setupSkyVBO();
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
#pragma endregion

// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	drawTexturedQuadVBO_Grass();
	drawTexturedQuadVBO_Sky();
	
	glDisable(GL_BLEND);
	glutSwapBuffers();
}


#pragma region Draw VBOs
void drawTexturedQuadVBO_Grass(void) {
	
	glUseProgram(myShaderProgram);

	// Move our ground shape to the bottom half of the screen
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
#pragma endregion


// square movement -------------------------------------------------------------
// update is called every frame
void update(void) {

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

	if (key == 'r') {

		theta = 0.0f;
		glutPostRedisplay();
	}
}

#pragma endregion

/*
void setupVBO(GLuint posVBO, GLuint colourVBO, GLuint texCoordVBO, GLuint indicesVBO,
GLfloat vertices[], GLubyte colors[], float texCoords[], GLubyte vertexIndices[]) {

// setup VBO for the quad object position data
glGenBuffers(1, &posVBO);
glBindBuffer(GL_ARRAY_BUFFER, posVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// setup VBO for the quad object colour data
glGenBuffers(1, &colourVBO);
glBindBuffer(GL_ARRAY_BUFFER, colourVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

// setup VBO for the quad object texture coord data
glGenBuffers(1, &texCoordVBO);
glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

// setup quad vertex index array
glGenBuffers(1, &indicesVBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);
}

void setupVAO(GLuint vao, GLuint posVBO, GLuint colourVBO, GLuint texCoordVBO, GLuint indicesVBO,
GLfloat vertices[], GLubyte colors[], float texCoords[], GLubyte vertexIndices[]) {

// Create and bind the vertex array object.  This remembers the vertex buffer object bindings below so we don't have to specify them at render time.
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);

// Setup the star position VBO
glGenBuffers(1, &posVBO);
glBindBuffer(GL_ARRAY_BUFFER, posVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
glEnableVertexAttribArray(0);

// Setup the star colour VBO
glGenBuffers(1, &colourVBO);
glBindBuffer(GL_ARRAY_BUFFER, colourVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
glEnableVertexAttribArray(1);

// setup the star texture coordinate VBO
glGenBuffers(1, &texCoordVBO);
glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
glEnableVertexAttribArray(2);

// setup star vertex index array
glGenBuffers(1, &indicesVBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

glBindVertexArray(0);
}

void drawVAO(GLubyte vao, int numOfElements) {
glBindVertexArray(vao);
glDrawElements(GL_LINE_LOOP, numOfElements, GL_UNSIGNED_BYTE, (GLvoid*)0);
}
*/
