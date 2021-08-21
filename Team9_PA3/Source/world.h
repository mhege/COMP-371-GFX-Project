#pragma once
#include"letterDigitPair.h"
#include"generate.h"
#include <algorithm>

struct Light;

class World {

private:

	int mapSize;
	float rotAXW;
	float rotAYW;
	glm::mat4* groupMatrixW;
	glm::mat4* projectionMatrix;
	glm::mat4* viewMatrix;
	glm::vec3* cameraPos;
	GLuint shaderProgram;
	GLuint texturedShaderProgram;
	GLuint texturedShaderProgramShadow;
	GLuint skyShaderProgram;
	GLuint shaderShadow;
	GLuint shaderShadow2;
	GLuint shaderShadow3;
	GLuint shaderShadow4;
	GLuint shaderShadow5;
	GLuint shaderShadow6;
	Light* light1;
	Light* light2;
	Light* light3;
	Light* light4;
	Light* light5;
	Light* light6;
	int OnOff;
	int shadowOn;
	bool day;

public:

	World();
	World(GLuint shaderProgram, GLuint texturedShaderProgram, GLuint texturedShaderProgramShadow, GLuint skyShaderProgram, GLuint shaderShadow, GLuint shaderShadow2, GLuint shaderShadow3, GLuint shaderShadow4, GLuint shaderShadow5, GLuint shaderShadow6);
	~World();
	void setView(glm::mat4 viewMatrix);
	void setUpWorld();
	void setCameraPos(float x, float y, float z);
	glm::vec3* getCameraPosition();
	glm::mat4* getViewMatrix();
	glm::mat4* getProjectionMatrix();
	void setLightSwitch(int onoff);
	void setRotAXW(float x);
	void setRotAYW(float y);
	float getRotAXW() { return rotAXW; };
	float getRotAYW() { return rotAYW; };
	glm::mat4* getgroupMatrixW(){return groupMatrixW;};
	void setShadowOn(int v) { shadowOn = v; };
	void setDay(bool v) { day = v; };
	bool getDay() { return day; };
	void SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value);
	void SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value);
	void drawGround(GLuint vao, GLuint vaoCube, GLuint roadTex, GLuint grassTex, GLuint brickTex, GLuint groundTex);
	void drawSky(GLuint vao, GLuint skyTexDay, GLuint sky2TexDay, GLuint sky1TexNight, GLuint sky2TexNight);
	void drawPineTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition);
	void drawCactusTree(GLuint vao, GLuint bark, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition);
	void drawSavannahTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition);
	void drawMapleTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition);
	void drawPole(GLuint vao, GLuint tex, GLuint worldMatrixLocation, glm::mat4 groupMatrixW);
	void drawBillBoard(float x, float z, float angle, GLuint vao, GLuint tex1ID, GLuint tex2ID,GLuint worldMatrixLocation);
	void placeBillBoards(GLuint vao, GLuint tex1ID, GLuint tex2ID, GLuint tex3ID, GLuint tex4ID,std::vector<int> vect, GLuint worldMatrixLocation);
	void drawBuildingT1(GLuint vao, int value, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5, GLuint cement, GLuint metal,float x, float z, GLuint worldMatrixLocation);
	void drawBuildingT2(GLuint vao, int value, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5, GLuint cement, GLuint metal, float x, float z, GLuint worldMatrixLocation);
	void placeBuildings(GLuint vao, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5, GLuint cement, GLuint metal,std::vector<int> vect, std::vector<int> vect2, GLuint worldMatrixLocation);
};

struct Light {

	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::vec3 viewPos;

	Light(glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos) {

		this->lightPos = lightPos;
		this->lightColor = lightColor;
		this->viewPos = viewPos;

	};

};



