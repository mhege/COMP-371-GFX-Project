#include"letterDigitPair.h"



LDModel::LDModel() {

	xP = 0.0;
	yP = 0.0;
	zP = 0.0;
	camera1X = 0.0;
	camera1Y = 0.0;
	camera1Z = 0.0;
	camera2X = 0.0;
	camera2Y = 0.0;
	camera2Z = 0.0;
	currentCamera = 1;
	angleFacing = 0.0;
	letterTID = 0;
	digitTID = 0;
	textureLvl = 0;
	spinning = 0.0f;
	projectileList = NULL;
	vect = glm::vec3();
	cameraLookAt = glm::vec3();
	cameraLookAt2 = glm::vec3();
}

LDModel::LDModel(float xP, float yP, float zP, GLuint letterID, GLuint digitID, int textureLvl) {

	this->xP = xP;
	this->yP = yP;
	this->zP = zP;
	camera1X = xP;
	camera1Y = yP + 8;
	camera1Z = zP;
	camera2X = xP;
	camera2Y = yP + 10;
	camera2Z = zP + 10;
	currentCamera = 1;
	angleFacing = 0.0f;
	letterTID = letterID;
	digitTID = digitID;
	this->textureLvl = textureLvl;
	spinning = 0.0f;
	projectileList = new std::list<Projectile>();
	vect = glm::vec3(0.0f, 0.0f, -0.5);
	cameraLookAt = glm::vec3(camera1X + vect.x, camera1Y + vect.y, camera1Z + vect.z);
	cameraLookAt2 = glm::vec3(xP, yP, zP);
};

void LDModel::moveRight(float x) {

	angleFacing -= x;
	vect = glm::vec3(0.0f, 0.0f, -0.5);

	vect.x = (vect.x * cos(glm::radians(angleFacing))) + (vect.z * sin(glm::radians(angleFacing)));
	vect.z = (-vect.x * sin(glm::radians(angleFacing))) + (vect.z * cos(glm::radians(angleFacing)));

	cameraLookAt.x = camera1X + vect.x;
	cameraLookAt.z = camera1Z + vect.z;


};

void LDModel::moveLeft(float x) {


	angleFacing += x;
	vect = glm::vec3(0.0f, 0.0f, -0.5);

	vect.x = (vect.x * cos(glm::radians(angleFacing))) + (vect.z * sin(glm::radians(angleFacing)));
	vect.z = (-vect.x * sin(glm::radians(angleFacing))) + (vect.z * cos(glm::radians(angleFacing)));

	cameraLookAt.x = camera1X + vect.x;
	cameraLookAt.z = camera1Z + vect.z;



};
bool LDModel::checkBoundary(float z) {

	bool insideMapZ = (zP + vect.z * z <= 148.0) && (zP + vect.z * z >= -148.0);
	bool insideMapX = (xP + vect.x * z <= 148.0) && (xP + vect.x * z >= -148.0);

	bool insideRoadMiddleZ = (xP + vect.x * z <= 15.0 && xP + vect.x * z >= -15.0) && insideMapZ;
	bool insideRoadMiddleX = (zP + vect.z * z <= 15.0 && zP + vect.z * z >= -15.0) && insideMapX;
	bool insideRoadTopX = (zP + vect.z * z <= -80 && zP + vect.z * z >= -100) && insideMapX;
	bool insideRoadBotX = (zP + vect.z * z <= 100 && zP + vect.z * z >= 80) && insideMapX;

	return (insideRoadMiddleZ || insideRoadMiddleX || insideRoadTopX || insideRoadBotX);

};

void LDModel::moveForward(float z) {

	if (checkBoundary(z)) {

		zP = zP + vect.z * z;
		xP = xP + vect.x * z;

		camera1Z = camera1Z + vect.z * z;
		camera2Z = camera2Z + vect.z * z;

		camera1X = camera1X + vect.x * z;
		camera2X = camera2X + vect.x * z;

		cameraLookAt.x = camera1X + vect.x * z;
		cameraLookAt.z = camera1Z + vect.z * z;

		cameraLookAt2 = glm::vec3(xP, yP, zP);
	}
};

