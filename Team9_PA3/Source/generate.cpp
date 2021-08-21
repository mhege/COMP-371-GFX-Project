#include "generate.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

Generate::Generate() {};

ColoredVertex* Generate::genSphere() {
	//Sphere calculation

	const double PI = 3.14159;
	const int triCount = 50;
	float Radius = .5;
	sphereVector = new ColoredVertex[12 * (triCount*triCount)];

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

	return sphereVector;
};

int Generate::compileAndLinkShaders(std::string vertexPath, std::string fragmentPath)
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

int* Generate::createVertexArrayObject(ColoredVertex vertexArray[], int vertexCount)
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

int* Generate::createTextureVertexArrayObject(TexturedColoredVertex vertexArray[], int vertexCount)
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

GLuint Generate::loadTexture(const char* filename)
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

Generate::~Generate() {

	delete[] sphereVector;

};