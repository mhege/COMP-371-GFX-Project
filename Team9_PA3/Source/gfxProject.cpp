//Authors:
//Marc Hegedus 26242219
//Nalveer Moochet 40072605
//Sean Howard 26346685
//Tzvi Filler 40042576
//Noah El Hayek 27080409
//
//Section CC, Team 9
//
// Based off COMP 371 Labs Framework
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

// - Function ''compileandLinkShaders()'' used from https://learnopengl.com

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <vector>
#include <algorithm>// for min max
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <ctime>
#include "world.h"
#include "include/irrKlang.h"

using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")



void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int myrandom(int i) { return std::rand() % i; };

void drawGridTiles(int vaoTile, int tileTextureID, glm::mat4 groupMatrixW, GLuint worldMatrixLocation ) {

	glm::mat4 transTemp;
	glm::mat4 worldMatrix;

	int vertexScale = 5; // Corresponds to position scale factor in texturedTileVertexArray

    glBindVertexArray(vaoTile);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tileTextureID);

	for (float i = -50.0; i <= 50.0; i = i + vertexScale) {

		for (float j = -50.0; j <= 50.0; j = j + vertexScale) {

			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, 0.0f, i));
			worldMatrix = (groupMatrixW)* transTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
			
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


	}

	glBindVertexArray(0);

}


void drawSphereLight(int vaoSphere, int triCount, glm::mat4 groupMatrix, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindVertexArray(vaoSphere);
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount)*36);
	glBindVertexArray(0);

}


void drawCoordinate(int vaoX, int vaoY, int vaoZ, glm::mat4 groupMatrixW, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	//x axis
	glBindVertexArray(vaoX);
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*transTemp * roTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	//y axis
	glBindVertexArray(vaoY);
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*transTemp * roTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	//z axis
	glBindVertexArray(vaoZ);
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*transTemp * roTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}


