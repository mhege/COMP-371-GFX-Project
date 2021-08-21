#pragma once

#include <vector>
#include <iostream>
#include<list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices

class Projectile;

class LDModel {


private:

	float xP, yP, zP;
	float camera1X, camera1Y, camera1Z;
	float camera2X, camera2Y, camera2Z;
	int currentCamera;
	float angleFacing;
	GLuint letterTID;
	GLuint digitTID;
	int textureLvl;
	float spinning;
	std::list<Projectile>* projectileList;
	glm::vec3 vect;
	glm::vec3 cameraLookAt;
	glm::vec3 cameraLookAt2;

public:

	LDModel();
	LDModel(float xP, float yP, float zP, GLuint letterID, GLuint digitID, int texureLvl);
	~LDModel() { delete projectileList; };
	void setCurrentCamera(int num) { currentCamera = num; };
	void setAngleFacing(float angle) { angleFacing = angle; };
	float getX() { return xP; };
	float getY() { return yP; };
	float getZ() { return zP; };
	float getAngleFacing() { return angleFacing; };
	int getCurrentCamera() { return currentCamera; };
	glm::mat4 getViewMatrix();

	void moveRight(float x);
	void moveLeft(float x);
	void moveForward(float z);
	void moveBack(float z);
	bool checkBoundary(float z);
	void draw(int model, GLuint vao, GLuint worldMatrixLocation, glm::mat4 groupMatrixW);
	void update();
	glm::vec3 getCameraPos();
	glm::vec3 getCameraView();
	void addGun(GLuint vaoSphere, GLuint vao, glm::mat4 groupMatrixW, GLuint program1, GLuint program2, float dt);
	void shoot(GLuint shaderProgram);
};


class Projectile {

private:
	GLuint mWorldMatrixLocation;
	glm::vec3 mPosition;
	glm::vec3 mVelocity;

public:
	Projectile(glm::vec3 position, glm::vec3 velocity, int shaderProgram);
	void Update(float dt);
	void Draw(GLuint vao);

};