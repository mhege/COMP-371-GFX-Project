#include "Car.h"
#include <algorithm>
void drawTire(GLuint Texture, GLuint worldMatrixLocation, mat4 WMatrix, float Px, float Py, float Pz, float thetaXZ);
float getSteeringAngle(float Px, float Pz, float theta, float& V);

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

struct ColoredVertex {
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

Car::Car() {
	Px = -25.0f; Py = 0.0f; Pz = -82.0f;
	Lx = 6.0f; Ly = 3.0f; Lz = 4.0f;
	V = 0.4f;
	w = 3.0f;//Degree/s
	theta = 0.0f;
	theta_t = 45.0f;//Degrees
}

Car::~Car() {

}

void Car::draw(GLuint worldMatrixLocation, mat4 groupdMatrix) {
	glBindVertexArray(vaoCube);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);//bind metal
	glm::mat4 transTemp;
	glm::mat4 roTemp;
	glm::mat4 scaleTemp;
	glm::mat4 worldMatrix;
	glm::mat4 baseTrans1;
	
	baseTrans1 = glm::translate(glm::mat4(1.0f), glm::vec3(Px, Py, Pz))
		*glm::rotate(glm::mat4(1.0f), radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
	
	float h = 1.0f;//Clearance height
	float t = 0.2f;//Roof thicknes
	float tp = t * 3;//Pillar thickness
	char* type = "triangle";

	float PX[] = { 0.0f,-Lx/6,Lx/6-tp/2,-Lx/2+tp/2,-Lx/2+tp/2,Lx/6-tp/2};
	float PY[] = { Ly/4+h,Ly+h+t/2,0.75f*Ly+h,0.75f*Ly+h,0.75f*Ly+h,0.75f*Ly+h};
	float PZ[] = { 0.0f,0.0f,Lz/2-tp/2,Lz/2-tp/2,-Lz/2+tp/2,-Lz/2+tp/2};
	float Theta[] = { 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f};
	float Sx[] = { Lx,2.0f/3*Lx,tp,tp,tp,tp};
	float Sy[] = { Ly/2,t,Ly/2,Ly/2,Ly/2,Ly/2};
	float Sz[] = {Lz,Lz,tp,tp,tp,tp};

	for (int i = 0; i < sizeof(PX) / sizeof(float); ++i) {
		transTemp = glm::translate(glm::mat4(1.0f), glm::vec3((PX[i]), PY[i], PZ[i]));
		roTemp = glm::rotate(glm::mat4(1.0f), radians(Theta[i]), glm::vec3(0.0f, 0.0f, 1.0f));
		scaleTemp = glm::scale(glm::mat4(1.0f), glm::vec3(Sx[i], Sy[i], Sz[i]));
		//worldMatrix = (groupMatrixW)*baseTrans*baseTrans1*(groupMatrixM)*(transTemp * roTemp * scaleTemp);
		worldMatrix = groupdMatrix*baseTrans1 * (transTemp * roTemp * scaleTemp);
		glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
		drawCube(type);
	}
	glBindVertexArray(0);
	float COS = cos(radians(theta));
	float SIN = sin(radians(theta));
	float front = Lx / 3;
	float back = -Lx / 3;
	float right = Lz / 2;
	float left = -Lz / 2;
	drawTire(Textures[2], worldMatrixLocation, groupdMatrix, Px+front*COS+left*SIN, Py+h, Pz+left*COS-front*SIN, (theta+theta_t)+180);//FL
	drawTire(Textures[2], worldMatrixLocation, groupdMatrix, Px+front*COS+right*SIN, Py+h, Pz+right*COS-front*SIN, (theta+theta_t));//FR
	drawTire(Textures[2], worldMatrixLocation, groupdMatrix, Px+back*COS+left*SIN, Py+h, Pz+left*COS-back*SIN, theta+180);//BL
	drawTire(Textures[2], worldMatrixLocation, groupdMatrix, Px+back*COS+right*SIN, Py+h, Pz+right*COS-back*SIN, theta);//BR
}

void Car::calculate(float dt) {
	theta_t = getSteeringAngle(Px, Pz,theta, V);
	float Vx = V * cos(radians(theta+theta_t/2));
	float Vz = -V * sin(radians(theta+theta_t/2));
	Px += Vx * cos(radians(theta_t));
	Pz += Vz * cos(radians(theta_t));
	float understeer = 175.0f;
	w = sin(radians(theta_t)) * V*understeer;
	theta += w * dt;
	if (theta > 180) theta -= 360;
	if (theta < -180) theta += 360;
}


void drawTire(GLuint Texture, GLuint worldMatrixLocation, mat4 groupdMatrix0, float Px, float Py, float Pz, float thetaXZ) {
	static bool init = true;
	static GLuint vao;
	unsigned int n = 17;
	if (init) {
		init = false;
		//cout << "initialization called on drawTire" << endl;
		ColoredVertex* tempVector = new ColoredVertex[9 * (n+1)];
		vec3 color = vec3(0.0f, 0.0f, 0.0f);
		float* tempX = new float[n+1];
		float* tempY = new float[n+1];
		float r = 1.0f;
		float L = 0.5f;
		float theta;
		for (int j = 0; j < n+1; j++) {
			theta = j * 3.14159265f / n * 2;
			//cout << theta << endl;
			tempX[j] = r * cos(theta);
			tempY[j] = r * sin(theta);
		}

		for (int i = 0; i < n+1; ++i) {
			tempVector[i * 3] = ColoredVertex(vec3(tempX[i], tempY[i], 0), color, vec3(tempX[i], tempY[i], 0));
			tempVector[i * 3 + 2] = ColoredVertex(vec3(tempX[i], tempY[i], L), color, vec3(tempX[i], tempY[i], 0));
			tempVector[i * 3 + 1] = ColoredVertex(vec3(tempX[(i + 1) % n], tempY[(i + 1) % n], 0), color, vec3(tempX[i], tempY[i], 0));/**/
			tempVector[(i + n) * 3] = ColoredVertex(vec3(tempX[i], tempY[i], L), color, vec3(tempX[i], tempY[i], 0));
			tempVector[(i + n) * 3 + 2] = ColoredVertex(vec3(tempX[(i + 1) % n], tempY[(i + 1) % n], 0), color, vec3(tempX[i], tempY[i], 0));
			tempVector[(i + n) * 3 + 1] = ColoredVertex(vec3(tempX[(i + 1) % n], tempY[(i + 1) % n], L), color, vec3(tempX[i], tempY[i], 0));/**/
			tempVector[(i + 2 * n) * 3] = ColoredVertex(vec3(tempX[i], tempY[i], L), color, vec3(0.0f, 0.0f, 1.0f));
			tempVector[(i + 2 * n) * 3 + 2] = ColoredVertex(vec3(0, 0, L), color, vec3(0.0f, 0.0f, 1.0f));
			tempVector[(i + 2 * n) * 3 + 1] = ColoredVertex(vec3(tempX[(i + 1) % n], tempY[(i + 1) % n], L), color, vec3(0.0f,0.0f,1.0f));/**/
		}
		vao = createVertexArrayObject(tempVector, 9 * (n+1) * 3)[0];
		delete[] tempVector;
		delete[] tempX;
		delete[] tempY;
	}
	mat4 groupMatrix = groupdMatrix0*glm::translate(glm::mat4(1.0f), glm::vec3((Px), Py, Pz))
			* glm::rotate(glm::mat4(1.0f), radians(thetaXZ), glm::vec3(0.0f, 1.0f, 0.0f));
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groupMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 9 * (n+1) -1);
	glBindVertexArray(0);

}

