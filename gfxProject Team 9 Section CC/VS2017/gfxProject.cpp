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
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

struct TexturedColoredVertex
{

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;

	TexturedColoredVertex() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 0.0f, 0.0f);
		uv = glm::vec2(0.0f, 0.0f);
	}

	TexturedColoredVertex(glm::vec3 _position, glm::vec3 _color, glm::vec3 _normal, glm::vec2 _uv)
		: position(_position), color(_color), normal(_normal), uv(_uv) {}

};

struct ColoredVertex
{

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;

	ColoredVertex() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	ColoredVertex(glm::vec3 _position, glm::vec3 _color, glm::vec3 _normal)
		: position(_position), color(_color), normal(_normal) {}

};

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value)
{
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value)
{
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, glm::value_ptr(uniform_value));
}

template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
	glUseProgram(shader_id);
	glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
	glUseProgram(0);
}

int compileAndLinkShaders(std::string vertexPath, std::string fragmentPath)
{

	//initialize
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//check if exceptions can be thrown
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// buffer to stream
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close handles
		vShaderFile.close();
		fShaderFile.close();
		//stream to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	//c++ string to c string
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile shaders

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	//print compile errors
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	//print compile errors
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//shader program

	int ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	//print linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//Delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return ID;
}

int* createVertexArrayObject(ColoredVertex vertexArray[], int vertexCount)
{

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertexArray, GL_STATIC_DRAW);


	//model attributes
	//position attribute
	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(ColoredVertex), // stride - each vertex contain 3 vec3 (position, color, normal)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//color attribute
	glVertexAttribPointer(1,   // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColoredVertex),
		(void*)sizeof(glm::vec3)  // color is offseted by 1 vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	//normal attribute
	glVertexAttribPointer(2,   // attribute 2 matches normal in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColoredVertex),
		(void*)(2 * sizeof(glm::vec3))  // normal is offseted by 2 vec3 (comes after color)
	);
	glEnableVertexAttribArray(2);

	//light attributes
	//position attribute
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColoredVertex),
		(void*)0
	);
	glEnableVertexAttribArray(0);

	//shadow attributes
	//position attributes
	GLuint depthVAO;
	glGenVertexArrays(1, &depthVAO);
	glBindVertexArray(depthVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(ColoredVertex),
		(void*)0
	);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int vaos[3];

	vaos[0] = vertexArrayObject;
	vaos[1] = lightVAO;
	vaos[2] = depthVAO;

	return vaos;

}

int* createTextureVertexArrayObject(TexturedColoredVertex vertexArray[], int vertexCount)
{

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertexArray, GL_STATIC_DRAW);


	//model attributes
	//position attribute
	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(TexturedColoredVertex), // stride - each vertex contain 3 vec3 (position, color, normal)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//color attribute
	glVertexAttribPointer(1,   // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)sizeof(glm::vec3)  // color is offseted by 1 vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	//normal attribute
	glVertexAttribPointer(2,   // attribute 2 matches normal in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(2 * sizeof(glm::vec3))  // normal is offseted by 2 vec3 (comes after color)
	);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3,                // attribute 2 matches aUV in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredVertex),
		(void*)(3 * sizeof(glm::vec3))      // uv is offseted by 2 vec3 (comes after position and color)
	);
	glEnableVertexAttribArray(3);

	//shadow attributes
	//position attribute
	GLuint depthVAO;
	glGenVertexArrays(1, &depthVAO);
	glBindVertexArray(depthVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(TexturedColoredVertex), // stride - each vertex contain 3 vec3 (position, color, normal)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int vaos[2];

	vaos[0] = vertexArrayObject;
	vaos[1] = depthVAO;

	return vaos;

}

GLuint loadTexture(const char* filename)
{
	// Step1 Create and bind textures
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	assert(textureId != 0);


	glBindTexture(GL_TEXTURE_2D, textureId);

	// Step2 Set filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Step3 Load Textures with dimension data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
		return 0;
	}

	// Step4 Upload the texture to the PU
	GLenum format = 0;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
		0, format, GL_UNSIGNED_BYTE, data);

	// Step5 Free resources
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

void setProjectionMatrix(int shaderProgram, glm::mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, glm::mat4 viewMatrix);
void setWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);

void drawCube(char* type, int model) {

	//draw type based on model numbers
	if ((strcmp(type, "point") == 0) && model == 1) {
		glDrawArrays(GL_POINTS, 0, 36);
	}
	else if ((strcmp(type, "line") == 0) && model == 1) {
		glDrawArrays(GL_LINES, 0, 36);
	}
	else if ((strcmp(type, "triangle") == 0) && model == 1) {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if ((strcmp(type, "point") == 0) && model == 2) {
		glDrawArrays(GL_POINTS, 0, 36);
	}
	else if ((strcmp(type, "line") == 0) && model == 2) {
		glDrawArrays(GL_LINES, 0, 36);
	}
	else if ((strcmp(type, "triangle") == 0) && model == 2) {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if ((strcmp(type, "point") == 0) && model == 3) {
		glDrawArrays(GL_POINTS, 0, 36);
	}
	else if ((strcmp(type, "line") == 0) && model == 3) {
		glDrawArrays(GL_LINES, 0, 36);
	}
	else if ((strcmp(type, "triangle") == 0) && model == 3) {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if ((strcmp(type, "point") == 0) && model == 4) {
		glDrawArrays(GL_POINTS, 0, 36);
	}
	else if ((strcmp(type, "line") == 0) && model == 4) {
		glDrawArrays(GL_LINES, 0, 36);
	}
	else if ((strcmp(type, "triangle") == 0) && model == 4) {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if ((strcmp(type, "point") == 0) && model == 5) {
		glDrawArrays(GL_POINTS, 0, 36);
	}
	else if ((strcmp(type, "line") == 0) && model == 5) {
		glDrawArrays(GL_LINES, 0, 36);
	}
	else if ((strcmp(type, "triangle") == 0) && model == 5) {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void drawGridVBO(int vaoVert, int vaoHorz, float lineLength, glm::mat4 groupMatrixW, GLuint worldMatrixLocation) { // Modern OpenGL

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	float incrX = 0, incrY = 0;

	glBindVertexArray(vaoVert);

	incrX = 0.0f;
	for (int i = 0; i < 100; i++)
	{	//Vertical lines
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(incrX, 0.0f, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*transTemp * roTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);
		incrX += lineLength / 50;
	}
	glBindVertexArray(0);

	glBindVertexArray(vaoHorz);
	incrY = 0.0f;
	for (int i = 0; i < 100; i++)
	{	//Horizontal lines
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, incrY, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		worldMatrix = (groupMatrixW)*roTemp * transTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_LINES, 0, 2);
		incrY += lineLength / 50;
	}


	glBindVertexArray(0);

}

void drawGridTiles(int vaoTile, int tileTextureID, glm::mat4 groupMatrixW, GLuint worldMatrixLocation ) {

	glm::mat4 transTemp;
	glm::mat4 worldMatrix;

	int vertexScale = 5; // Corresponds to position scale factor in texturedTileVertexArray

	glBindVertexArray(vaoTile);
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

void drawV4(int vaoCubeTri, int letterTID, int digitTID, char* type, int model, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation, glm::mat4 baseTrans) {

	glBindVertexArray(vaoCubeTri);

	float Px[] = { 0.0f,5.0f,2.5f,-2.5f,7.5f,20.0f,13.0f,16.0f };
	float Py[] = { 5.0f,5.0f,0.5f,9.5f,9.5f,5.5f,8.0f,5.5f };
	float Pz[] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	float Theta[] = { 30.0f,-30.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	float Sx[] = { 2.0f,2.0f,4.0f ,4.0f ,4.0f,2.0f,2.0f,8.0f };
	float Sy[] = { 10.0f,10.0f,2.0f ,2.0f ,2.0f,11.0f,5.5f,2.0f };
	float Sz[] = { 2.0f,2.0f,2.0f ,2.0f ,2.0f,2.0f,2.0f,2.0f };


	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindTexture(GL_TEXTURE_2D, letterTID);//Start with letter texture
	for (int i = 0; i < sizeof(Px) / sizeof(float); ++i) {
		if (i == 5) glBindTexture(GL_TEXTURE_2D, digitTID);//At i==5, switch to number texture
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(Px[i] * cubeHalfLength, Py[i] * cubeHalfLength, Pz[i] * cubeHalfLength));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(Theta[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(Sx[i] * cubeHalfLength, Sy[i] * cubeHalfLength, Sz[i] * cubeHalfLength));
		worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		drawCube(type, model);
	}

	glBindVertexArray(0);
}

void drawR4(int vaoCubeTri, int letterTID, int digitTID, char* type, int model, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation, glm::mat4 baseTrans) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindTexture(GL_TEXTURE_2D, letterTID);
	glBindVertexArray(vaoCubeTri);

	//For R


	//Left

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	worldMatrix = (groupMatrixW) * (groupMatrix)*baseTrans * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Top

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Right

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Under

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Round

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Front tail

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 2.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//For 4

	glBindTexture(GL_TEXTURE_2D, digitTID);
	//Front

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Back

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Middle

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	glBindVertexArray(0);
}

void draw_L0(int vaoCubeTri, int letterTID, int digitTID, char* type, int model, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation, glm::mat4 baseTrans)
{

	glBindVertexArray(vaoCubeTri);


	//make 'L'
	glBindTexture(GL_TEXTURE_2D, letterTID);
	//make cube to cuboid 
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	scalingMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &scalingMatrix[0][0]);
	drawCube(type, model);


	//second cube for base of L
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, cubeHalfLength, 0.0f));
	translationMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
	drawCube(type, model);


	//make '0'
	glBindTexture(GL_TEXTURE_2D, digitTID);

	//left side of 0
	glm::mat4 worldMatrix;
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//base of 0
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, cubeHalfLength, 0.0f));
	translationMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
	drawCube(type, model);


	//right side of 0
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix * scalingMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);


	//top of 0
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	translationMatrix = (groupMatrixW)*baseTrans * (groupMatrix)*translationMatrix;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
	drawCube(type, model);

	//'L 0' completed
	glBindVertexArray(0);
}

