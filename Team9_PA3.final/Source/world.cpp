#include "World.h"



World::World(){

	rotAXW = 0.0f;
	rotAYW = 0.0f;
	groupMatrixW=NULL;
	projectionMatrix = NULL;
	viewMatrix = NULL;
	mapSize = 0;
	shaderProgram = 0;
	texturedShaderProgram = 0;
	texturedShaderProgramShadow = 0;
	skyShaderProgram = 0;
	shaderShadow = 0;
	shaderShadow2 = 0;
	shaderShadow3 = 0;
	shaderShadow4 = 0;
	shaderShadow5 = 0;
	shaderShadow6 = 0;
	cameraPos = NULL;
	light1 = NULL;
	light2 = NULL;
	light3 = NULL;
	light4 = NULL;
	light5 = NULL;
	light6 = NULL;
	OnOff = 0;
	shadowOn = 1;
	day = false;
};

World::World(GLuint shaderProgram, GLuint texturedShaderProgram, GLuint texturedShaderProgramShadow,GLuint skyShaderProgram, GLuint shaderShadow, GLuint shaderShadow2, GLuint shaderShadow3, GLuint shaderShadow4, GLuint shaderShadow5, GLuint shaderShadow6) {

	glm::vec3 cameraLookAt(0.0f, -12.0f, -50.0f);
	glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
	cameraPos = new glm::vec3(0.0f, 6.0f, 25.0f);

	projectionMatrix = new glm::mat4 (glm::perspective(glm::radians(60.0f), 
		1024.0f / 768.0f,     
		1.0f, 5000.0f));       

	viewMatrix = new glm::mat4(glm::lookAt((*cameraPos),  // eye (camera location)
		(*cameraPos) + cameraLookAt,  // center (Where is the camera looking)
		cameraUp));

	mapSize = 150;
	this->shaderProgram = shaderProgram;
	this->texturedShaderProgram = texturedShaderProgram;
	this->texturedShaderProgramShadow = texturedShaderProgramShadow;
	this->skyShaderProgram = skyShaderProgram;
	this->shaderShadow = shaderShadow;
	this->shaderShadow2 = shaderShadow2;
	this->shaderShadow3 = shaderShadow3;
	this->shaderShadow4 = shaderShadow4;
	this->shaderShadow5 = shaderShadow5;
	this->shaderShadow6 = shaderShadow6;
	light1 = new Light(glm::vec3(0.0f, 400.0f, 50.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	light2 = new Light(glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	light3 = new Light(glm::vec3(0.0f, 25.0f, -90.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	light4 = new Light(glm::vec3(0.0f, 25.0f, 90.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	light5 = new Light(glm::vec3(-90.0f, 25.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	light6 = new Light(glm::vec3(90.0f, 25.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), *cameraPos);
	rotAXW = 0.0f;
	rotAYW = 0.0f;
	glm::mat4 roGroupXW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXW), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYW), glm::vec3(0.0f, 1.0f, 0.0f));
	groupMatrixW = new glm::mat4(roGroupXW*roGroupYW);
	shadowOn = 1;
	day = true;
};

World::~World() {

	delete viewMatrix;
	delete projectionMatrix;
	delete cameraPos;
	
};

void World::setLightSwitch(int onoff) {
	OnOff = onoff;
}

void World::setView(glm::mat4 view) {

	*viewMatrix =  view;

};

void World::setCameraPos(float x, float y, float z) {
  
	cameraPos->x = x;
	cameraPos->y = y;
	cameraPos->z = z;

};

glm::vec3* World::getCameraPosition() {
  
	glm::vec3* pos = new glm::vec3(*cameraPos);
	return pos;

};

glm::mat4* World::getViewMatrix() {

	glm::mat4* view = new glm::mat4(*viewMatrix);
	return view;

};

glm::mat4* World::getProjectionMatrix() {

	glm::mat4* proj = new glm::mat4(*projectionMatrix);
	return proj;

};

void World::setRotAXW(float x) {

	rotAXW = x;
};

void World::setRotAYW(float y) {
   
	rotAYW = y;

};


void World::drawGround(GLuint vao, GLuint vaoCube, GLuint roadTex, GLuint grassTex, GLuint brickTex, GLuint road2Tex) {

	GLuint worldMatrixLocation;

	
	glUseProgram(texturedShaderProgramShadow);
	worldMatrixLocation = glGetUniformLocation(texturedShaderProgramShadow, "worldMatrix");
	

	glm::mat4 transTemp;
	glm::mat4 worldMatrix;

	int vertexScale = 10; // Corresponds to position scale factor in texturedTileVertexArray

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);


	for (float i = -mapSize; i <= mapSize; i = i + vertexScale) {

		for (float j = -mapSize; j <= mapSize; j = j + vertexScale) {

			if ((j >= -15 && j <= 15) || (i >= -15 && i <= 15) || (i >= 80 && i <= 100) || (i >= -100 && i <= -80))
				glBindTexture(GL_TEXTURE_2D, roadTex);
			else
				glBindTexture(GL_TEXTURE_2D, grassTex);

			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, 0.0f, i));
			worldMatrix = (*groupMatrixW) * transTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


	}

	glBindVertexArray(vaoCube);

	//sidewalk
	glBindTexture(GL_TEXTURE_2D, brickTex);

	for (float i = -mapSize; i <= mapSize; i = i + vertexScale) {


		if (!(i >= -15 && i <= 15) && !(i >= -100 && i <= -80) && !(i >= 80 && i <= 100)) {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(20.0, 0.25f, i));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		if (!(i >= -15 && i <= 15) && !(i >= -100 && i <= -80) && !(i >= 80 && i <= 100)) {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0, 0.25f, i));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		if (!(i >= -15 && i <= 15)) {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, 20.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		if (!(i >= -15 && i <= 15)) {
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, -20.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		if (!(i >= -15 && i <= 15)) {

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale / 2));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, -75.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);


			scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale / 2));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, -105.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale / 2));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, 75.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);


			scale = glm::scale(glm::mat4(1.0f), glm::vec3(vertexScale, 0.5f, vertexScale / 2));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.25f, 105.0));
			worldMatrix = (*groupMatrixW) * transTemp * scale;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
	}

	glBindVertexArray(0);


}