float getSteeringAngle(float Px, float Pz, float theta, float& V) {//Currently configured to make it go CCW
	float theta_goal;
	float dtheta_goal = 0;
	const float kp_position = 10.0f;
	const float dtheta_goal_max = 90.0f;
	//cout << "Px: " << Px << "\tPz: " << Pz << endl;
	if (Px >= -20) {
		if (Pz >= 0) {
			dtheta_goal = (98 - Pz) * kp_position;
			dtheta_goal = std::max(-dtheta_goal_max, dtheta_goal);
			dtheta_goal = std::min(dtheta_goal_max, dtheta_goal);
			theta_goal = 180+ dtheta_goal;
			if (theta < dtheta_goal) theta_goal -= 360;
		} else {
			dtheta_goal = (-8 - Px) * kp_position;
			dtheta_goal = std::max(-dtheta_goal_max, dtheta_goal);
			dtheta_goal = std::min(dtheta_goal_max, dtheta_goal);
			theta_goal = -90 + dtheta_goal;
			if (theta > dtheta_goal+90) theta_goal += 360;
		}
	} else {
		if (Pz >= 0) {
			dtheta_goal = -(-165 - Px) * kp_position;
			dtheta_goal = std::max(-dtheta_goal_max, dtheta_goal);
			dtheta_goal = std::min(dtheta_goal_max, dtheta_goal);
			theta_goal = 90 + dtheta_goal;
			if (theta < dtheta_goal - 90) theta_goal -= 360;
		} else {
			dtheta_goal = -(-82 - Pz) * kp_position;
			dtheta_goal = std::max(-dtheta_goal_max, dtheta_goal);
			dtheta_goal = std::min(dtheta_goal_max, dtheta_goal);
			theta_goal = dtheta_goal;
			if (theta < dtheta_goal-180) theta_goal -= 360;
		}
	}
	float theta_error = theta_goal - theta;
	float kp_theta = 2.0f;//Proportional constant for theta
	float kp_dXZ = 30.0f;
	float theta_t = kp_theta* theta_error;
	if (theta_t >= 45) theta_t = 45;
	if (theta_t <= -45) theta_t = -45;
	if (theta_t < 1 && theta_t > -1) V = 2.0f;
	else if (theta_t < 5 && theta_t > -5) V = 1.0f;
	else if (theta_t < 10 && theta_t > -10) V = 0.5f;
	else V = 0.3f;
	return theta_t;
}

//Legacy functions
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

	return vaos;//This is a dangerous return method
}