void drawA8(int vaoCubeTri, int letterTID, int digitTID, char* type, int model, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation, glm::mat4 baseTrans)
{

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindVertexArray(vaoCubeTri);

	// --- Drawing 'A' ---
	glBindTexture(GL_TEXTURE_2D, letterTID);

	//	Left
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Top
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Middle
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Right
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	// --- Drawing '8' ----
	glBindTexture(GL_TEXTURE_2D, digitTID);

	// Left Top
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	// Left Bottom
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Middle Top
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Middle Center
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//	Middle Bottom
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 1.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	// Right Top
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	// Right Bottom
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	glBindVertexArray(0);
}

void drawA6(int vaoCubeTri, int letterTID, int digitTID, char* type, int model, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation, glm::mat4 baseTrans)
{

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindVertexArray(vaoCubeTri);

	//For A

	glBindTexture(GL_TEXTURE_2D, letterTID);

	//Left vertical side of A

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Middle horizontal side of A

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Right vertical side of A

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Top horizontal side of A

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//For 6
	glBindTexture(GL_TEXTURE_2D, digitTID);

	//Right vertical side of 6

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Left vertical side of 6

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Bottom horizontal side of 6

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 1.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Middle horizontal side of 6

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	//Top horizontal side of 6

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans * (groupMatrix) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	drawCube(type, model);

	glBindVertexArray(0);
}

void drawSphereLight(int vaoSphere, int triCount, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glBindVertexArray(vaoSphere);
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	worldMatrix = (groupMatrixW)*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount)*36);
	glBindVertexArray(0);

}

void drawSphereModel(int vaoSphere, int triCount, float cubeHalfLength, glm::mat4 groupMatrix, glm::mat4 groupMatrixW, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glm::mat4 baseR4;
	glm::mat4 baseL0;
	glm::mat4 baseA8;
	glm::mat4 baseA6;
	glm::mat4 baseV4;

	baseA6 = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f * cubeHalfLength, 0.0f, 30.0f));
	baseA8 = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f * cubeHalfLength, 0.0f, -35.0f));
	baseL0 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f * cubeHalfLength, 0.0f, -35.0f));
	baseR4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	baseV4 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f * cubeHalfLength, 0.0f, 60.0f* cubeHalfLength));

	glBindVertexArray(vaoSphere);

	//For R4
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 11.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseR4*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount) * 36);

	//For L0
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 11.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseL0*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount) * 36);

	//For A6
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 11.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseA6*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount) * 36);

	//For A8
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 11.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseA8*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount) * 36);

	//For V4
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 11.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseV4*(groupMatrix)*(transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount*triCount) * 36);

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

#if defined(PLATFORM_OSX)
	GLuint boxTextureID = loadTexture("Textures/box.jpg");
	GLuint cementTextureID = loadTexture("Textures/metal.jpg");
	GLuint tileTextureID = loadTexture("Textures/tile.jpg");
#else
	GLuint boxTextureID = loadTexture("../Assets/Textures/box.jpg");
	GLuint metalTextureID = loadTexture("../Assets/Textures/metal.jpg");
	GLuint tileTextureID = loadTexture("../Assets/Textures/marble.jpg");
