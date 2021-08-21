#pragma once
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



using glm::mat4;
using glm::vec3;
using std::cout;
using std::endl;


struct TexturedColoredVertex;
struct ColoredVertex;

//Functions defined in Source1.cpp

int* createVertexArrayObject(ColoredVertex vertexArray[], int vertexCount);
int* createTextureVertexArrayObject(TexturedColoredVertex vertexArray[], int vertexCount);
inline void drawCube(char* type) { glDrawArrays(GL_TRIANGLES, 0, 36); }//Workaround to allow different function signature
inline void drawCube() { glDrawArrays(GL_TRIANGLES, 0, 36); }
void drawCoordinate(int vaoX, int vaoY, int vaoZ, glm::mat4 groupMatrixW, GLuint worldMatrixLocation);
inline float radians(float a) { return a * (3.14159f / 180.0f); }
void setProjectionMatrix(int shaderProgram, glm::mat4 projectionMatrix);
void setViewMatrix(int shaderProgram, glm::mat4 viewMatrix);
void setWorldMatrix(int shaderProgram, glm::mat4 worldMatrix);
void drawGridTiles(int vaoTile, int tileTextureID, glm::mat4 groupMatrixW, GLuint worldMatrixLocation);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
	glUseProgram(shader_id);
	glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
	glUseProgram(0);
}

class Car {
public:
	Car();
	~Car();
	void draw(GLuint worldMatrixLocation, mat4 groupMatrixW);
	void calculate(float dt = 0.1f);

	//The following should be private, but are not for now
	float Px, Py, Pz;
	float Lx, Ly, Lz;
	float V, w;
	GLuint vaoCube;
	GLuint Textures[10];
	float theta, theta_t;
};