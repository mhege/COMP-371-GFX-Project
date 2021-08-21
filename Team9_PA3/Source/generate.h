#pragma once
#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include <stdlib.h>



struct TexturedColoredVertex;
struct ColoredVertex;

class Generate {

private:
	ColoredVertex* sphereVector;
public:
	Generate();
	int compileAndLinkShaders(std::string vertexPath, std::string fragmentPath);
	int* createVertexArrayObject(ColoredVertex vertexArray[], int vertexCount);
	int* createTextureVertexArrayObject(TexturedColoredVertex vertexArray[], int vertexCount);
	GLuint loadTexture(const char* filename);
	ColoredVertex* genSphere();
	ColoredVertex* genCone();
	~Generate();
};

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