void World::drawSky(GLuint vao, GLuint skyTexDay, GLuint sky2TexDay, GLuint sky1TexNight, GLuint sky2TexNight) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;

	glUseProgram(skyShaderProgram);
	GLuint worldMatrixLocation = glGetUniformLocation(skyShaderProgram, "worldMatrix");

	
	glActiveTexture(GL_TEXTURE1);

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, sky2TexNight);
		setLightSwitch(0);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, skyTexDay);
		setLightSwitch(1);
	}

	glBindVertexArray(vao);
	
	
	for (float i = -mapSize; i <= mapSize; i = i + 50.0) {

		for (float j = -mapSize; j <= mapSize; j = j + 50.0) {

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i, -mapSize));
			worldMatrix = (*groupMatrixW) * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i, mapSize));
			worldMatrix = (*groupMatrixW) * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);


			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 50.0f, 50.0f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(mapSize, i, j));
			worldMatrix = (*groupMatrixW) * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 50.0f, 50.0f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-mapSize, i, j));
			worldMatrix = (*groupMatrixW) * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 0.1f, 50.0f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, mapSize, i));
			worldMatrix = (*groupMatrixW) * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

	}



	glBindVertexArray(0);

};

void World::drawBillBoard(float x, float z, float angle, GLuint vao, GLuint tex1ID, GLuint tex2ID, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));;

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE1);

	glBindTexture(GL_TEXTURE_2D, tex2ID);

	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 5.0f, 0.5f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0, 2.5f, 0.0f));
	worldMatrix = (*groupMatrixW) * trans * roTemp * transTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 5.0f, 0.5f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0, 2.5f, 0.0f));
	worldMatrix = (*groupMatrixW) * trans * roTemp * transTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 10.0f, 0.5f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 10.0f, 0.0f));
	worldMatrix = (*groupMatrixW) * trans * roTemp * transTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, tex1ID);

	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(19.0f, 9.0f, 0.15f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 10.25f, 0.5));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	worldMatrix = (*groupMatrixW) * trans * roTemp * transTemp * rot * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);


};

