
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


// Textures
GLuint myGroundTexture = 0;
GLuint myMountainTexture = 1;

// Mouse input (rotation) example
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;


// Shader program object for applying textures to our shapes
GLuint myShaderProgram;

// Second shader progam object for non textured shapes such as our rainbow star
GLuint myShaderProgramNoTexture;

// Vertex Buffer Object IDs for the ground texture object
GLuint quadPosVBO, quadColourVBO, quadTexCoordVBO, quadIndicesVBO;


// Vertex Buffer Object IDs for the mountain and sky texture object
GLuint quadMPosVBO, quadMColourVBO, quadMTexCoordVBO, quadMIndicesVBO;

// Packed vertex arrays for the ground object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat quadVertices[] = {

	-1.0, -1.0f,
	-1.0f, 1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte quadColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255
	
};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float quadTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte quadVertexIndices[] = { 0, 1, 2, 3 };

// -----------------------
// Packed vertex arrays for the mountain and sky object

// 1) Position Array - Store vertices as (x,y) pairs
static GLfloat quadMVertices[] = {

	-1.0, -1.0f,
	-1.0f, 1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f
};

// 2) Colour Array - Store RGB values as unsigned bytes
static GLubyte quadMColors[] = {

	255, 0, 0, 255,
	255, 255, 0, 255,
	0, 255, 0, 255,
	0, 255, 255, 255

};

// 3) Texture coordinate array (store uv coordinates as floating point values)
static float quadMTextureCoords[] = {

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f

};

// 4) Index Array - Store indices to quad vertices - this determines the order the vertices are to be processed
static GLubyte quadMVertexIndices[] = { 0, 1, 2, 3 };


// Function Prototypes

void init(int argc, char* argv[]);
void setupStarVBO(void);
void setupQuadTextureGroundVBO(void);
void setupQuadTextureMountainVBO(void);
void report_version(void);
void display(void);
void drawTexturedQuad(void);
void drawTexturedQuadVBOGround(void);
void drawTexturedQuadVBOMountain(void);
void drawStar(void);
void drawStar_VBO(void);
void update(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);

void setupVBO(GLuint, GLuint, GLuint, GLuint, GLfloat[], GLubyte[], float[], GLubyte[]);

void setupVAO(GLuint, GLuint, GLuint, GLuint, GLuint, GLfloat[], GLubyte[], float[], GLubyte[]);
void drawVAO(GLubyte, int);

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
	myGroundTexture = fiLoadTexture("Sky.png");
	myMountainTexture = fiLoadTexture("Mountains and River.png");

	// Shader setup 
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	myShaderProgramNoTexture = setupShaders(string("Shaders\\notexture_vertex_shader.txt"), string("Shaders\\notexture_fragment_shader.txt"));

	// Get uniform location of "T" variable in shader program (we'll use this in the play function to give the uniform variable "T" a value)
	locT = glGetUniformLocation(myShaderProgram, "T");
	locT2 = glGetUniformLocation(myShaderProgramNoTexture, "T2");

	// Setup star object using Vertex Buffer Objects (VBOs)
	setupStarVBO();
	setupQuadTextureGroundVBO();
	//setupVBO(quadPosVBO, quadColourVBO, quadTexCoordVBO, quadIndicesVBO, quadVertices, quadColors, quadTextureCoords, quadVertexIndices);
	setupQuadTextureMountainVBO();
}


void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

// our ground
void setupQuadTextureGroundVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &quadPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &quadColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColors), quadColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &quadTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoords), quadTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &quadIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadVertexIndices), quadVertexIndices, GL_STATIC_DRAW);
}

// our mountain and sky
void setupQuadTextureMountainVBO(void) {

	// setup VBO for the quad object position data
	glGenBuffers(1, &quadMPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadMPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadMVertices), quadMVertices, GL_STATIC_DRAW);

	// setup VBO for the quad object colour data
	glGenBuffers(1, &quadMColourVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadMColourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadMColors), quadMColors, GL_STATIC_DRAW);

	// setup VBO for the quad object texture coord data
	glGenBuffers(1, &quadMTexCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadMTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadMTextureCoords), quadMTextureCoords, GL_STATIC_DRAW);

	// setup quad vertex index array
	glGenBuffers(1, &quadMIndicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMIndicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadMVertexIndices), quadMVertexIndices, GL_STATIC_DRAW);
}

// Example rendering functions

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	// render our scene in sequence

	// draw our ground
	drawTexturedQuadVBOGround();

	// draw our mountain and sky.
	drawTexturedQuadVBOMountain();

	// when we call our star draw function we will use our second shader for non textured shapes such as our rainbow star

	drawStar_VBO();
	
	glDisable(GL_BLEND);
	glutSwapBuffers();
}


// draw our ground function
void drawTexturedQuadVBOGround(void) {
	
	glUseProgram(myShaderProgram);

	// Move our ground shape to the bottom half of the screen
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)
	
	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myGroundTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);
	
	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);

	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);

	// use to force disable our shaderprogram
 	// glUseProgram(0);

}

// Draw our mountain amd sky

void drawTexturedQuadVBOMountain(void) {
	
	glUseProgram(myShaderProgram);

	//Move our shape into the top position
	GUMatrix4 T = GUMatrix4::translationMatrix(0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);


	// Bind each vertex buffer and enable
	// The data is still stored in the GPU but we need to set it up (which also includes validation of the VBOs behind-the-scenes)

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myMountainTexture);
	glUniform1i(glGetUniformLocation(myShaderProgram, "texture"), 0);
	glEnable(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, quadPosVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, quadColourVBO);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, quadTexCoordVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray(2);


	// Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndicesVBO);


	// Draw the object - same function call as used for vertex arrays but the last parameter is interpreted as an offset into the currently bound index buffer (set to 0 so we start drawing from the beginning of the buffer).
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glDisable(GL_TEXTURE_2D);
	

}



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