int main(int argc, char* argv[])
{
	
	// start the sound engine with default parameters
	ISoundEngine* engine = createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engin

	// play some sound stream, looped
	engine->play2D("IceGiants.MP3", true);

	// Initialize GLFW and OpenGL version
	glfwInit();

#if defined(PLATFORM_OSX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// On windows, we set OpenGL version to 2.1, to support more hardware
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	// Create Window and rendering context using GLFW, resolution is 800x600
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Team 9 Section CC gfxProject", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	Generate generate;

	GLuint boxTextureID = generate.loadTexture("../Assets/Textures/box.jpg");
	GLuint metalTextureID = generate.loadTexture("../Assets/Textures/metal.jpg");
	GLuint tileTextureID = generate.loadTexture("../Assets/Textures/marble.jpg");
	GLuint brickTextureID = generate.loadTexture("../Assets/Textures/brick.jpg");
	GLuint grassTextureID = generate.loadTexture("../Assets/Textures/grass.jpg");
	GLuint roadTextureID = generate.loadTexture("../Assets/Textures/road.jpg");
	GLuint savannahID = generate.loadTexture("../Assets/Textures/acacia.jpg");
	GLuint deadID = generate.loadTexture("../Assets/Textures/dead.jpg");
	GLuint mapleID = generate.loadTexture("../Assets/Textures/maple.jpg");
	GLuint cactusID = generate.loadTexture("../Assets/Textures/cactus.jpg");
	GLuint pineID = generate.loadTexture("../Assets/Textures/pine.jpg");
	GLuint pineLeafID = generate.loadTexture("../Assets/Textures/pineleaf.jpg");
	GLuint savannahLeafID = generate.loadTexture("../Assets/Textures/acacialeaf.jpg");
	GLuint mapleLeafID = generate.loadTexture("../Assets/Textures/mapleleaf.jpg");
	GLuint poleID = generate.loadTexture("../Assets/Textures/pole.jpg");
	GLuint sky1TextureID = generate.loadTexture("../Assets/Textures/sky1.jpg");
	GLuint sky2TextureID = generate.loadTexture("../Assets/Textures/sky2.jpg");
	GLuint sky3TextureID = generate.loadTexture("../Assets/Textures/sky3.jpg");
	GLuint sky4TextureID = generate.loadTexture("../Assets/Textures/sky4.jpg");
	GLuint road2TextureID = generate.loadTexture("../Assets/Textures/road2.jpg");
	GLuint billBoardTexID = generate.loadTexture("../Assets/Textures/board.jpg");
	GLuint build1 = generate.loadTexture("../Assets/Textures/build1.jpg");
	GLuint build2 = generate.loadTexture("../Assets/Textures/build2.jpg");
	GLuint build3 = generate.loadTexture("../Assets/Textures/build3.jpg");
	GLuint build4 = generate.loadTexture("../Assets/Textures/build4.jpg");
	GLuint build5 = generate.loadTexture("../Assets/Textures/build5.jpg");
	GLuint cement = generate.loadTexture("../Assets/Textures/cement.jpg");
	GLuint build6 = generate.loadTexture("../Assets/Textures/build6.jpg");
	GLuint billBoard2 = generate.loadTexture("../Assets/Textures/board2.jpg");
	GLuint billBoard3 = generate.loadTexture("../Assets/Textures/board3.jpg");

	// Black background
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	// Compile and link shaders here ...
	int shaderProgram = generate.compileAndLinkShaders("vertexShader.txt", "fragShader.txt");
	int shaderLightingProgram = generate.compileAndLinkShaders("lampVertexShader.txt", "lampFragShader.txt");
	int TexturedShaderProgram = generate.compileAndLinkShaders("vertexShaderTextured.txt", "fragShaderTextured.txt");
	int shaderShadow = generate.compileAndLinkShaders("shadow_vertex1.txt", "shadow_fragment.txt");
	int shaderShadow2 = generate.compileAndLinkShaders("shadow_vertex2.txt", "shadow_fragment.txt");
	int shaderShadow3 = generate.compileAndLinkShaders("shadow_vertex3.txt", "shadow_fragment.txt");
	int shaderShadow4 = generate.compileAndLinkShaders("shadow_vertex4.txt", "shadow_fragment.txt");
	int shaderShadow5 = generate.compileAndLinkShaders("shadow_vertex5.txt", "shadow_fragment.txt");
	int shaderShadow6 = generate.compileAndLinkShaders("shadow_vertex6.txt", "shadow_fragment.txt");
	int shaderProgramNoShadow = generate.compileAndLinkShaders("vertexShaderNoShadow.txt", "fragShaderNoShadow.txt");
	int TexturedShaderProgramShadow = generate.compileAndLinkShaders("texturedShadowVertex.txt", "texturedShadowFragment.txt");
	int skyShaderProgram = generate.compileAndLinkShaders("vertexSkyBox.txt", "fragmentskyBox.txt");

	//toggle texture
	bool textureOn = true;
	
	//light attributes
	glm::vec3 lightPosition1(0.0f, 30.0f, 0.0f);
	glm::vec3 lightPosition2(0.0f, 25.0f, 0.0f);
	glm::vec3 lightPosition3(0.0f, 25.0f, -90.0f);
	glm::vec3 lightPosition4(0.0f, 25.0f, 90.0f);
	glm::vec3 lightPosition5(-90.0f, 25.0f, 0.0f);
	glm::vec3 lightPosition6(90.0f, 25.0f, 0.0f);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Define and upload geometry to the GPU here ...
	float cubeHalfLength = 0.5f;
	float lineLength = 100.0f * cubeHalfLength;

	//Sphere generation
	ColoredVertex* sphereVector = generate.genSphere();
	int triCount = 50;

	int vaoSphere = generate.createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[0];
	int vaoLamp = generate.createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[1];
	int vaoSphereShadow = generate.createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[2];

	//y coordinate axis
	ColoredVertex vertexArrayYLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f)),

		ColoredVertex(glm::vec3(0.0f, 10.0f*cubeHalfLength, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f)),
	};

	int vaoY = generate.createVertexArrayObject(vertexArrayYLine, 2*3)[0];
	int vaoYShadow = generate.createVertexArrayObject(vertexArrayYLine, 2 * 3)[2];

	//x coordinate axis
	ColoredVertex vertexArrayXLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),

		ColoredVertex(glm::vec3(10.0f*cubeHalfLength, 0.0f, 0.0f),
		glm::vec3(1.0f,  0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),
	};

	int vaoX = generate.createVertexArrayObject(vertexArrayXLine, 2*3)[0];
	int vaoXShadow = generate.createVertexArrayObject(vertexArrayXLine, 2 * 3)[2];

	//zcoordinate axis
	ColoredVertex vertexArrayZLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),

		ColoredVertex(glm::vec3(0.0f, 0.0f, 10.0f*cubeHalfLength),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),
	};

	int vaoZ = generate.createVertexArrayObject(vertexArrayZLine, 2*3)[0];
	int vaoZShadow = generate.createVertexArrayObject(vertexArrayZLine, 2 * 3)[2];

	ColoredVertex vertexArrayCubeTriangle[] = {
		//tri 1 front
		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)),

		//tri 2 front
		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)),

		//tri 3 back
		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		//tri 4 back
		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		//tri 5 top
		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		//tri 6 top
		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)),

		//tri 7 bottom
		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		//tri 8 bottom
		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)),

		//tri 9 left
		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		//tri 10 left
		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)),

		//tri 11 right
		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),

		//tri 12 right
		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),

		ColoredVertex(glm::vec3(cubeHalfLength, -cubeHalfLength, -cubeHalfLength),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)),
	};

	int vaoCubeTri = generate.createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[0]; //Each triangle has 6 vectors
	int vaoCubeShadow = generate.createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[2];

	TexturedColoredVertex texturedCubeVertexArray[] = {  
		// position, color
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, -cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)), //left - red
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, -cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),

		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)), // far - blue
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f),glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // bottom - turquoise
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)), // near - green
		TexturedColoredVertex(glm::vec3(-cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // right - purple
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(cubeHalfLength,-cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)), // top - yellow
		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),

		TexturedColoredVertex(glm::vec3(cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength,-cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(-cubeHalfLength, cubeHalfLength, cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f))
	};

	int texturedVaoCubeTri = generate.createTextureVertexArrayObject(texturedCubeVertexArray, 4 * 6 * 6)[0];
	int notTexturedVaoCubeTri = generate.createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[0];

	TexturedColoredVertex texturedTileVertexArray[] = {

		TexturedColoredVertex(glm::vec3(10 * cubeHalfLength, 0.0f, 10 * cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(10 * cubeHalfLength, 0.0f,10 * -cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(10 * -cubeHalfLength, 0.0f,10 * -cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

		TexturedColoredVertex(glm::vec3(10 * cubeHalfLength, 0.0f,10 * cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(10 * -cubeHalfLength, 0.0f,10 * -cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(10 * -cubeHalfLength, 0.0f,10 * cubeHalfLength), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f))

	};

	int vaoTile = generate.createTextureVertexArrayObject(texturedTileVertexArray, 4 * 6)[0];
	int vaoTileShadow = generate.createTextureVertexArrayObject(texturedTileVertexArray, 4 * 6)[1];

	/*
	Initialize scale and translation variables
	To be used with keyboard input presses
	*/

	int currentModel = 1;
	glm::mat4 groupMatrixW;

	//Initiate Lighting model matrix
	glm::mat4 transGroupML = glm::translate(glm::mat4(1.0f), lightPosition1);
	glm::mat4 scaleGroupML = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML = transGroupML * scaleGroupML;

	//Initiate Lighting model matrix2
	glm::mat4 transGroupML2 = glm::translate(glm::mat4(1.0f), lightPosition2);
	glm::mat4 scaleGroupML2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML2 = transGroupML2 * scaleGroupML2;

	//Initiate Lighting model matrix2
	glm::mat4 transGroupML4 = glm::translate(glm::mat4(1.0f), lightPosition4);
	glm::mat4 scaleGroupML4 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML4 = transGroupML4 * scaleGroupML4;

	//Initiate Lighting model matrix2
	glm::mat4 transGroupML3 = glm::translate(glm::mat4(1.0f), lightPosition3);
	glm::mat4 scaleGroupML3 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML3 = transGroupML3 * scaleGroupML3;

	//Initiate Lighting model matrix2
	glm::mat4 transGroupML5 = glm::translate(glm::mat4(1.0f), lightPosition5);
	glm::mat4 scaleGroupML5 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML5 = transGroupML5 * scaleGroupML5;

	//Initiate Lighting model matrix2
	glm::mat4 transGroupML6 = glm::translate(glm::mat4(1.0f), lightPosition6);
	glm::mat4 scaleGroupML6 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixML6 = transGroupML6 * scaleGroupML6;


	//Initiate Sphere group matrix
	glm::mat4 transGroupMS = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 scaleGroupMS = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 groupMatrixMS = transGroupMS * scaleGroupMS;

	//Initialize zoom matrix
	glm::mat4 zoom = glm::mat4(1.0f);

	// For frame time
	float lastFrameTime = glfwGetTime();
	double lastMousePosX, lastMousePosY;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
	bool initialMouse = 0;

	
	//setting level for texture and shadow
	GLint ImageLoc = glGetUniformLocation(TexturedShaderProgramShadow, "textureSampler");
	GLint shadowLoc = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map1");
	GLint shadowLoc2 = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map2");
	GLint shadowLoc3 = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map3");
	GLint shadowLoc4 = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map4");
	GLint shadowLoc5 = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map5");
	GLint shadowLoc6 = glGetUniformLocation(TexturedShaderProgramShadow, "shadow_map6");

	glUseProgram(TexturedShaderProgramShadow);
	glUniform1i(ImageLoc, 1);
	glUniform1i(shadowLoc, 0);
	glUniform1i(shadowLoc2, 2);
	glUniform1i(shadowLoc3, 3);
	glUniform1i(shadowLoc4, 4);
	glUniform1i(shadowLoc5, 5);
	glUniform1i(shadowLoc6, 6);

	glUseProgram(TexturedShaderProgram);
	ImageLoc = glGetUniformLocation(TexturedShaderProgram, "textureSampler");
	glUniform1i(ImageLoc, 1);
	

	glUseProgram(skyShaderProgram);
	ImageLoc = glGetUniformLocation(skyShaderProgram, "textureSampler");
	glUniform1i(ImageLoc, 1);

	////////////////////////SHADOW 1//////////////////////////////////////////////////////////////
	GLuint depth_map_fbo;  // fbo: framebuffer object
						   // Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo);

	const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture;
	// Get the texture
	glGenTextures(1, &depth_map_texture);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo); //activating this line makes the screen black

													  // Attach the depth map texture to the depth map framebuffer
													  //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////SHADOW 2////////////////////////////////////////////////////////////////////////////
	GLuint depth_map_fbo2;  // fbo: framebuffer object
							// Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo2);
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture2;
	// Get the texture
	glGenTextures(1, &depth_map_texture2);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture2);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo2); //activating this line makes the screen black

													   // Attach the depth map texture to the depth map framebuffer
													   //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture2, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////SHADOW 3////////////////////////////////////////////////////////////////////////////

	GLuint depth_map_fbo3;  // fbo: framebuffer object
							// Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo3);
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture3;
	// Get the texture
	glGenTextures(1, &depth_map_texture3);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture3);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo3); //activating this line makes the screen black

													   // Attach the depth map texture to the depth map framebuffer
													   //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture3, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////SHADOW 4////////////////////////////////////////////////////////////////////////////

	GLuint depth_map_fbo4;  // fbo: framebuffer object
							// Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo4);
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture4;
	// Get the texture
	glGenTextures(1, &depth_map_texture4);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture4);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo4); //activating this line makes the screen black

													   // Attach the depth map texture to the depth map framebuffer
													   //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture4, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////SHADOW 5////////////////////////////////////////////////////////////////////////////

	GLuint depth_map_fbo5;  // fbo: framebuffer object
							// Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo5);
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture5;
	// Get the texture
	glGenTextures(1, &depth_map_texture5);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture5);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo5); //activating this line makes the screen black

													   // Attach the depth map texture to the depth map framebuffer
													   //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture5, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////SHADOW 6////////////////////////////////////////////////////////////////////////////

	GLuint depth_map_fbo6;  // fbo: framebuffer object
							// Get the framebuffer

	glGenFramebuffers(1, &depth_map_fbo6);
	// Variable storing index to texture used for shadow mapping
	GLuint depth_map_texture6;
	// Get the texture
	glGenTextures(1, &depth_map_texture6);
	// Bind the texture so the next glTex calls affect it
	glBindTexture(GL_TEXTURE_2D, depth_map_texture6);

	// Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Set texture sampler parameters.
	// The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Variable storing index to framebuffer used for shadow mapping



	// Bind the framebuffer so the next glFramebuffer calls affect it
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo6); //activating this line makes the screen black

													   // Attach the depth map texture to the depth map framebuffer
													   //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture6, 0);
	glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////


	//Camera settings
	float cameraHorizontalAngle = -90.0f;
	float cameraVerticalAngle = 0.0f;
	glm::vec3 cameraPosition(0.0f, 6.0f, 25.0f);
	glm::vec3 cameraLookAt(0.0f, -12.0f, -50.0f);
	glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

	float saveTheta = glm::radians(-90.0f);
	float savePhi = glm::radians(-13.496f); // Pre-calculated to keep facing origin. Using pythagoras.

	std::srand(unsigned(std::time(0)));

	std::vector<int> vect;
    for (int i = 1; i <= 5; i += 1) {
			vect.push_back(i);
	}

	std::vector<int> vect2;
	for (int i = 1; i <= 3; i += 1) {
		    vect2.push_back(i);
	}

	std::vector<int> vect3;
	for (int i = 0; i <= 2; i += 1) {
		vect3.push_back(i);
	}

	//shuffle vectors to place buildings randomly
	for (int k = 0; k <= myrandom(10)+1; k += 1) {
		std::random_shuffle(vect.begin(), vect.end(),myrandom);
		std::random_shuffle(vect2.begin(), vect2.end(),myrandom);
		std::random_shuffle(vect3.begin(), vect3.end(),myrandom);
	}

	glEnable(GL_CULL_FACE);

	//tree available positions

	glm::vec3 posi1 = glm::vec3(-30.0f, 0.0f, -60.0f);
	glm::vec3 posi2 = glm::vec3(-30.0f, 0.0f, -120.0f);
	glm::vec3 posi3 = glm::vec3(30.0f, 0.0f, -120.0f);
	glm::vec3 posi4 = glm::vec3(80.0f, 0.0f, -120.0f);
	glm::vec3 posi5 = glm::vec3(40.0f, 0.0f, 120.0f);
	glm::vec3 posi6 = glm::vec3(-40.0f, 0.0f, 120.0f);
	glm::vec3 posi7 = glm::vec3(-90.0f, 0.0f, 120.0f);
	glm::vec3 posi8 = glm::vec3(45.0f, 0.0f, 45.0f);
	glm::vec3 posi9 = glm::vec3(-85.0f, 0.0f, 45.0f);

	std::vector<glm::vec3> randPositions;
	std::vector<int> randTrees;

	std::vector<int> treeRand;
	for (int i = 1; i <= 9; i++) {
		treeRand.push_back(i);
	}

	for (int k = 0; k <= myrandom(10) + 1; k += 1) {
		std::random_shuffle(treeRand.begin(), treeRand.end(), myrandom);
	}

	for (int i = 0; i < 9; i++) {
		randTrees.push_back(1 + (std::rand() % (4 - 1 + 1)));
	}

	for (int i = 0; i < 9; i++) {

		switch (treeRand.at(i)) {
		case 1:
			randPositions.push_back(posi1);
			break;
		case 2:
			randPositions.push_back(posi2);
			break;
		case 3:
			randPositions.push_back(posi3);
			break;
		case 4:
			randPositions.push_back(posi4);
			break;
		case 5:
			randPositions.push_back(posi5);
			break;
		case 6:
			randPositions.push_back(posi6);
			break;
		case 7:
			randPositions.push_back(posi7);
			break;
		case 8:
			randPositions.push_back(posi8);
			break;
		case 9:
			randPositions.push_back(posi9);
			break;
		}


	}


	// create world
    World world(shaderProgram, TexturedShaderProgram,TexturedShaderProgramShadow,skyShaderProgram, shaderShadow, shaderShadow2, shaderShadow3, shaderShadow4, shaderShadow5, shaderShadow6);

	//create models
	LDModel r4(0.0,0.0,0.0,boxTextureID,metalTextureID,1);
	LDModel L0(0.0, 0.0, -30.0, boxTextureID, metalTextureID, 1);
	LDModel A8(30.0, 0.0, 0.0, boxTextureID, metalTextureID, 1);
	LDModel A6(-30.0, 0.0, 0.0, boxTextureID, metalTextureID, 1);
	LDModel V4(0.0, 0.0, 30.0, boxTextureID, metalTextureID, 1);

	

	//Camera is currently setup to look at the center of the grid.
	int currentCamera = 1;//model-1(press 1), center view-2(press C)
	int playerCamera = 1;//which model(press model number)

	glm::vec3 cameraOnModel = r4.getCameraPos();
	glm::vec3 cameraLookAt2 = glm::vec3(0.0f, 0.0f, -10.0f);


	glm::mat4 viewMatrixOri = *world.getViewMatrix();

	glm::mat4 viewMatrixOnModel = r4.getViewMatrix();


	glm::mat4 viewMatrix;

    int lastMouseLeftState = GLFW_RELEASE;
	
	while (!glfwWindowShouldClose(window))
	{

		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;
		
		//white spheres

		glUseProgram(shaderLightingProgram);

		GLint lightColorLocLight = glGetUniformLocation(shaderLightingProgram, "lightColor");
		glUniform3f(lightColorLocLight, 1.0f, 1.0f, 1.0f);

		GLuint projectionMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocationLight, 1, GL_FALSE, &(*world.getProjectionMatrix())[0][0]);

		GLuint viewMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocationLight, 1, GL_FALSE, &viewMatrix[0][0]);

		GLuint worldMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "worldMatrix");

		// Each frame, reset color of each pixel to glClearColor and clear depth
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		world.setUpWorld();


		
		if (currentModel == 1)
			viewMatrixOnModel = r4.getViewMatrix();
		else if (currentModel == 2)
			viewMatrixOnModel = L0.getViewMatrix();
		else if (currentModel == 3)
			viewMatrixOnModel = A8.getViewMatrix();
		else if (currentModel == 4)
			viewMatrixOnModel = A6.getViewMatrix();
		else
			viewMatrixOnModel = V4.getViewMatrix();

		viewMatrixOri = *world.getViewMatrix();


		if (currentCamera == 1) {
			viewMatrix = viewMatrixOnModel;
		}
		else {
			viewMatrix = viewMatrixOri;
		}

		viewMatrix = zoom * viewMatrix;

		world.setView(viewMatrix);
		groupMatrixW = *world.getgroupMatrixW();

		

		/////////////////////////////////////START FIRST RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry
		GLuint worldMatrixLocation;

		glUseProgram(shaderShadow);
		worldMatrixLocation = glGetUniformLocation(shaderShadow, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeShadow, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeShadow, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeShadow, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeShadow, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);

		world.placeBillBoards(vaoCubeShadow, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);
		world.placeBuildings(vaoCubeShadow, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);

		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);

		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END FIRST RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START SECOND RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry

		glUseProgram(shaderShadow2);
		worldMatrixLocation = glGetUniformLocation(shaderShadow2, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo2);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE2);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);

		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);


		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END SECOND RENDER///////////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////START THIRD RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry

		glUseProgram(shaderShadow3);
		worldMatrixLocation = glGetUniformLocation(shaderShadow3, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo3);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE3);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);
		
		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);

		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END THIRD RENDER///////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START FOURTH RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry

		glUseProgram(shaderShadow4);
		worldMatrixLocation = glGetUniformLocation(shaderShadow4, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo4);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE4);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);
		
		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);

		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END SECOND RENDER///////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START FIFTH RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry

		glUseProgram(shaderShadow5);
		worldMatrixLocation = glGetUniformLocation(shaderShadow5, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo5);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE5);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);
		
		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);


		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END FIFTH RENDER///////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START SIXTH RENDER//////////////////////////////////////////////////////////////////////////////////

		// Draw geometry

		glUseProgram(shaderShadow6);
		worldMatrixLocation = glGetUniformLocation(shaderShadow6, "worldMatrix");

		// Use proper image output size
		glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
		// Bind depth map texture as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo6);

		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE6);

		r4.draw(1, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeShadow, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeShadow, worldMatrixLocation, groupMatrixW);

		
		
		world.drawPole(vaoCubeShadow, poleID, worldMatrixLocation, groupMatrixW);
		
		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);


		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END SIXTH RENDER///////////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////START SEVENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		int winWidth, winHeight;
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);

		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Flip shader based on toggle

		

		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture);

		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);
        world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);

		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);
		//Draw untextured elements


		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture);


		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END SEVENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START EIGHTH RENDER//////////////////////////////////////////////////////////////////////////////////

		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);

		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//Flip shader based on toggle

		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture2);
 

		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);

		world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);
		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);

		//Draw untextured elements

		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture2);


		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END EIGTH RENDER//////////////////////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////START NINTH RENDER//////////////////////////////////////////////////////////////////////////////////

		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);

		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//Flip shader based on toggle

		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");


		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture3);
        

		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);

		world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);
		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);
		
		//Draw untextured elements

		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture3);

		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END NINTH RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START TENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);

		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//Flip shader based on toggle

		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");


		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture4);


		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);


		world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);
		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);
		
		//Draw untextured elements

		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture4);


		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END TENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START ELEVENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);



		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		//Flip shader based on toggle

		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");


		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture5);


		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);


		world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2, billBoard3, vect3, worldMatrixLocation);
		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID, vect, vect2, worldMatrixLocation);

		//Draw untextured elements

		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture5);


		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END ELEVENTH RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START TWELFTH RENDER//////////////////////////////////////////////////////////////////////////////////

		glfwGetWindowSize(window, &winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);

		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		world.drawSky(vaoCubeTri, sky3TextureID, sky4TextureID, sky1TextureID, sky2TextureID);


		vaoCubeTri = texturedVaoCubeTri;
		glUseProgram(TexturedShaderProgramShadow);
		worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");


		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture6);


		r4.draw(1, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		L0.draw(2, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A8.draw(3, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		A6.draw(4, vaoCubeTri, worldMatrixLocation, groupMatrixW);
		V4.draw(5, vaoCubeTri, worldMatrixLocation, groupMatrixW);

		for (int i = 0; i < 9; i++) {

			switch (randTrees.at(i)) {
			case 1:
				world.drawPineTree(vaoCubeTri, pineID, pineLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 2:
				world.drawCactusTree(vaoCubeTri, cactusID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 3:
				world.drawSavannahTree(vaoCubeTri, savannahID, savannahLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			case 4:
				world.drawMapleTree(vaoCubeTri, mapleID, mapleLeafID, worldMatrixLocation, groupMatrixW, randPositions.at(i));
				break;
			}

		}
		world.drawPole(vaoCubeTri, poleID, worldMatrixLocation, groupMatrixW);


		world.placeBillBoards(vaoCubeTri, billBoardTexID, metalTextureID, billBoard2,billBoard3,vect3, worldMatrixLocation);

		world.placeBuildings(vaoCubeTri, build1, build2, build3, build4, build5, cement, metalTextureID,vect,vect2, worldMatrixLocation);

		//Draw untextured elements

		glUseProgram(shaderProgram);
		worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");


		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, depth_map_texture6);


		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		glUseProgram(shaderLightingProgram);
		drawSphereLight(vaoLamp, triCount, groupMatrixML2, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML3, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML4, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML5, worldMatrixLocationLight);
		drawSphereLight(vaoLamp, triCount, groupMatrixML6, worldMatrixLocationLight);

		/////////////////////////////////////END TWELFTH RENDER//////////////////////////////////////////////////////////////////////////////////
		
		world.drawGround(vaoTile, vaoCubeTri, roadTextureID, grassTextureID, brickTextureID, tileTextureID);

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();

		//Camera calculations

		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			//zoom
			zoom = glm::translate(zoom, glm::vec3(0.0f, 0.0f, dy));

		}

		// by default, camera is centered at the origin and look towards negative z-axis
		//set variable model to set which model we are working with(1-5)
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			currentModel = 1;
			currentCamera = 1;
		}

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			currentModel = 2;
			currentCamera = 1;
		}

		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			currentModel = 3;
			currentCamera = 1;
		}

		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			currentModel = 4;
			currentCamera = 1;
		}

		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		{
			currentModel = 5;
			currentCamera = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			currentCamera = 2;
		}


		//Translate left
		if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS))
		{
			if (currentModel == 1)
				r4.moveLeft(2);

			if (currentModel == 2)
				L0.moveLeft(2);

			if (currentModel == 3)
				A8.moveLeft(2);

			if (currentModel == 4)
				A6.moveLeft(2);

			if (currentModel == 5)
				V4.moveLeft(2);
		}

		//Translate right
		if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS))
		{
			if (currentModel == 1) {
				r4.moveRight(2);
			}
			if (currentModel == 2)
				L0.moveRight(2);

			if (currentModel == 3)
				A8.moveRight(2);

			if (currentModel == 4)
				A6.moveRight(2);

			if (currentModel == 5)
				V4.moveRight(2);
		}

		//Translate up
		if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS))
		{
			if (currentModel == 1) {
				r4.moveForward(2);
				
			}
			if (currentModel == 2)
				L0.moveForward(2);

			if (currentModel == 3)
				A8.moveForward(2);

			if (currentModel == 4)
				A6.moveForward(2);

			if (currentModel == 5)
				V4.moveForward(2);
		}

		//Translate down
		if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS))
		{
			if (currentModel == 1)
				r4.moveBack(2);

			if (currentModel == 2)
				L0.moveBack(2);

			if (currentModel == 3)
				A8.moveBack(2);

			if (currentModel == 4)
				A6.moveBack(2);

			if (currentModel == 5)
				V4.moveBack(2);
		}


		//World orientation
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			world.setRotAXW(world.getRotAXW() + 1.0f);
			

		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			world.setRotAXW(world.getRotAXW() - 1.0f);

		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			world.setRotAYW(world.getRotAYW() + 1.0f);

		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			world.setRotAYW(world.getRotAYW() - 1.0f);

		}
		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {

			world.setRotAYW(0.0f);
			world.setRotAXW(0.0f);
			
		}

		// change camera position to corners
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)  //center
		{
			cameraPosition = glm::vec3(0.0f, 6.0f, 25.0f);

		}
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)  //L0 at top right corner
		{
			cameraPosition = glm::vec3(60.0f * cubeHalfLength, 6.0f, -10.0f);

		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)  //A8 at top left corner
		{
			cameraPosition = glm::vec3(-60.0f * cubeHalfLength, 6.0f, -10.0f);

		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)//V4 back right corner
		{
			cameraPosition = glm::vec3(60.0f * cubeHalfLength, 6.0f, 55.0f);

		}
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)//A6 back left corner
		{
			cameraPosition = glm::vec3(-60.0f * cubeHalfLength, 6.0f, 55.0f);

		}


		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)  
		{
			//sleep to toggle only once(due to frame speed)
			_sleep(100);

			//toggle
			if (playerCamera == 1) {
				playerCamera = 2;
				r4.setCurrentCamera(2);
				L0.setCurrentCamera(2);
				A8.setCurrentCamera(2);
				A6.setCurrentCamera(2);
				V4.setCurrentCamera(2);
			}
			else {
				playerCamera = 1;
				r4.setCurrentCamera(1);
				L0.setCurrentCamera(1);
				A8.setCurrentCamera(1);
				A6.setCurrentCamera(1);
				V4.setCurrentCamera(1);
			}

		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			//sleep to toggle only once(due to frame speed)
			_sleep(100);

			//toggle
			if (world.getDay() == true) {
			     world.setDay(false);
				 
			}
			else {
				world.setDay(true);
				
			}

		}



		if (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			
			if (currentModel == 1) {
				r4.shoot(shaderProgram);
				
			}
		
		}
		lastMouseLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	}

	// Shutdown GLFW
	glfwTerminate();

	return 0;
}

void setProjectionMatrix(int shaderProgram, glm::mat4 projectionMatrix)
{
	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, glm::mat4 viewMatrix)
{
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, glm::mat4 worldMatrix)
{
	glUseProgram(shaderProgram);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}