void World::placeBillBoards(GLuint vao, GLuint tex1ID, GLuint tex2ID, GLuint tex3ID , GLuint tex4ID,std::vector<int> vect, GLuint worldMatrixLocation) {

	GLuint arr[3] = { tex1ID,tex3ID,tex4ID };

	int val = vect.back();
    drawBillBoard(-30.0, 40.0, 90.0f, vao, arr[val], tex2ID, worldMatrixLocation);
	vect.pop_back();
	
	val = vect.back();
	drawBillBoard(-35.0, -35.0, 30.0f, vao, arr[val], tex2ID, worldMatrixLocation);
	vect.pop_back();

	val = vect.back();
	drawBillBoard(35.0, 60.0, -30.0f, vao, arr[val], tex2ID, worldMatrixLocation);
	vect.pop_back();
};

void World::drawBuildingT1(GLuint vao, int value, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5, GLuint cement, GLuint metal, float x, float z,GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));;



	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(vao);

	if (value == 1) {

		for (float i = 0; i <= 80; i = i + 5.0) {

			for (float j = -10; j <= 10; j = j + 5.0) {


				if (i == 0.0 && j == 0.0) {

					glBindTexture(GL_TEXTURE_2D, metal);

				}
				else if (i <= 5.0) {
					glBindTexture(GL_TEXTURE_2D, cement);
				}
				else {

					glBindTexture(GL_TEXTURE_2D, tex4);
				}
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, 12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, -12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);


				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

			}
		}
		glBindTexture(GL_TEXTURE_2D, cement);
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.25f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 80.0 + 2.5, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if (value == 2) {
	    
	
		for (float i = 0; i <= 80; i = i + 5.0) {

			for (float j = -10; j <= 10; j = j + 5.0) {

				if (i == 0.0 && j == 0.0) {

					glBindTexture(GL_TEXTURE_2D, metal);
				}
				else if (i <= 5.0) {

					glBindTexture(GL_TEXTURE_2D, tex2);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, tex1);
				}
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, 12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, -12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);


				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

			}
		}
		glBindTexture(GL_TEXTURE_2D, cement);
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.25f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 80.0 + 2.5, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else if (value == 3) {
		
		int n = 0;

		for (float i = 0; i <= 80; i = i + 5.0) {

			for (float j = -10; j <= 10; j = j + 5.0) {
				if (i == 0) {
					glBindTexture(GL_TEXTURE_2D, cement);
				}
				else if (n % 2 == 0) {

					glBindTexture(GL_TEXTURE_2D, tex3);
				}
				else {

					glBindTexture(GL_TEXTURE_2D, tex4);
				}

				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, 12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, -12.5));
				worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);


				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
				roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
				transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5, j));
				worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
				glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

				glDrawArrays(GL_TRIANGLES, 0, 36);

			}
			n += 1;
		}
		glBindTexture(GL_TEXTURE_2D, cement);
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.25f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 80.0 + 2.5, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);


	}

	else if (value == 4) {

	int n = 0;

	for (float i = 0; i <= 40; i = i + 5.0) {

		for (float j = -10; j <= 10; j = j + 5.0) {
			if (j==0) {

				glBindTexture(GL_TEXTURE_2D, metal);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, tex2);
			}

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, 12.5));
			worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, -12.5));
			worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);


			roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5, j));
			worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5, j));
			worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		n += 1;
	}
	glBindTexture(GL_TEXTURE_2D, cement);
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.25f, 25.0f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 40.0 + 2.5, 0.0f));
	worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);


	}

	else if (value == 5) {

	int n = 0;

	for (float i = 0; i <= 80; i = i + 5.0) {

		for (float j = -10; j <= 10; j = j + 5.0) {
			
			if (i == 0) {

				glBindTexture(GL_TEXTURE_2D, cement);
			}
			else if ((j == 10 || j==-10)&& i!=0) {

				glBindTexture(GL_TEXTURE_2D, tex3);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, tex4);
			}

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, 12.5));
			worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5, -12.5));
			worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);


			roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5, j));
			worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5, j));
			worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		n += 1;
	}
	glBindTexture(GL_TEXTURE_2D, cement);
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.25f, 25.0f));
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 80.0 + 2.5, 0.0f));
	worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);


	}


	glBindVertexArray(0);


};