#endif

	// Black background
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	// Compile and link shaders here ...
	int shaderProgram = compileAndLinkShaders("vertexShader.txt", "fragShader.txt");
	int shaderLightingProgram = compileAndLinkShaders("lampVertexShader.txt", "lampFragShader.txt");
	int TexturedShaderProgram = compileAndLinkShaders("vertexShaderTextured.txt", "fragShaderTextured.txt");
	int shaderShadow = compileAndLinkShaders("shadow_vertex.txt", "shadow_fragment.txt");
	int shaderProgramNoShadow = compileAndLinkShaders("vertexShaderNoShadow.txt","fragShaderNoShadow.txt");
	int TexturedShaderProgramShadow = compileAndLinkShaders("texturedShadowVertex.txt", "texturedShadowFragment.txt");

	//toggle texture
	bool textureOn = true;
	bool shadowOn = false;

	//boolean to keep track if an lower half shearing occured(must be followed by top half movement)
	bool m1R = false;
	bool m2R = false;
	bool m3R = false;
	bool m4R = false;
	bool m5R = false;
	bool m1L = false;
	bool m2L = false;
	bool m3L = false;
	bool m4L = false;
	bool m5L = false;
	bool m1F = false;
	bool m2F = false;
	bool m3F = false;
	bool m4F = false;
	bool m5F = false;
	bool m1B = false;
	bool m2B = false;
	bool m3B = false;
	bool m4B = false;
	bool m5B = false;

	//boolean for key hold in which direction
	bool hold1R = false;
	bool hold1L = false;
	bool hold1F = false;
	bool hold1B = false;
	bool hold2R = false;
	bool hold2L = false;
	bool hold2F = false;
	bool hold2B = false;
	bool hold3R = false;
	bool hold3L = false;
	bool hold3F = false;
	bool hold3B = false;
	bool hold4R = false;
	bool hold4L = false;
	bool hold4F = false;
	bool hold4B = false;
	bool hold5R = false;
	bool hold5L = false;
	bool hold5F = false;
	bool hold5B = false;


	//light attributes
	glm::vec3 lightPosition(0.0f, 30.0f, 0.0f);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Define and upload geometry to the GPU here ...
	float cubeHalfLength = 0.5f;
	float lineLength = 100.0f * cubeHalfLength;

	//Sphere calculation

	const double PI = 3.14159;
	const int triCount = 50;
	float Radius = 1;
	ColoredVertex* sphereVector = new ColoredVertex[12 * (triCount*triCount)];

	float x1, y1;
	float x2, y2;
	float z1, z2;
	float seg1, seg2, seg3, seg4;
	float topR, botR;

	int index = 0;

	float phiSphere = 0.0f;
	phiSphere = -triCount / 2.0f;

	for (int thetaLoop = 0; thetaLoop < triCount; thetaLoop++) {
		for (int phiLoop = 0; phiLoop < triCount; phiLoop++) {

			seg1 = (thetaLoop / (float)triCount) * 2 * PI;
			seg2 = ((thetaLoop + 1) / (float)triCount) * 2 * PI;

			seg3 = (phiSphere / (float)triCount)*PI;
			seg4 = ((phiSphere + 1) / (float)triCount)*PI;

			x1 = cos(seg1);
			x2 = cos(seg2);
			y1 = Radius * sin(seg3);
			y2 = Radius * sin(seg4);
			z1 = sin(seg1);
			z2 = sin(seg2);
			
			topR = Radius * cos(seg3);
			botR = Radius * cos(seg4);

			//Triangle 1

			//Line 1
			sphereVector[index++] = ColoredVertex(glm::vec3(topR*x1, y1, topR*z1),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(x1, y1, z1) / glm::length(glm::vec3(x1, y1, z1))),

				sphereVector[index++] = ColoredVertex(glm::vec3(topR*x2, y1, topR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y1, z2) / glm::length(glm::vec3(x2, y1, z2))),

				//Line 2

				sphereVector[index++] = ColoredVertex(glm::vec3(topR*x2, y1, topR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y1, z2) / glm::length(glm::vec3(x2, y1, z2))),

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x2, y2, botR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y2, z2) / glm::length(glm::vec3(x2, y2, z2))),

				//Line 3

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x2, y2, botR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y2, z2) / glm::length(glm::vec3(x2, y2, z2))),

				sphereVector[index++] = ColoredVertex(glm::vec3(topR*x1, y1, topR*z1),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x1, y1, z1) / glm::length(glm::vec3(x1, y1, z1))),


				//Triangle 2

				//Line 1

				sphereVector[index++] = ColoredVertex(glm::vec3(topR*x1, y1, topR*z1),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x1, y1, z1) / glm::length(glm::vec3(x1, y1, z1))),

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x2, y2, botR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y2, z2) / glm::length(glm::vec3(x2, y2, z2))),

				//Line 2

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x2, y2, botR*z2),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x2, y2, z2) / glm::length(glm::vec3(x2, y2, z2))),

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x1, y2, botR*z1),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x1, y2, z1) / glm::length(glm::vec3(x1, y2, z1))),

				//Line 3

				sphereVector[index++] = ColoredVertex(glm::vec3(botR*x1, y2, botR*z1),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x1, y2, z1) / glm::length(glm::vec3(x1, y2, z1))),

				sphereVector[index++] = ColoredVertex(glm::vec3(topR*x1, y1, topR*z1),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(x1, y1, z1) / glm::length(glm::vec3(x1, y1, z1))),

			phiSphere += 1.0f;
		}
		phiSphere = -triCount / 2.0f;
	}
	
	int vaoSphere = createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[0];
	int vaoLamp = createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[1];
	int vaoSphereShadow = createVertexArrayObject(sphereVector, 36 * (triCount*triCount))[2];

	//vertical line for grid
	ColoredVertex vertexArrayVertLine[] = {
		ColoredVertex(glm::vec3(-lineLength, -lineLength, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),


		ColoredVertex(glm::vec3(-lineLength, lineLength, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, -1.0f)),
	};

	int vaoVert = createVertexArrayObject(vertexArrayVertLine, 2*3)[0];
	int vaoVertShadow = createVertexArrayObject(vertexArrayVertLine, 2 * 3)[2];

	//horizontal line for grid
	ColoredVertex vertexArrayHorzLine[] = {
		ColoredVertex(glm::vec3(-lineLength, -lineLength, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)),

		ColoredVertex(glm::vec3(lineLength, -lineLength, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, -1.0f)),
	};

	int vaoHorz = createVertexArrayObject(vertexArrayHorzLine, 2*3)[0];
	int vaoHorzShadow = createVertexArrayObject(vertexArrayHorzLine, 2 * 3)[2];

	//y coordinate axis
	ColoredVertex vertexArrayYLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f)),

		ColoredVertex(glm::vec3(0.0f, 10.0f*cubeHalfLength, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f)),
	};

	int vaoY = createVertexArrayObject(vertexArrayYLine, 2*3)[0];
	int vaoYShadow = createVertexArrayObject(vertexArrayYLine, 2 * 3)[2];

	//x coordinate axis
	ColoredVertex vertexArrayXLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),

		ColoredVertex(glm::vec3(10.0f*cubeHalfLength, 0.0f, 0.0f),
		glm::vec3(1.0f,  0.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),
	};

	int vaoX = createVertexArrayObject(vertexArrayXLine, 2*3)[0];
	int vaoXShadow = createVertexArrayObject(vertexArrayXLine, 2 * 3)[2];

	//zcoordinate axis
	ColoredVertex vertexArrayZLine[] = {
		ColoredVertex(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),

		ColoredVertex(glm::vec3(0.0f, 0.0f, 10.0f*cubeHalfLength),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(0.0f,  1.0f, 0.0f)),
	};

	int vaoZ = createVertexArrayObject(vertexArrayZLine, 2*3)[0];
	int vaoZShadow = createVertexArrayObject(vertexArrayZLine, 2 * 3)[2];

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

	int vaoCubeTri = createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[0]; //Each triangle has 6 vectors
	int vaoCubeShadow = createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[2];

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

	int texturedVaoCubeTri = createTextureVertexArrayObject(texturedCubeVertexArray, 4 * 6 * 6)[0];
	int notTexturedVaoCubeTri = createVertexArrayObject(vertexArrayCubeTriangle, 9 * 12)[0];

	TexturedColoredVertex texturedTileVertexArray[] = {

		TexturedColoredVertex(glm::vec3(5 * cubeHalfLength, 0.0f, 5 * cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(5 * cubeHalfLength, 0.0f,5 * -cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(5 * -cubeHalfLength, 0.0f,5 * -cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

		TexturedColoredVertex(glm::vec3(5 * cubeHalfLength, 0.0f,5 * cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		TexturedColoredVertex(glm::vec3(5 * -cubeHalfLength, 0.0f,5 * -cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
		TexturedColoredVertex(glm::vec3(5 * -cubeHalfLength, 0.0f,5 * cubeHalfLength), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f))

	};

	int vaoTile = createTextureVertexArrayObject(texturedTileVertexArray, 4 * 6)[0];
	int vaoTileShadow = createTextureVertexArrayObject(texturedTileVertexArray, 4 * 6)[1];

	/*
	Initialize scale and translation variables
	To be used with keyboard input presses
	*/

	int currentModel = 1;

	int modelM1 = 1;
	char* typeM1 = (char*)"triangle";
	float incrSM1 = 1.0f;
	float incrXM1 = 0.0f;
	float incrYM1 = 0.0f;
	float rotAXM1 = 0.0f;
	float rotAYM1 = 0.0f;

	int modelM2 = 2;
	char* typeM2 = (char*)"triangle";
	float incrSM2 = 1.0f;
	float incrXM2 = 0.0f;
	float incrYM2 = 0.0f;
	float rotAXM2 = 0.0f;
	float rotAYM2 = 0.0f;

	int modelM3 = 3;
	char* typeM3 = (char*)"triangle";
	float incrSM3 = 1.0f;
	float incrXM3 = 0.0f;
	float incrYM3 = 0.0f;
	float rotAXM3 = 0.0f;
	float rotAYM3 = 0.0f;

	int modelM4 = 4;
	char* typeM4 = (char*)"triangle";
	float incrSM4 = 1.0f;
	float incrXM4 = 0.0f;
	float incrYM4 = 0.0f;
	float rotAXM4 = 0.0f;
	float rotAYM4 = 0.0f;

	int modelM5 = 5;
	char* typeM5 = (char*)"triangle";
	float incrSM5 = 1.0f;
	float incrXM5 = 0.0f;
	float incrYM5 = 0.0f;
	float rotAXM5 = 0.0f;
	float rotAYM5 = 0.0f;

	//initialize world orientation variables
	float incrXW = 0.0f;
	float incrYW = 0.0f;
	float rotAXW = 0.0f;
	float rotAYW = 0.0f;


	glm::mat4 shear1R;
	shear1R[0] = glm::vec4(1, 0, 0, 0);
	shear1R[1] = glm::vec4(-1.0, 1, 0, 0);
	shear1R[2] = glm::vec4(0, 0, 1, 0);
	shear1R[3] = glm::vec4(5.0, 0, 0, 1);

	glm::mat4 shear2R;
	shear2R[0] = glm::vec4(1, 0, 0, 0);
	shear2R[1] = glm::vec4(1.0, 1, 0, 0);
	shear2R[2] = glm::vec4(0, 0, 1, 0);
	shear2R[3] = glm::vec4(0, 0, 0, 1);

	glm::mat4 shear1L;
	shear1L[0] = glm::vec4(1, 0, 0, 0);
	shear1L[1] = glm::vec4(1.0, 1, 0, 0);
	shear1L[2] = glm::vec4(0, 0, 1, 0);
	shear1L[3] = glm::vec4(-5.0, 0, 0, 1);

	glm::mat4 shear2L;
	shear2L[0] = glm::vec4(1, 0, 0, 0);
	shear2L[1] = glm::vec4(-1.0, 1, 0, 0);
	shear2L[2] = glm::vec4(0, 0, 1, 0);
	shear2L[3] = glm::vec4(0, 0, 0, 1);

	glm::mat4 shear1F;
	shear1F[0] = glm::vec4(1, 0, 0, 0);
	shear1F[1] = glm::vec4(0, 1, -1.0, 0);
	shear1F[2] = glm::vec4(0, 0, 1, 0);
	shear1F[3] = glm::vec4(0, 0, 5.0, 1);

	glm::mat4 shear2F;
	shear2F[0] = glm::vec4(1, 0, 0, 0);
	shear2F[1] = glm::vec4(0, 1, 1.0, 0);
	shear2F[2] = glm::vec4(0, 0, 1, 0);
	shear2F[3] = glm::vec4(0, 0, 0, 1);

	glm::mat4 shear1B;
	shear1B[0] = glm::vec4(1, 0, 0, 0);
	shear1B[1] = glm::vec4(0, 1, 1.0, 0);
	shear1B[2] = glm::vec4(0, 0, 1, 0);
	shear1B[3] = glm::vec4(0, 0, -5.0, 1);

	glm::mat4 shear2B;
	shear2B[0] = glm::vec4(1, 0, 0, 0);
	shear2B[1] = glm::vec4(0, 1, -1.0, 0);
	shear2B[2] = glm::vec4(0, 0, 1, 0);
	shear2B[3] = glm::vec4(0, 0, 0, 1);

	//Initialize model matrices
	glm::mat4 transGroupM1 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM1, incrYM1, 0.0f));
	glm::mat4 roGroupXM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM1), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM1), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYM1 = glm::mat4(1.0f);
	glm::mat4 scaleGroupM1 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM1, incrSM1, incrSM1));
	glm::mat4 shear1M1 = glm::mat4(1.0f);
	glm::mat4 shear2M1 = glm::mat4(1.0f);
	glm::mat4 baseTransM1 = glm::mat4(1.0f);
	glm::mat4 groupMatrixM1 = transGroupM1 * roGroupXYM1 * scaleGroupM1 * shear1M1 * shear2M1;

	glm::mat4 transGroupM2 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM2, incrYM2, 0.0f));
	glm::mat4 roGroupXM2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM2), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYM2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM2), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYM2 = glm::mat4(1.0f);
	glm::mat4 scaleGroupM2 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM2, incrSM2, incrSM2));
	glm::mat4 shear1M2 = glm::mat4(1.0f);
	glm::mat4 shear2M2 = glm::mat4(1.0f);
	glm::mat4 baseTransM2 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f * cubeHalfLength, 0.0f, -35.0f));
	glm::mat4 groupMatrixM2 = transGroupM2 * roGroupXYM2 * scaleGroupM2;

	glm::mat4 transGroupM3 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM3, incrYM3, 0.0f));
	glm::mat4 roGroupXM3 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM3), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYM3 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM3), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYM3 = glm::mat4(1.0f);
	glm::mat4 scaleGroupM3 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM3, incrSM3, incrSM3));
	glm::mat4 shear1M3 = glm::mat4(1.0f);
	glm::mat4 shear2M3 = glm::mat4(1.0f);
	glm::mat4 baseTransM3 = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f * cubeHalfLength, 0.0f, -35.0f));
	glm::mat4 groupMatrixM3 = transGroupM3 * roGroupXYM3 * scaleGroupM3;

	glm::mat4 transGroupM4 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM4, incrYM4, 0.0f));
	glm::mat4 roGroupXM4 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM4), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYM4 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM4), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYM4 = glm::mat4(1.0f);
	glm::mat4 scaleGroupM4 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM4, incrSM4, incrSM4));
	glm::mat4 shear1M4 = glm::mat4(1.0f);
	glm::mat4 shear2M4 = glm::mat4(1.0f);
	glm::mat4 baseTransM4 = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f * cubeHalfLength, 0.0f, 60.0f * cubeHalfLength));
	glm::mat4 groupMatrixM4 = transGroupM4 * roGroupXYM4 * scaleGroupM4;

	glm::mat4 transGroupM5 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM5, incrYM5, 0.0f));
	glm::mat4 roGroupXM5 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM5), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYM5 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM5), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYM5 = glm::mat4(1.0f);
	glm::mat4 scaleGroupM5 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM5, incrSM5, incrSM5));
	glm::mat4 shear1M5 = glm::mat4(1.0f);
	glm::mat4 shear2M5 = glm::mat4(1.0f);
	glm::mat4 baseTransM5 = glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f * cubeHalfLength, 0.0f, 30.0f));
	glm::mat4 groupMatrixM5 = transGroupM5 * roGroupXYM5 * scaleGroupM5;

	//Initialize world orientation matrix
	glm::mat4 transWorld = glm::translate(glm::mat4(1.0f), glm::vec3(incrXW, incrYW, 0.0f));
	glm::mat4 roGroupXW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXW), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYW), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 roGroupXYW = glm::mat4(1.0f);
	glm::mat4 groupMatrixW = transWorld * roGroupXYW;

	//Initiate Lighting model matrix
	glm::mat4 transGroupML = glm::translate(glm::mat4(1.0f), lightPosition);
	glm::mat4 scaleGroupML = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,1.0f,1.0f));
	glm::mat4 groupMatrixML = transGroupML * scaleGroupML;

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

	// Dimensions of the shadow texture, which should cover the viewport window size and shouldn't be oversized and waste resources
	

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

	float lightAngleOuter = 30.0;
	float lightAngleInner = 20.0;
	// Set light cutoff angles on scene shader
	SetUniform1Value(shaderProgram, "light_cutoff_inner", cos(glm::radians(lightAngleInner)));
	SetUniform1Value(shaderProgram, "light_cutoff_outer", cos(glm::radians(lightAngleOuter)));

	//Camera settings
	float cameraHorizontalAngle = -90.0f;
	float cameraVerticalAngle = 0.0f;
	glm::vec3 cameraPosition(0.0f, 6.0f, 25.0f);
	glm::vec3 cameraLookAt(0.0f, -12.0f, -50.0f);
	glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

	float saveTheta = glm::radians(-90.0f);
	float savePhi = glm::radians(-13.496f); // Pre-calculated to keep facing origin. Using pythagoras.

	/*
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Other OpenGL states to set once
	//Enable Backface culling
	glEnable(GL_CULL_FACE);
    */

	// Entering Main Loop
	while (!glfwWindowShouldClose(window))
	{

		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;


		// Each frame, reset color of each pixel to glClearColor and clear depth
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//projection matrix, perspective

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f),  // field of view in degrees
			1024.0f / 768.0f,      // aspect ratio
			1.0f, 5000.0f);       // near and far (near > 0)

		setProjectionMatrix(TexturedShaderProgram, projectionMatrix);
		setProjectionMatrix(shaderProgram, projectionMatrix);
		setProjectionMatrix(shaderProgramNoShadow, projectionMatrix);
		setProjectionMatrix(TexturedShaderProgramShadow, projectionMatrix);
		//view matrix

		//Camera is currently setup to look at the center of the grid.
		glm::mat4 viewMatrix = glm::lookAt(cameraPosition,  // eye (camera location)
			cameraPosition + cameraLookAt,  // center (Where is the camera looking)
			cameraUp);// up

		viewMatrix = zoom * viewMatrix;

		setViewMatrix(TexturedShaderProgram, viewMatrix);
		setViewMatrix(shaderProgram, viewMatrix);
		setViewMatrix(shaderProgramNoShadow, viewMatrix);
		setViewMatrix(TexturedShaderProgramShadow, viewMatrix);
		//Initialize light for both shaders


		//Untextured shadow
		glUseProgram(shaderProgram);
		
		GLint lightColorLocUT = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3f(lightColorLocUT, 1.0f, 1.0f, 1.0f);
		
        SetUniformVec3(shaderProgram, "lightPos", lightPosition);
	
		GLint viewPosLocUT = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3f(viewPosLocUT, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//Untextured No shadow
		glUseProgram(shaderProgramNoShadow);

		lightColorLocUT = glGetUniformLocation(shaderProgramNoShadow, "lightColor");
		glUniform3f(lightColorLocUT, 1.0f, 1.0f, 1.0f);

		SetUniformVec3(shaderProgramNoShadow, "lightPos", lightPosition);

		viewPosLocUT = glGetUniformLocation(shaderProgramNoShadow, "viewPos");
		glUniform3f(viewPosLocUT, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//textured No Shadow
		glUseProgram(TexturedShaderProgram);

		GLint lightColorLocT = glGetUniformLocation(TexturedShaderProgram, "lightColor");
		glUniform3f(lightColorLocT, 1.0f, 1.0f, 1.0f);

		GLint lightPosLocT = glGetUniformLocation(TexturedShaderProgram, "lightPos");
		glUniform3f(lightPosLocT, lightPosition.x, lightPosition.y, lightPosition.z);

		
		GLint viewPosLocT = glGetUniformLocation(TexturedShaderProgram, "viewPos");
		glUniform3f(viewPosLocT, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//textured shadow
		lightColorLocT = glGetUniformLocation(TexturedShaderProgramShadow, "lightColor");
		glUniform3f(lightColorLocT, 1.0f, 1.0f, 1.0f);

		lightPosLocT = glGetUniformLocation(TexturedShaderProgramShadow, "lightPos");
		glUniform3f(lightPosLocT, lightPosition.x, lightPosition.y, lightPosition.z);


		viewPosLocT = glGetUniformLocation(TexturedShaderProgramShadow, "viewPos");
		glUniform3f(viewPosLocT, cameraPosition.x, cameraPosition.y, cameraPosition.z);


		//Update Model matrices
		transGroupM1 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM1, incrYM1, 0.0f));
		roGroupXM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM1), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYM1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM1), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleGroupM1 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM1, incrSM1, incrSM1));
		groupMatrixM1 = transGroupM1 * roGroupXYM1 * scaleGroupM1 * shear1M1 * shear2M1;


		transGroupM2 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM2, incrYM2, 0.0f));
		roGroupXM2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM2), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYM2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM2), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleGroupM2 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM2, incrSM2, incrSM2));
		groupMatrixM2 = transGroupM2 * roGroupXYM2 * scaleGroupM2 * shear1M2 * shear2M2;

		transGroupM3 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM3, incrYM3, 0.0f));
		roGroupXM3 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM3), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYM3 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM3), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleGroupM3 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM3, incrSM3, incrSM3));
		groupMatrixM3 = transGroupM3 * roGroupXYM3 * scaleGroupM3 * shear1M3 * shear2M3;

		transGroupM4 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM4, incrYM4, 0.0f));
		roGroupXM4 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM4), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYM4 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM4), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleGroupM4 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM4, incrSM4, incrSM4));
		groupMatrixM4 = transGroupM4 * roGroupXYM4 * scaleGroupM4 * shear1M4 * shear2M4;

		transGroupM5 = glm::translate(glm::mat4(1.0f), glm::vec3(incrXM5, incrYM5, 0.0f));
		roGroupXM5 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXM5), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYM5 = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYM5), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleGroupM5 = glm::scale(glm::mat4(1.0f), glm::vec3(incrSM5, incrSM5, incrSM5));
		groupMatrixM5 = transGroupM5 * roGroupXYM5 * scaleGroupM5 * shear1M5 * shear2M5;

		//Update world orientation matrix
		transWorld = glm::translate(glm::mat4(1.0f), glm::vec3(incrXW, incrYW, 0.0f));
		roGroupXW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXW), glm::vec3(1.0f, 0.0f, 0.0f));
		roGroupYW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYW), glm::vec3(0.0f, 1.0f, 0.0f));
		groupMatrixW = transWorld * roGroupXYW;

		
		//light parameters
		//lightPosition = glm::vec3(sinf(glfwGetTime()* 3.141592f), sinf(glfwGetTime() *30* 3.141592f), cosf(glfwGetTime() * 3.141592f));
		glm::vec3 lightFocus(0.0f, 0.0f, 0.0f);      // the point in 3D space the light "looks" at
		glm::vec3 lightDirection = normalize(lightFocus - lightPosition);

		//float lightNearPlane = 1.0f;
		//float lightFarPlane = 100.0f;

		float lightNearPlane = 1.0f, lightFarPlane = 100.0f;
		//glm::mat4 lightProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);

		glm::mat4 lightProjectionMatrix = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, lightNearPlane, lightFarPlane);
		
		glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, lightFocus, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		// Set light space matrix 
		SetUniformMat4(shaderShadow, "light_view_proj_matrix", lightSpaceMatrix);
		SetUniformMat4(shaderProgram, "light_view_proj_matrix", lightSpaceMatrix);
		SetUniformMat4(TexturedShaderProgramShadow, "light_view_proj_matrix", lightSpaceMatrix);

		// Set light far and near planes on scene shader
		SetUniform1Value(shaderProgram, "light_near_plane", lightNearPlane);
		SetUniform1Value(shaderProgram, "light_far_plane", lightFarPlane);

		SetUniform1Value(TexturedShaderProgramShadow, "light_near_plane", lightNearPlane);
		SetUniform1Value(TexturedShaderProgramShadow, "light_far_plane", lightFarPlane);
		
		// Set light position on scene shader
		SetUniformVec3(shaderProgram, "lightPos", lightPosition);

		// Set light direction on scene shader
		SetUniformVec3(shaderProgram, "light_direction", lightDirection);

		// Set light position on scene shader
		SetUniformVec3(TexturedShaderProgramShadow, "lightPos", lightPosition);

		// Set light direction on scene shader
		SetUniformVec3(TexturedShaderProgramShadow, "light_direction", lightDirection);
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
		
		drawGridVBO(vaoVert, vaoHorz, lineLength, groupMatrixW, worldMatrixLocation);

		//Draw R4 (Marc)
		drawR4(vaoCubeShadow, boxTextureID, metalTextureID, typeM1, modelM1, cubeHalfLength, groupMatrixM1, groupMatrixW, worldMatrixLocation,baseTransM1);

		//Draw L0 (Nalveer)
		draw_L0(vaoCubeShadow, boxTextureID, metalTextureID, typeM2, modelM2, cubeHalfLength, groupMatrixM2, groupMatrixW, worldMatrixLocation,baseTransM2);

		//Draw A8 (Noah)
		drawA8(vaoCubeShadow, boxTextureID, metalTextureID, typeM3, modelM3, cubeHalfLength, groupMatrixM3, groupMatrixW, worldMatrixLocation,baseTransM3);

		//Draw V4 (Hersh)
		drawV4(vaoCubeShadow, boxTextureID, metalTextureID, typeM4, modelM4, cubeHalfLength, groupMatrixM4, groupMatrixW, worldMatrixLocation,baseTransM4);

		//Draw A6 (Sean)
		drawA6(vaoCubeShadow, boxTextureID, metalTextureID, typeM5, modelM5, cubeHalfLength, groupMatrixM5, groupMatrixW, worldMatrixLocation,baseTransM5);

		//Draw Coordinate
		drawCoordinate(vaoXShadow, vaoYShadow, vaoZShadow, groupMatrixW, worldMatrixLocation);

		//Draw Sphere
		drawSphereModel(vaoSphereShadow, triCount, cubeHalfLength, groupMatrixMS, groupMatrixW, worldMatrixLocation);
		
		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/////////////////////////////////////END FIRST RENDER//////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////START SECOND RENDER//////////////////////////////////////////////////////////////////////////////////
		
		
		
		glViewport(0, 0, 1024, 768);


		// Clear color and depth data on framebuffer
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Flip shader based on toggle
		if (shadowOn && !textureOn) {
			vaoCubeTri = notTexturedVaoCubeTri;
			glUseProgram(shaderProgram);
			worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		}
		else if (!shadowOn && !textureOn) {
			vaoCubeTri = notTexturedVaoCubeTri;
			glUseProgram(shaderProgramNoShadow);
			worldMatrixLocation = glGetUniformLocation(shaderProgramNoShadow, "worldMatrix");
		}
		else if (textureOn && shadowOn) {
			
			vaoCubeTri = texturedVaoCubeTri;
			glUseProgram(TexturedShaderProgramShadow);
			worldMatrixLocation = glGetUniformLocation(TexturedShaderProgramShadow, "worldMatrix");
			
		}
		else {
			vaoCubeTri = texturedVaoCubeTri;
			glUseProgram(TexturedShaderProgram);
			worldMatrixLocation = glGetUniformLocation(TexturedShaderProgram, "worldMatrix");
		}
		
        glBindTexture(GL_TEXTURE_2D, depth_map_texture);
		glActiveTexture(GL_TEXTURE0);

		//Draw Grid
		if (textureOn) {
			drawGridTiles(vaoTile, tileTextureID, groupMatrixW, worldMatrixLocation);
		}
		else {
			drawGridVBO(vaoVert, vaoHorz, lineLength, groupMatrixW, worldMatrixLocation);
		}

		glBindTexture(GL_TEXTURE_2D, depth_map_texture);

		//Draw R4 (Marc)
		drawR4(vaoCubeTri, boxTextureID, metalTextureID, typeM1, modelM1, cubeHalfLength, groupMatrixM1, groupMatrixW, worldMatrixLocation,baseTransM1);

	
		//Draw L0 (Nalveer)
		draw_L0(vaoCubeTri, boxTextureID, metalTextureID, typeM2, modelM2, cubeHalfLength, groupMatrixM2, groupMatrixW, worldMatrixLocation,baseTransM2);

		//Draw A8 (Noah)
		drawA8(vaoCubeTri, boxTextureID, metalTextureID, typeM3, modelM3, cubeHalfLength, groupMatrixM3, groupMatrixW, worldMatrixLocation,baseTransM3);

		//Draw V4 (Hersh)
		drawV4(vaoCubeTri, boxTextureID, metalTextureID, typeM4, modelM4, cubeHalfLength, groupMatrixM4, groupMatrixW, worldMatrixLocation,baseTransM4);

		//Draw A6 (Sean)
		drawA6(vaoCubeTri, boxTextureID, metalTextureID, typeM5, modelM5, cubeHalfLength, groupMatrixM5, groupMatrixW, worldMatrixLocation,baseTransM5);


		//Draw untextured elements
		if (shadowOn) {
			glUseProgram(shaderProgram);
			worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
		}
		else {
			glUseProgram(shaderProgramNoShadow);
			worldMatrixLocation = glGetUniformLocation(shaderProgramNoShadow, "worldMatrix");
		}
		glBindTexture(GL_TEXTURE_2D, depth_map_texture);
		glActiveTexture(GL_TEXTURE0);
		
		//Draw Coordinate
		drawCoordinate(vaoX, vaoY, vaoZ, groupMatrixW, worldMatrixLocation);

		//Draw Sphere
		drawSphereModel(vaoSphere, triCount, cubeHalfLength, groupMatrixMS, groupMatrixW, worldMatrixLocation);
		
		/////////////////////////////////////END SECOND RENDER//////////////////////////////////////////////////////////////////////////////////

		//lighting 
		
		glUseProgram(shaderLightingProgram);

		GLint lightColorLocLight = glGetUniformLocation(shaderLightingProgram, "lightColor");
		glUniform3f(lightColorLocLight, 1.0f, 1.0f, 1.0f);

		GLuint projectionMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "projectionMatrix");
		glUniformMatrix4fv(projectionMatrixLocationLight, 1, GL_FALSE, &projectionMatrix[0][0]);

		GLuint viewMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "viewMatrix");
		glUniformMatrix4fv(viewMatrixLocationLight, 1, GL_FALSE, &viewMatrix[0][0]);

		GLuint worldMatrixLocationLight = glGetUniformLocation(shaderLightingProgram, "worldMatrix");
		
		//draw light sphere

		drawSphereLight(vaoLamp, triCount, groupMatrixML, groupMatrixW, worldMatrixLocationLight);

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

		// Convert to spherical coordinates
		const float cameraAngularSpeed = 60.0f / 10.0f;
		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
		cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

		float theta = glm::radians(cameraHorizontalAngle);
		float phi = glm::radians(cameraVerticalAngle);

		// Handle inputs

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			//pan 
			cameraLookAt = glm::vec3(1.0f*cosf(phi)*cosf(theta), 1.0f*sinf(savePhi), 1.0f*cosf(savePhi)*sinf(theta));
			saveTheta = theta; //saveTheta and savePhi keeps last theta position or phi position respectively
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
			//tilt
			cameraLookAt = glm::vec3(1.0f*cosf(phi)*cosf(saveTheta), 1.0f*sinf(phi), 1.0f*cosf(phi)*sinf(saveTheta));
			savePhi = phi;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			//zoom
			zoom = glm::translate(zoom, glm::vec3(0.0f, 0.0f, dy));

		}

		// by default, camera is centered at the origin and look towards negative z-axis
		//set variable model to set which model we are working with(1-5)
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			currentModel = 1;
		}

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			currentModel = 2;
		}

		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			currentModel = 3;
		}

		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			currentModel = 4;
		}

		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		{
			currentModel = 5;
		}

		//Scale up
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		{
			if (currentModel == 1)
				incrSM1 += .01;

			if (currentModel == 2)
				incrSM2 += .01;

			if (currentModel == 3)
				incrSM3 += .01;

			if (currentModel == 4)
				incrSM4 += .01;

			if (currentModel == 5)
				incrSM5 += .01;
		}

		//Scale down
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		{
			if (currentModel == 1)
				incrSM1 -= .01;

			if (currentModel == 2)
				incrSM2 -= .01;

			if (currentModel == 3)
				incrSM3 -= .01;

			if (currentModel == 4)
				incrSM4 -= .01;

			if (currentModel == 5)
				incrSM5 -= .01;
		}

		//Translate left
		if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1)
				incrXM1 -= .1;

			if (currentModel == 2)
				incrXM2 -= .1;

			if (currentModel == 3)
				incrXM3 -= .1;

			if (currentModel == 4)
				incrXM4 -= .1;

			if (currentModel == 5)
				incrXM5 -= .1;
		}

		//Translate right
		if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1)
				incrXM1 += .1;

			if (currentModel == 2)
				incrXM2 += .1;

			if (currentModel == 3)
				incrXM3 += .1;

			if (currentModel == 4)
				incrXM4 += .1;

			if (currentModel == 5)
				incrXM5 += .1;
		}

		//Translate up
		if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1)
				incrYM1 += .1;

			if (currentModel == 2)
				incrYM2 += .1;

			if (currentModel == 3)
				incrYM3 += .1;

			if (currentModel == 4)
				incrYM4 += .1;

			if (currentModel == 5)
				incrYM5 += .1;
		}

		//Translate down
		if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) & (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1)
				incrYM1 -= .1;

			if (currentModel == 2)
				incrYM2 -= .1;

			if (currentModel == 3)
				incrYM3 -= .1;

			if (currentModel == 4)
				incrYM4 -= .1;

			if (currentModel == 5)
				incrYM5 -= .1;
		}

		//Rotate left in y
		if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) & !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{

			if (currentModel == 1) {

				rotAYM1 -= 5.0f;
				roGroupXYM1 = roGroupYM1 * roGroupXM1;
			}

			if (currentModel == 2) {

				rotAYM2 -= 5.0f;
				roGroupXYM2 = roGroupYM2 * roGroupXM2;
			}

			if (currentModel == 3) {

				rotAYM3 -= 5.0f;
				roGroupXYM3 = roGroupYM3 * roGroupXM3;
			}

			if (currentModel == 4) {

				rotAYM4 -= 5.0f;
				roGroupXYM4 = roGroupYM4 * roGroupXM4;
			}

			if (currentModel == 5) {

				rotAYM5 -= 5.0f;
				roGroupXYM5 = roGroupYM5 * roGroupXM5;
			}

		}

		//Rotate right in y
		if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) & !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1) {
				rotAYM1 += 5.0f;
				roGroupXYM1 = roGroupYM1 * roGroupXM1;
			}

			if (currentModel == 2) {

				rotAYM2 += 5.0f;
				roGroupXYM2 = roGroupYM2 * roGroupXM2;
			}

			if (currentModel == 3) {

				rotAYM3 += 5.0f;
				roGroupXYM3 = roGroupYM3 * roGroupXM3;
			}

			if (currentModel == 4) {

				rotAYM4 += 5.0f;
				roGroupXYM4 = roGroupYM4 * roGroupXM4;
			}

			if (currentModel == 5) {

				rotAYM5 += 5.0f;
				roGroupXYM5 = roGroupYM5 * roGroupXM5;
			}
		}

		//Rotate up in x
		if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) & !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1) {

				rotAXM1 -= 5.0f;
				roGroupXYM1 = roGroupXM1 * roGroupYM1;
			}

			if (currentModel == 2) {

				rotAXM2 -= 5.0f;
				roGroupXYM2 = roGroupXM2 * roGroupYM2;
			}

			if (currentModel == 3) {

				rotAXM3 -= 5.0f;
				roGroupXYM3 = roGroupXM3 * roGroupYM3;
			}

			if (currentModel == 4) {

				rotAXM4 -= 5.0f;
				roGroupXYM4 = roGroupXM4 * roGroupYM4;
			}

			if (currentModel == 5) {

				rotAXM5 -= 5.0f;
				roGroupXYM5 = roGroupXM5 * roGroupYM5;
			}
		}

		//Rotate down in x
		if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) & !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
		{
			if (currentModel == 1) {
				rotAXM1 += 5.0f;
				roGroupXYM1 = roGroupXM1 * roGroupYM1;
			}

			if (currentModel == 2) {

				rotAXM2 += 5.0f;
				roGroupXYM2 = roGroupXM2 * roGroupYM2;
			}

			if (currentModel == 3) {

				rotAXM3 += 5.0f;
				roGroupXYM3 = roGroupXM3 * roGroupYM3;
			}

			if (currentModel == 4) {

				rotAXM4 += 5.0f;
				roGroupXYM4 = roGroupXM4 * roGroupYM4;
			}

			if (currentModel == 5) {

				rotAXM5 += 5.0f;
				roGroupXYM5 = roGroupXM5 * roGroupYM5;
			}
		}

		//Draw points for models
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			if (currentModel == 1)
				typeM1 = (char*)"point";

			if (currentModel == 2)
				typeM2 = (char*)"point";

			if (currentModel == 3)
				typeM3 = (char*)"point";

			if (currentModel == 4)
				typeM4 = (char*)"point";

			if (currentModel == 5)
				typeM5 = (char*)"point";

		}

		//Draw triangles for models
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			if (currentModel == 1)
				typeM1 = (char*)"triangle";

			if (currentModel == 2)
				typeM2 = (char*)"triangle";

			if (currentModel == 3)
				typeM3 = (char*)"triangle";

			if (currentModel == 4)
				typeM4 = (char*)"triangle";

			if (currentModel == 5)
				typeM5 = (char*)"triangle";

		}

		//Draw lines for models
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			if (currentModel == 1)
				typeM1 = (char*)"line";

			if (currentModel == 2)
				typeM2 = (char*)"line";

			if (currentModel == 3)
				typeM3 = (char*)"line";

			if (currentModel == 4)
				typeM4 = (char*)"line";

			if (currentModel == 5)
				typeM5 = (char*)"line";

		}

		//World orientation
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			rotAXW += 1.0f;
			roGroupXYW = roGroupXW * roGroupYW;

		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			rotAXW -= 1.0f;
			roGroupXYW = roGroupXW * roGroupYW;

		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			rotAYW += 1.0f;
			roGroupXYW = roGroupYW * roGroupXW;

		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {


			rotAYW -= 1.0f;
			roGroupXYW = roGroupYW * roGroupXW;

		}
		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {

			rotAXW = 0.0f;
			rotAYW = 0.0f;
			roGroupXYW = glm::mat4(1.0f);
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

		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)  //press x to toggle texture
		{
			//sleep to toggle only once(due to frame speed)
			_sleep(100);

			//toggle
			if (textureOn == true) {
				textureOn = false;
			}
			else {
				textureOn = true;
			}

		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  //press x to toggle texture
		{
			//sleep to toggle only once(due to frame speed)
			_sleep(100);

			//toggle
			if (shadowOn == true) {
				shadowOn = false;
			}
			else {
				shadowOn = true;
			}

		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{

			if (currentModel == 1) {
				incrXM1 = 0;
				incrYM1 = 0;

				float x = rand() % 81 - 40;
				float z = rand() % 81 - 40;
				_sleep(150);
				baseTransM1 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));

			}
			if (currentModel == 2) {
				incrXM2 = 0;
				incrYM2 = 0;

				float x = rand() % 81 - 40;
				float z = rand() % 81 - 40;
				_sleep(150);
				baseTransM2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
			}
			if (currentModel == 3) {
				incrXM3 = 0;
				incrYM3 = 0;

				float x = rand() % 81 - 40;
				float z = rand() % 81 - 40;
				_sleep(150);
				baseTransM3 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
			}
			if (currentModel == 4) {
				incrXM4 = 0;
				incrYM4 = 0;

				float x = rand() % 81 - 40;
				float z = rand() % 81 - 40;
				_sleep(150);
				baseTransM4 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
			}
			if (currentModel == 5) {
				incrXM5 = 0;
				incrYM5 = 0;

				float x = rand() % 81 - 40;
				float z = rand() % 81 - 40;
				_sleep(150);
				baseTransM5 = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
			}
		}

		//upper m1 shear
		if (m1R) {
			_sleep(550);
			shear2M1 = shear2M1 * shear2R;
			m1R = false;
			hold1R = true;
		}
		else { hold1R = false; }
		if (m1L) {
			_sleep(550);
			shear2M1 = shear2M1 * shear2L;
			m1L = false;
			hold1L = true;
		}
		else { hold1L = false; }
		if (m1F) {
			_sleep(550);
			shear2M1 = shear2M1 * shear2F;
			m1F = false;
			hold1F = true;
		}
		else { hold1F = false; }
		if (m1B) {
			_sleep(550);
			shear2M1 = shear2M1 * shear2B;
			m1B = false;
			hold1B = true;
		}
		else { hold1B = false; }
		//upper m2 shear
		if (m2R) {
			_sleep(550);
			shear2M2 = shear2M2 * shear2R;
			m2R = false;
			hold2R = true;
		}
		else { hold2R = false; }
		if (m2L) {
			_sleep(550);
			shear2M2 = shear2M2 * shear2L;
			m2L = false;
			hold2L = true;
		}
		else { hold2L = false; }
		if (m2F) {
			_sleep(550);
			shear2M2 = shear2M2 * shear2F;
			m2F = false;
			hold2F = true;
		}
		else { hold2F = false; }
		if (m2B) {
			_sleep(550);
			shear2M2 = shear2M2 * shear2B;
			m2B = false;
			hold2B = true;
		}
		else { hold2B = false; }
		//m3
		if (m3R) {
			_sleep(550);
			shear2M3 = shear2M3 * shear2R;
			m3R = false;
			hold3R = true;
		}
		else { hold3R = false; }
		if (m3L) {
			_sleep(550);
			shear2M3 = shear2M3 * shear2L;
			m3L = false;
			hold3L = true;
		}
		else { hold3L = false; }
		if (m3F) {
			_sleep(550);
			shear2M3 = shear2M3 * shear2F;

			m3F = false;
			hold3F = true;
		}
		else { hold3F = false; }
		if (m3B) {
			_sleep(550);
			shear2M3 = shear2M3 * shear2B;
			m3B = false;
			hold3B = true;
		}
		else { hold3B = false; }
		//m4
		if (m4R) {
			_sleep(550);
			shear2M4 = shear2M4 * shear2R;
			m4R = false;
			hold4R = true;
		}
		else { hold4R = false; }
		if (m4L) {
			_sleep(550);
			shear2M4 = shear2M4 * shear2L;
			m4L = false;
			hold4L = true;
		}
		else { hold4L = false; }
		if (m4F) {
			_sleep(550);
			shear2M4 = shear2M4 * shear2F;
			m4F = false;
			hold4F = true;
		}
		else { hold4F = false; }
		if (m4B) {
			_sleep(550);
			shear2M4 = shear2M4 * shear2B;
			m4B = false;
			hold4B = true;
		}
		else { hold4B = false; }
		//m5
		if (m5R) {
			_sleep(550);
			shear2M5 = shear2M5 * shear2R;
			m5R = false;
			hold5R = true;
		}
		else { hold5R = false; }
		if (m5L) {
			_sleep(550);
			shear2M5 = shear2M5 * shear2L;
			m5L = false;
			hold5L = true;
		}
		else { hold5L = false; }
		if (m5F) {
			_sleep(550);
			shear2M5 = shear2M5 * shear2F;
			m5F = false;
			hold5F = true;
		}
		else { hold5F = false; }
		if (m5B) {
			_sleep(550);
			shear2M5 = shear2M5 * shear2B;
			m5B = false;
			hold5B = true;
		}
		else { hold5B = false; }
		//press right arrow to walk right
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			//lower half shear
			if (currentModel == 1 && !hold1R) {
				_sleep(100);
				shear1M1 = shear1M1 * shear1R;
				m1R = true;

			}
			if (currentModel == 2 && !hold2R) {
				_sleep(100);
				shear1M2 = shear1M2 * shear1R;
				m2R = true;
			}
			if (currentModel == 3 && !hold3R) {
				_sleep(100);
				shear1M3 = shear1M3 * shear1R;
				m3R = true;
			}
			if (currentModel == 4 && !hold4R) {
				_sleep(100);
				shear1M4 = shear1M4 * shear1R;
				m4R = true;
			}
			if (currentModel == 5 && !hold5R) {
				_sleep(100);
				shear1M5 = shear1M5 * shear1R;
				m5R = true;
			}
		}
		//press left arrow to walk left
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{

			if (currentModel == 1 && !hold1L) {
				_sleep(100);
				shear1M1 = shear1M1 * shear1L;
				m1L = true;
			}
			if (currentModel == 2 && !hold2L) {
				_sleep(100);
				shear1M2 = shear1M2 * shear1L;
				m2L = true;
			}
			if (currentModel == 3 && !hold3L) {
				_sleep(100);
				shear1M3 = shear1M3 * shear1L;
				m3L = true;
			}
			if (currentModel == 4 && !hold4L) {
				_sleep(100);
				shear1M4 = shear1M4 * shear1L;
				m4L = true;
			}
			if (currentModel == 5 && !hold5L) {
				_sleep(100);
				shear1M5 = shear1M5 * shear1L;
				m5L = true;
			}
		}
		//press up arrow to walk forward
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{

			if (currentModel == 1 && !hold1F) {
				_sleep(100);
				shear1M1 = shear1M1 * shear1F;
				m1F = true;
			}
			if (currentModel == 2 && !hold2F) {
				_sleep(100);
				shear1M2 = shear1M2 * shear1F;
				m2F = true;
			}
			if (currentModel == 3 && !hold3F) {
				_sleep(100);
				shear1M3 = shear1M3 * shear1F;
				m3F = true;
			}
			if (currentModel == 4 && !hold4F) {
				_sleep(100);
				shear1M4 = shear1M4 * shear1F;
				m4F = true;
			}
			if (currentModel == 5 && !hold5F) {
				_sleep(100);
				shear1M5 = shear1M5 * shear1F;
				m5F = true;
			}
		}
		//press down arrow to walk backward
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{

			if (currentModel == 1 && !hold1B) {
				_sleep(100);
				shear1M1 = shear1M1 * shear1B;
				m1B = true;
			}
			if (currentModel == 2 && !hold2B) {
				_sleep(100);
				shear1M2 = shear1M2 * shear1B;
				m2B = true;
			}
			if (currentModel == 3 && !hold3B) {
				_sleep(100);
				shear1M3 = shear1M3 * shear1B;
				m3B = true;
			}
			if (currentModel == 4 && !hold4B) {
				_sleep(100);
				shear1M4 = shear1M4 * shear1B;
				m4B = true;
			}
			if (currentModel == 5 && !hold5B) {
				_sleep(100);
				shear1M5 = shear1M5 * shear1B;
				m5B = true;
			}
		}


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