void LDModel::moveBack(float z) {

	if (checkBoundary(z)) {

		zP = zP - vect.z * z;
		xP = xP - vect.x * z;

		camera1Z = camera1Z - vect.z * z;
		camera2Z = camera2Z - vect.z * z;

		camera1X = camera1X - vect.x * z;
		camera2X = camera2X - vect.x * z;

		cameraLookAt.x = camera1X + vect.x * z;
		cameraLookAt.z = camera1Z + vect.z * z;

		cameraLookAt2 = glm::vec3(xP, yP, zP);
	}
};

glm::vec3 LDModel::getCameraPos() {

	if (currentCamera == 1) {

		return glm::vec3(camera1X, camera1Y, camera1Z);
	}
	else {

		return glm::vec3(camera2X, camera2Y, camera2Z);
	}
};

glm::vec3 LDModel::getCameraView() {

	if (currentCamera == 1) {

		return cameraLookAt;
	}
	else {

		return cameraLookAt2;
	}
};


glm::mat4 LDModel::getViewMatrix() {

	glm::mat4 viewMatrix = glm::lookAt(getCameraPos(),
		getCameraView(),
		glm::vec3(0.0, 1.0, 0.0));

	return viewMatrix;
};

void LDModel::draw(int model, GLuint vao, GLuint worldMatrixLocation, glm::mat4 groupMatrixW) {




	float cubeHalfLength = 0.5;
	glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), glm::vec3(xP, yP, zP));

	if (model == 1) {

		glm::mat4 transTemp;
		glm::mat4 roTemp;
		glm::mat4 scaleTemp;
		glm::mat4 worldMatrix;
		glm::mat4 groupMatrix;

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, letterTID);

		glBindVertexArray(vao);

		//For R


		//Left

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans   * roTemp * (transTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//std::cout << glm::radians(angleFacing) << std::endl;
		//Top

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans* roTemp * (transTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Right

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp  * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Under

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp  * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Round

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp  * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Front tail

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 2.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//For 4
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, digitTID);
		//Front

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Back

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp  * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Middle

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * roTemp * (transTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

	}
	else if (model == 2) {


		glBindVertexArray(vao);


		//make 'L'
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, letterTID);
		//make cube to cuboid 
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		scalingMatrix = (groupMatrixW)*baseTrans *translationMatrix * scalingMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &scalingMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//second cube for base of L
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, cubeHalfLength, 0.0f));
		translationMatrix = (groupMatrixW)*baseTrans*translationMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//make '0'
		glBindTexture(GL_TEXTURE_2D, digitTID);

		//left side of 0
		glm::mat4 worldMatrix;
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans *translationMatrix * scalingMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//base of 0
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, cubeHalfLength, 0.0f));
		translationMatrix = (groupMatrixW)*baseTrans *translationMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//right side of 0
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans *translationMatrix * scalingMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//top of 0
		translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		translationMatrix = (groupMatrixW)*baseTrans *translationMatrix;
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//'L 0' completed
		glBindVertexArray(0);

	}

	else if (model == 3) {

		glm::mat4 transTemp;
		glm::mat4 roTemp;
		glm::mat4 scaleTemp;
		glm::mat4 worldMatrix;


		glBindVertexArray(vao);

		// --- Drawing 'A' ---
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, letterTID);

		//	Left
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Top
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Middle
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Right
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// --- Drawing '8' ----
		glBindTexture(GL_TEXTURE_2D, digitTID);

		// Left Top
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Left Bottom
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Middle Top
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Middle Center
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//	Middle Bottom
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f * cubeHalfLength, 1.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Right Top
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f * cubeHalfLength, 7.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Right Bottom
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 4.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

	}

	else if (model == 4) {

		glm::mat4 transTemp;
		glm::mat4 roTemp;
		glm::mat4 scaleTemp;
		glm::mat4 worldMatrix;

		glBindVertexArray(vao);

		//For A
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, letterTID);

		//Left vertical side of A

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Middle horizontal side of A

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Right vertical side of A

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f * cubeHalfLength, 4.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Top horizontal side of A

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//For 6
		glBindTexture(GL_TEXTURE_2D, digitTID);

		//Right vertical side of 6

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f * cubeHalfLength, 3.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Left vertical side of 6

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans  * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Bottom horizontal side of 6

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 1.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Middle horizontal side of 6

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f * cubeHalfLength, 5.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Top horizontal side of 6

		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f * cubeHalfLength, 9.0f * cubeHalfLength, 0.0f));
		roTemp = glm::rotate(glm::mat4(1.0f), 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
		worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

	}


	else {

		glBindVertexArray(vao);

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

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, letterTID);//Start with letter texture
		for (int i = 0; i < sizeof(Px) / sizeof(float); ++i) {
			if (i == 5) glBindTexture(GL_TEXTURE_2D, digitTID);//At i==5, switch to number texture
			transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(Px[i] * cubeHalfLength, Py[i] * cubeHalfLength, Pz[i] * cubeHalfLength));
			roTemp = glm::rotate(glm::mat4(1.0f), glm::radians(Theta[i]), glm::vec3(0.0f, 0.0f, 1.0f));
			scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(Sx[i] * cubeHalfLength, Sy[i] * cubeHalfLength, Sz[i] * cubeHalfLength));
			worldMatrix = (groupMatrixW)*baseTrans * (transTemp * roTemp * scaleTemp);
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);
	}

};