void World::drawBuildingT2(GLuint vao, int value, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5,GLuint cement,GLuint metal ,float x, float z, GLuint worldMatrixLocation) {

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));;


	glActiveTexture(GL_TEXTURE1);
	glBindVertexArray(vao);

	if (value == 1) {


	    glBindTexture(GL_TEXTURE_2D, cement);

		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 5.0f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
		worldMatrix = (*groupMatrixW) * trans* transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		

		glBindTexture(GL_TEXTURE_2D, tex3);
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 5.0f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f+5.0, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);


		glBindTexture(GL_TEXTURE_2D, cement);

		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 5.0f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f+10.0, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		
		for (float i = 0; i <= 80; i = i + 5.0) {

				for (float j = -10; j <= 10; j = j + 5.0) {

					glBindTexture(GL_TEXTURE_2D, tex5);

					scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
					transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5 + 15.0, 12.5));
					worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
					glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

					glDrawArrays(GL_TRIANGLES, 0, 36);

					scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
					transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(j, i + 2.5+15.0, -12.5));
					worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
					glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

					glDrawArrays(GL_TRIANGLES, 0, 36);


					roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
					transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(12.5, i + 2.5+15.0, j));
					worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
					glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

					glDrawArrays(GL_TRIANGLES, 0, 36);
					roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.1f));
					transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5, i + 2.5+15.0, j));
					worldMatrix = (*groupMatrixW) * trans * transTemp * roTemp * scaleTemp;
					glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

					glDrawArrays(GL_TRIANGLES, 0, 36);




				}
		}

		glBindTexture(GL_TEXTURE_2D, cement);

		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 0.5f, 25.0f));
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 80.0 + 2.5 + 15.0, 0.0f));
		worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);

	}
	else if (value == 2) {

	int j = 0;
	    for (float i = 0.0; i <= 50.0; i += 5.0) {

			if (j % 2 == 0) {

				glBindTexture(GL_TEXTURE_2D, cement);
			}
			else {

		    	glBindTexture(GL_TEXTURE_2D, tex5);
			}

			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f, 5.0f, 25.0f));
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i+2.5f, 0.0f));
			worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			j+=1;

	    }
	   
    }
	else if (value == 3) {

	  int j = 0;
	     
	 
	  glBindTexture(GL_TEXTURE_2D, cement);

	  scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f, 5.0f, 25.0f));
	  transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
	  worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
	  glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	  glDrawArrays(GL_TRIANGLES, 0, 36);

	  glBindTexture(GL_TEXTURE_2D, tex5);

	  scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f, 5.0f, 25.0f));
	  transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f+5, 0.0f));
	  worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
	  glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	  glDrawArrays(GL_TRIANGLES, 0, 36);


	  glBindTexture(GL_TEXTURE_2D, cement);

	  scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f, 5.0f, 25.0f));
	  transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f+10, 0.0f));
	  worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
	  glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	  glDrawArrays(GL_TRIANGLES, 0, 36);
	  
	  
	  for (float i = 0.0; i <= 50.0; i += 5.0) {
			
		    if (j % 2 == 0) {
			glBindTexture(GL_TEXTURE_2D, tex2);
		    }
		    else {

			glBindTexture(GL_TEXTURE_2D, tex4);
		    }

		    scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(60.0f, 5.0f, 25.0f));
		    transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i + 2.5f+15.0, 0.0f));
		    worldMatrix = (*groupMatrixW) * trans * transTemp * scaleTemp;
		    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

		    glDrawArrays(GL_TRIANGLES, 0, 36);

		    j += 1;

	     }

	}
    
	
	glBindVertexArray(0);


};

void World::placeBuildings(GLuint vao, GLuint tex1, GLuint tex2, GLuint tex3, GLuint tex4, GLuint tex5, GLuint cement, GLuint metal,std::vector<int> vect, std::vector<int> vect2, GLuint worldMatrixLocation) {

	//placing type 1 buildings

	int val = vect.back();
	drawBuildingT1(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, 45.0, -45.0,worldMatrixLocation);
	vect.pop_back();

	val = vect.back();
	drawBuildingT1(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, -80.0, -130.0,worldMatrixLocation);
	vect.pop_back();

	val = vect.back();
	drawBuildingT1(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, -110.0, 50.0, worldMatrixLocation);
	vect.pop_back();

	val = vect.back();
	drawBuildingT1(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, -50.0, 50.0, worldMatrixLocation);
	vect.pop_back();

	val = vect.back();
	drawBuildingT1(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, 80.0, 130.0, worldMatrixLocation);
	vect.pop_back();

	//place type 2 buildings

	val = vect2.back();
	drawBuildingT2(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, 110.0, -45.0, worldMatrixLocation);
	vect2.pop_back();

	val = vect2.back();
	drawBuildingT2(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, -100.0, -45.0, worldMatrixLocation);
	vect2.pop_back();

	val = vect2.back();
	drawBuildingT2(vao, val, tex1, tex2, tex3, tex4, tex5, cement, metal, 100.0, 45.0, worldMatrixLocation);
	vect2.pop_back();


};



void World::drawPineTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition) {
	
	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), randPosition);

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 groupMatrix;

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bark);

	//bark

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 15.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//leaves
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, leaves);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 3.0f, 20.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 9.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(18.0f, 3.0f, 18.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 11.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(16.0f, 3.0f, 16.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 13.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(14.0f, 3.0f, 14.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(12.0f, 3.0f, 12.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 17.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 19.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 3.0f, 8.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 3.0f, 6.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 23.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 3.0f, 4.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 27.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

};

void World::drawCactusTree(GLuint vao, GLuint bark, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition) {
	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), randPosition);

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 groupMatrix;

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bark);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 15.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 9.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 4.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 8.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 4.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 11.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 4.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 12.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 4.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
};