void LDModel::addGun(GLuint vaoSphere, GLuint vao, glm::mat4 groupMatrixW, GLuint program1, GLuint program2, float dt) {

	float cubeHalfLength = 0.5f;
	int triCount = 50;
	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	spinning += 180.0f * dt;

	glUseProgram(program2);
	GLuint worldMatrixLocation = glGetUniformLocation(program2, "worldMatrix");
	glBindVertexArray(vao);

	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(xP, yP + 6, zP));
	roTemp = glm::rotate(glm::mat4(1.0), glm::radians(spinning), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 roTY = glm::rotate(glm::mat4(1.0), glm::radians(angleFacing), glm::vec3(0.0f, 1.0f, 0.0f));;

	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 5.0f));
	worldMatrix = (groupMatrixW) * (transTemp * roTY * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	glUseProgram(program1);
	worldMatrixLocation = glGetUniformLocation(program1, "worldMatrix");
	glBindVertexArray(vaoSphere);


	transTemp = glm::translate(glm::mat4(1.0f), glm::vec3(xP, yP + 6.0, zP));
	roTemp = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	worldMatrix = (groupMatrixW) * (transTemp * roTemp * scaleTemp);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_LINES, 0, (triCount * triCount) * 36);

	glBindVertexArray(0);



	for (std::list<Projectile>::iterator it = projectileList->begin(); it != projectileList->end(); it++) {

		it->Update(dt);
		it->Draw(vao);
	}


}

void LDModel::shoot(GLuint shaderProgram) {
	glm::vec3 cameraLookAtT((xP + vect.x*2.0), 0.0, (vect.z)*2.0);
	const float projectileSpeed = 25.0f;
	projectileList->push_back(Projectile(glm::vec3(xP, yP + 6, zP), projectileSpeed * cameraLookAtT, shaderProgram));
}

using namespace glm;
//code from lab3

Projectile::Projectile(vec3 position, vec3 velocity, int shaderProgram) {

	mPosition = position;
	mVelocity = velocity;
	mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

}

void Projectile::Update(float dt)
{
	mPosition += mVelocity * dt;
}

void Projectile::Draw(GLuint vao) {

	glBindVertexArray(vao);
	mat4 worldMatrix = translate(mat4(1.0f), mPosition) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.3f, 0.3f, 0.3f));
	glUniformMatrix4fv(mWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}