void World::drawSavannahTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition) {
	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), randPosition);

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 groupMatrix;

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bark);

	//bark

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 5.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 6.00f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 5.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 8.40f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 5.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, 10.80f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 5.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.4f, 13.2f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 5.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.5f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 7.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.5f, 18.0f, 1.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 7.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *(transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.75f, 9.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-7.5f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 10.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(10.f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 10.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 10.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//leaves
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, leaves);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 22.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-9.0f, 22.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 4.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.50f, 24.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(19.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.50f, 26.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(14.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.50f, 28.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 3.0f, 5.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
};

void World::drawMapleTree(GLuint vao, GLuint bark, GLuint leaves, GLuint worldMatrixLocation, glm::mat4 groupMatrixW, glm::vec3 randPosition) {

	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), randPosition);

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 groupMatrix;

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bark);

	//bark

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.5f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 15.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 5.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 8.0f, 1.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//leaves
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, leaves);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 3.0f, 5.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 3.0f, 5.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 12.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 3.0f, 8.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 12.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 3.0f, 8.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 12.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(8.0f, 3.0f, 8.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(11.0f, 3.0f, 11.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(11.0f, 3.0f, 11.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(11.0f, 3.0f, 11.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(14.0f, 3.0f, 14.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(14.0f, 3.0f, 14.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 18.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(14.0f, 3.0f, 14.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(12.0f, 3.0f, 12.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(12.0f, 3.0f, 12.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(12.0f, 3.0f, 12.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 23.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 23.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 23.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 3.0f, 10.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 3.0f, 6.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 3.0f, 6.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(6.0f, 3.0f, 6.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 27.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 27.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 27.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 7.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 3.0f, 5.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 8.0f, 0.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

};

void World::drawPole(GLuint vao, GLuint tex, GLuint worldMatrixLocation, glm::mat4 groupMatrixW) {
	
	
	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 roTemp2;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 groupMatrix;

	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	//middle
	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f+90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW) * baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//left

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f-90.0f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f - 90.0f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f - 90.0f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f - 90.0f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f - 90.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f - 90.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f - 90.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f - 90.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//right

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f + 90.0f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f + 90.0f, 4.75f, 15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f + 90.0f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f + 90.0f, 4.75f, -15.75f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f + 90.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f + 90.0f, 17.0f, 8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f + 90.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f + 90.0f, 17.0f, -8.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//back

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, 15.75f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, 15.75f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, -15.75f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, -15.75f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, 8.0f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, 8.0f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, -8.0f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, -8.0f - 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//forward 

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, 15.75f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, 15.75f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-15.75f, 4.75f, -15.75f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(15.75f, 4.75f, -15.75f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 9.5f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, 8.0f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)*baseTrans   *  (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, 8.0f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 17.0f, -8.0f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 17.0f, -8.0f + 90.0f));
	roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	roTemp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, 27.0f, .5f));
	worldMatrix = (groupMatrixW)* baseTrans * (transTemp*roTemp2*roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

};


void World::setUpWorld() {


	//set view
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &(*viewMatrix)[0][0]);

	glUseProgram(texturedShaderProgram);
	viewMatrixLocation = glGetUniformLocation(texturedShaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &(*viewMatrix)[0][0]);

	glUseProgram(texturedShaderProgramShadow);
	viewMatrixLocation = glGetUniformLocation(texturedShaderProgramShadow, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &(*viewMatrix)[0][0]);

	glUseProgram(skyShaderProgram);
	viewMatrixLocation = glGetUniformLocation(skyShaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &(*viewMatrix)[0][0]);

	//set Projection
	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &(*projectionMatrix)[0][0]);

	glUseProgram(texturedShaderProgram);
	projectionMatrixLocation = glGetUniformLocation(texturedShaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &(*projectionMatrix)[0][0]);

	glUseProgram(texturedShaderProgramShadow);
	projectionMatrixLocation = glGetUniformLocation(texturedShaderProgramShadow, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &(*projectionMatrix)[0][0]);

	glUseProgram(skyShaderProgram);
	projectionMatrixLocation = glGetUniformLocation(skyShaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &(*projectionMatrix)[0][0]);

	
	//Untextured shadow
	glUseProgram(shaderProgram);

	GLint lightColorLocUT = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightColorLocUT, 1.0f, 1.0f, 1.0f);

	SetUniformVec3(shaderProgram, "lightPos1", light1->lightPos);
	SetUniformVec3(shaderProgram, "lightPos2", light2->lightPos);
	SetUniformVec3(shaderProgram, "lightPos3", light3->lightPos);
	SetUniformVec3(shaderProgram, "lightPos4", light4->lightPos);
	SetUniformVec3(shaderProgram, "lightPos5", light5->lightPos);
	SetUniformVec3(shaderProgram, "lightPos6", light6->lightPos);

	GLint viewPosLocUT = glGetUniformLocation(shaderProgram, "viewPos1");
	glUniform3f(viewPosLocUT, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLint viewPosLocUT2 = glGetUniformLocation(shaderProgram, "viewPos2");
	glUniform3f(viewPosLocUT2, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLint viewPosLocUT3 = glGetUniformLocation(shaderProgram, "viewPos3");
	glUniform3f(viewPosLocUT3, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLint viewPosLocUT4 = glGetUniformLocation(shaderProgram, "viewPos4");
	glUniform3f(viewPosLocUT4, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLint viewPosLocUT5 = glGetUniformLocation(shaderProgram, "viewPos5");
	glUniform3f(viewPosLocUT5, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLint viewPosLocUT6 = glGetUniformLocation(shaderProgram, "viewPos6");
	glUniform3f(viewPosLocUT6, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);

	GLint lightSpotdirLocUT = glGetUniformLocation(shaderProgram, "light_cutoff");
	glUniform1f(lightSpotdirLocUT, glm::cos(glm::radians(35.0f)));

	GLint lightOff = glGetUniformLocation(shaderProgram, "lightOff");
	glUniform1i(lightOff, OnOff);

	//textured shadow
	glUseProgram(texturedShaderProgramShadow);

	GLuint lightColorLocT = glGetUniformLocation(texturedShaderProgramShadow, "lightColor");
	glUniform3f(lightColorLocT, 1.0f, 1.0f, 1.0f);

	SetUniformVec3(texturedShaderProgramShadow, "lightPos1", light1->lightPos);
	SetUniformVec3(texturedShaderProgramShadow, "lightPos2", light2->lightPos);
	SetUniformVec3(texturedShaderProgramShadow, "lightPos3", light3->lightPos);
	SetUniformVec3(texturedShaderProgramShadow, "lightPos4", light4->lightPos);
	SetUniformVec3(texturedShaderProgramShadow, "lightPos5", light5->lightPos);
	SetUniformVec3(texturedShaderProgramShadow, "lightPos6", light6->lightPos);

	GLuint viewPosLocT = glGetUniformLocation(texturedShaderProgramShadow, "viewPos1");
	glUniform3f(viewPosLocT, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLuint viewPosLocT2 = glGetUniformLocation(texturedShaderProgramShadow, "viewPos2");
	glUniform3f(viewPosLocT2, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLuint viewPosLocT3 = glGetUniformLocation(texturedShaderProgramShadow, "viewPos3");
	glUniform3f(viewPosLocT3, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLuint viewPosLocT4 = glGetUniformLocation(texturedShaderProgramShadow, "viewPos4");
	glUniform3f(viewPosLocT4, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLuint viewPosLocT5 = glGetUniformLocation(texturedShaderProgramShadow, "viewPos5");
	glUniform3f(viewPosLocT5, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);
	GLuint viewPosLocT6 = glGetUniformLocation(texturedShaderProgramShadow, "viewPos6");
	glUniform3f(viewPosLocT6, (*getCameraPosition()).x, (*getCameraPosition()).y, (*getCameraPosition()).z);

	GLuint lightSpotdirLocT = glGetUniformLocation(texturedShaderProgramShadow, "light_cutoff");
	glUniform1f(lightSpotdirLocT, glm::cos(glm::radians(35.0f)));

	lightOff = glGetUniformLocation(texturedShaderProgramShadow, "lightOff");
	glUniform1i(lightOff, OnOff);

	//update worldOrientation
	glm::mat4 roGroupXW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAXW), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 roGroupYW = glm::rotate(glm::mat4(1.0f), glm::radians(rotAYW), glm::vec3(0.0f, 1.0f, 0.0f));
	*groupMatrixW = roGroupXW * roGroupYW;

	// set up lihgt + shadow

	glm::vec3 lightPosition1 = light1->lightPos;
	glm::vec3 lightPosition2 = light2->lightPos;
	glm::vec3 lightPosition3 = light3->lightPos;
	glm::vec3 lightPosition4 = light4->lightPos;
	glm::vec3 lightPosition5 = light5->lightPos;
	glm::vec3 lightPosition6 = light6->lightPos;

	glm::vec3 lightFocus1(0.0f, 0.0f, 0.0f);
	glm::vec3 lightFocus2(0.0f, 0.0f, 0.0f);
	glm::vec3 lightFocus3(0.0f, 0.0f, -90.0f);
	glm::vec3 lightFocus4(0.0f, 0.0f, 90.0f);
	glm::vec3 lightFocus5(-90.0f, 0.0f, 0.0f);
	glm::vec3 lightFocus6(90.0f, 0.0f, 0.0f);


	glm::vec3 lightDirection1 = normalize(lightFocus1 - lightPosition1);
	glm::vec3 lightDirection2 = normalize(lightFocus2 - lightPosition2);
	glm::vec3 lightDirection3 = normalize(lightFocus3 - lightPosition3);
	glm::vec3 lightDirection4 = normalize(lightFocus4 - lightPosition4);
	glm::vec3 lightDirection5 = normalize(lightFocus5 - lightPosition5);
	glm::vec3 lightDirection6 = normalize(lightFocus6 - lightPosition6);

	float lightNearPlane = 1.0f, lightFarPlane = 800.0f;

	glm::mat4 lightProjectionMatrix = glm::frustum(-2.0f, 2.0f, -2.5f, 1.5f, lightNearPlane, lightFarPlane);

	glm::mat4 lightViewMatrix1 = glm::lookAt(lightPosition1, lightFocus1, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix1 = lightProjectionMatrix * lightViewMatrix1;

	glm::mat4 lightViewMatrix2 = glm::lookAt(lightPosition2, lightFocus2, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix2 = lightProjectionMatrix * lightViewMatrix2;

	glm::mat4 lightViewMatrix3 = glm::lookAt(lightPosition3, lightFocus3, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix3 = lightProjectionMatrix * lightViewMatrix3;

	glm::mat4 lightViewMatrix4 = glm::lookAt(lightPosition4, lightFocus4, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix4 = lightProjectionMatrix * lightViewMatrix4;

	glm::mat4 lightViewMatrix5 = glm::lookAt(lightPosition5, lightFocus5, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix5 = lightProjectionMatrix * lightViewMatrix5;

	glm::mat4 lightViewMatrix6 = glm::lookAt(lightPosition6, lightFocus6, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix6 = lightProjectionMatrix * lightViewMatrix6;

	// Set light space matrix 
	SetUniformMat4(shaderShadow, "light_view_proj_matrix1", lightSpaceMatrix1);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix1", lightSpaceMatrix1);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix1", lightSpaceMatrix1);

	// Set light space matrix 
	SetUniformMat4(shaderShadow2, "light_view_proj_matrix2", lightSpaceMatrix2);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix2", lightSpaceMatrix2);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix2", lightSpaceMatrix2);

	// Set light space matrix 
	SetUniformMat4(shaderShadow3, "light_view_proj_matrix3", lightSpaceMatrix3);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix3", lightSpaceMatrix3);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix3", lightSpaceMatrix3);

	// Set light space matrix 
	SetUniformMat4(shaderShadow4, "light_view_proj_matrix4", lightSpaceMatrix4);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix4", lightSpaceMatrix4);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix4", lightSpaceMatrix4);

	// Set light space matrix 
	SetUniformMat4(shaderShadow5, "light_view_proj_matrix5", lightSpaceMatrix5);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix5", lightSpaceMatrix5);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix5", lightSpaceMatrix5);

	// Set light space matrix 
	SetUniformMat4(shaderShadow6, "light_view_proj_matrix6", lightSpaceMatrix6);
	SetUniformMat4(shaderProgram, "light_view_proj_matrix6", lightSpaceMatrix6);
	SetUniformMat4(texturedShaderProgramShadow, "light_view_proj_matrix6", lightSpaceMatrix6);

	// Set light far and near planes on scene shader
	
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "light_near_plane"), lightNearPlane);
	glUniform1i(glGetUniformLocation(shaderProgram, "light_far_plane"), lightFarPlane);

	glUseProgram(texturedShaderProgramShadow);
	glUniform1i(glGetUniformLocation(texturedShaderProgramShadow, "light_near_plane"), lightNearPlane);
	glUniform1i(glGetUniformLocation(texturedShaderProgramShadow, "light_far_plane"), lightFarPlane);

	// Set light direction on scene shader
	SetUniformVec3(shaderProgram, "light_direction1", lightDirection1);
	SetUniformVec3(shaderProgram, "light_direction2", lightDirection2);
	SetUniformVec3(shaderProgram, "light_direction3", lightDirection3);
	SetUniformVec3(shaderProgram, "light_direction4", lightDirection4);
	SetUniformVec3(shaderProgram, "light_direction5", lightDirection5);
	SetUniformVec3(shaderProgram, "light_direction6", lightDirection6);

	// Set light direction on scene shader
	SetUniformVec3(texturedShaderProgramShadow, "light_direction1", lightDirection1);
	SetUniformVec3(texturedShaderProgramShadow, "light_direction2", lightDirection2);
	SetUniformVec3(texturedShaderProgramShadow, "light_direction3", lightDirection3);
	SetUniformVec3(texturedShaderProgramShadow, "light_direction4", lightDirection4);
	SetUniformVec3(texturedShaderProgramShadow, "light_direction5", lightDirection5);
	SetUniformVec3(texturedShaderProgramShadow, "light_direction6", lightDirection6);

};


void World::SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value)
{
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, glm::value_ptr(uniform_value));
}

void World::SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value)
{
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

