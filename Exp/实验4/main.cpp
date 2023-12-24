#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Torus.h"
#include "sphere.h"
#include "Utils.h"
#include "ImportedModel.h"
//#include "shader.h"
using namespace std;

//向弧度转换
float toRadians(float degrees) {
	return (degrees * 2.0f * 3.14159f) / 360.0f; 
}

#define numVAOs 20  //10(从0开始的编号）
#define numVBOs 30  //21
#define numEBOs 10  //2

#define SCALE 2
#define SCALE2 2.5

float cameraX, cameraY, cameraZ;//相机的坐标
float torLocX, torLocY, torLocZ;//环的坐标
float sphLocX, sphLocY, sphLocZ;//球的坐标

GLuint renderingProgram1;
GLuint skybox_shader;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];
GLuint vColorLoc;
GLuint vPositionLoc;

Sphere mySphere(0.2 , 58);//定义一个球,中间的那个
Sphere mySphere2(0.06, 58);//定义一个球，光源
Torus myTorus(0.5f/ SCALE, 0.2f/ SCALE, 48);//定义一个环
int numTorusVertices = myTorus.getNumVertices();
int numTorusIndices = myTorus.getNumIndices();
int numTorusVertices2 = mySphere.getNumVertices();
int numTorusIndices2 = mySphere.getNumIndices();
int numTorusIndices3 = mySphere2.getNumIndices();

GLuint trunkTexture;
GLuint sphereTexture;
GLuint torusTexture;
GLuint skyTexture;
GLuint cubemapTexture;
GLuint leavesTexture;
GLuint groudTexture;
GLuint shuttleTexture;
GLuint popTexture;

float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc,vLoc_sky,projLoc_sky;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3];//光源的朝向
float fov;
float Cube_l = 0.3, Cube_w = 0.3, Cube_h = 1.0;
//float Cube_l = 0.3, Cube_w = 0.3, Cube_h = 0.3;
float skybox_l = 8, skybox_w = 8, skybox_h = 8;

ImportedModel myModel("shuttle.obj");

// white light
float globalAmbient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //环境光
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //漫反射
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //高光

// gold material
float* matAmb = Utils::goldAmbient();//环境光
float* matDif = Utils::goldDiffuse();//漫反射
float* matSpe = Utils::goldSpecular();//高光
float matShi = Utils::goldShininess();

class Light {
public:
	glm::vec3 lightLoc = glm::vec3(0.5f, 0.2f, 0.2f);//光源的位置
	float MovementSpeed=0.001;

	void ProcessKeyboard(int direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == GLFW_KEY_I)
			lightLoc.y += velocity;
		if (direction == GLFW_KEY_K)
			lightLoc.y -= velocity;
		if (direction == GLFW_KEY_J)
			lightLoc.x -= velocity;
		if (direction == GLFW_KEY_L)
			lightLoc.x += velocity;
		if (direction == GLFW_KEY_O)
			lightLoc.z -= velocity;
		if (direction == GLFW_KEY_P)
			lightLoc.z += velocity;
	}
	glm::vec3 GetLight() {
		return lightLoc;
	}
};
Light light;

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f))
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(0.01f),
		MouseSensitivity(0.1f), Yaw(-90.0f), Pitch(0.0f) {
		Position = position;
		WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(int direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;
		if (direction == GLFW_KEY_W)
			Position += Front * velocity;
		if (direction == GLFW_KEY_S)
			Position -= Front * velocity;
		if (direction == GLFW_KEY_A)
			Position -= Right * velocity;
		if (direction == GLFW_KEY_D)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset,
		GLboolean constrainPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += 7*xoffset;
		Pitch += 7*yoffset;

		if (constrainPitch) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
// 相机
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));


void installLights(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram1, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram1, "light.ambient");//环境光
	diffLoc = glGetUniformLocation(renderingProgram1, "light.diffuse");//漫反射
	specLoc = glGetUniformLocation(renderingProgram1, "light.specular");//高光
	posLoc = glGetUniformLocation(renderingProgram1, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram1, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram1, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram1, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram1, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram1, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram1, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram1, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram1, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram1, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram1, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram1, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram1, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram1, mshiLoc, matShi);
}
void silver_material() {
	matAmb = Utils::silverAmbient();//环境光
	matDif = Utils::silverDiffuse();//漫反射
	matSpe = Utils::silverSpecular();//高光
	matShi = Utils::silverShininess();
}
void gold_material() {
	matAmb = Utils::goldAmbient();//环境光
	matDif = Utils::goldDiffuse();//漫反射
	matSpe = Utils::goldSpecular();//高光
	matShi = Utils::goldShininess();
}
void yellow_material() {
	matAmb = Utils::yellowAmbient();//环境光
	matDif = Utils::yellowDiffuse();//漫反射
	matSpe = Utils::yellowSpecular();//高光
	matShi = Utils::yellowShininess();
}
void bronze_material() {
	matAmb = Utils::bronzeAmbient();//环境光
	matDif = Utils::bronzeDiffuse();//漫反射
	matSpe = Utils::bronzeSpecular();//高光
	matShi = Utils::bronzeShininess();
}
void pearl_material() {
	matAmb = Utils::pearlAmbient();//环境光
	matDif = Utils::pearlDiffuse();//漫反射
	matSpe = Utils::pearlSpecular();//高光
	matShi = Utils::pearlShininess();
}
void jade_material() {
	matAmb = Utils::jadeAmbient();//环境光
	matDif = Utils::jadeDiffuse();//漫反射
	matSpe = Utils::jadeSpecular();//高光
	matShi = Utils::jadeShininess();
}
void ruby_material() {
	matAmb = Utils::rubyAmbient();//环境光
	matDif = Utils::rubyDiffuse();//漫反射
	matSpe = Utils::rubySpecular();//高光
	matShi = Utils::rubyShininess();
}
void emerald_material() {
	matAmb = Utils::emeraldAmbient();//环境光
	matDif = Utils::emeraldDiffuse();//漫反射
	matSpe = Utils::emeraldSpecular();//高光
	matShi = Utils::emeraldShininess();
}
void regular_material() {
	matAmb = Utils::regularAmbient();//环境光
	matDif = Utils::regularDiffuse();//漫反射
	matSpe = Utils::regularSpecular();//高光
	matShi = Utils::regularShininess();
}

void setupVertices_pri(void) {
	float pyramidPositions2[] =
	{ -1.0f, -1.0f, 1.0f,  0.0f,2.0f,4.0f,
		1.0f, -1.0f, 1.0f, 0.0f,2.0f,4.0f,
		0.0f, 1.0f, 0.0f,   0.0f,2.0f,4.0f, //front

		1.0f, -1.0f, 1.0f,  4.0f,2.0f,0.0f,
		1.0f, -1.0f, -1.0f, 4.0f,2.0f,0.0f,
		0.0f, 1.0f, 0.0f,   4.0f,2.0f,0.0f, //right

		1.0f, -1.0f, -1.0f, 0.0f,2.0f,-4.0f,
		-1.0f, -1.0f, -1.0f,  0.0f,2.0f,-4.0f,
		0.0f, 1.0f, 0.0f,  0.0f,2.0f,-4.0f, //back

		-1.0f, -1.0f, -1.0f,   -4.0f,2.0f,0.0f,
		-1.0f, -1.0f, 1.0f, -4.0f,2.0f,0.0f,
		0.0f, 1.0f, 0.0f,  -4.0f,2.0f,0.0f,//left

		-1.0f, -1.0f, -1.0f,  0.0f,-1.0f,0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,-1.0f,0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f,-1.0f,0.0f,//LF

		1.0f, -1.0f, 1.0f, 0.0f,-1.0f,0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f,-1.0f,0.0f,
		1.0f, -1.0f, -1.0f,  0.0f,-1.0f,0.0f,//RR
	};
	float pyramidTexture[36] =
	{   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions2), pyramidPositions2, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[17]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexture), pyramidTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST); //启用深度测试
}

void setupVertices(void) {
	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();//纹理
	std::vector<glm::vec3> norm = myTorus.getNormals(); //法向量

	std::vector<float> pvalues; //顶点的坐标
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}

	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	//cout << pvalues.size() << endl;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);//useless?
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	//cout << nvalues.size() << endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	torusTexture = Utils::loadTexture("bread.jpg");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, torusTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glEnable(GL_DEPTH_TEST); //启用深度测试
	glDepthFunc(GL_LEQUAL);
}

void setup_sphere() {

	std::vector<glm::vec3> vert = mySphere.getVertices(); //顶点
	std::vector<glm::vec3> norm = mySphere.getNormals(); //法向量
	std::vector<int> ind = mySphere.getIndices(); //
	
//绑定当前VAO
	glBindVertexArray(vao[1]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);/****顶点****/
	glBufferData(GL_ARRAY_BUFFER, vert.size() * 4 * sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	
	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);/****法向量****/
	glBufferData(GL_ARRAY_BUFFER, norm.size() * 4 * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

	
	glEnable(GL_DEPTH_TEST); //启用深度测试
	
}

void setup_sphere2() {

	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}


	glBindVertexArray(vao[6]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[11]);/****顶点****/
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);/****法向量****/
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[13]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	sphereTexture = Utils::loadTexture("earth.jpg");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sphereTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glEnable(GL_DEPTH_TEST); //启用深度测试
	glDepthFunc(GL_LEQUAL);

}

void setup_sphere_light() {

	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getvers_Normals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}
//绑定当前VAO
	glBindVertexArray(vao[2]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);/****顶点****/
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);/****法向量****/
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);
	glEnable(GL_DEPTH_TEST); //启用深度测试

}

void setup_cube() {
	float vertices2[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float vertices[] = {
		-0.51f, -0.51f, -0.51f,  0.01f,  0.01f, -1.0f,
		 0.5f, -0.51f, -0.51f,  0.01f,  0.01f, -1.0f,
		 0.5f,  0.5f, -0.51f,  0.01f,  0.01f, -1.0f,
		 0.5f,  0.5f, -0.51f,  0.01f,  0.01f, -1.0f,
		-0.51f,  0.5f, -0.51f,  0.01f,  0.01f, -1.0f,
		-0.51f, -0.51f, -0.51f,  0.01f,  0.01f, -1.0f,

		-0.51f, -0.51f,  0.5f,  0.01f,  0.01f,  1.0f,
		 0.5f, -0.51f,  0.5f,  0.01f,  0.01f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.01f,  0.01f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.01f,  0.01f,  1.0f,
		-0.51f,  0.5f,  0.5f,  0.01f,  0.01f,  1.0f,
		-0.51f, -0.51f,  0.5f,  0.01f,  0.01f,  1.0f,

		-0.51f,  0.5f,  0.5f, -1.0f,  0.01f,  0.01f,
		-0.51f,  0.5f, -0.51f, -1.0f,  0.01f,  0.01f,
		-0.51f, -0.51f, -0.51f, -1.0f,  0.01f,  0.01f,
		-0.51f, -0.51f, -0.51f, -1.0f,  0.01f,  0.01f,
		-0.51f, -0.51f,  0.5f, -1.0f,  0.01f,  0.01f,
		-0.51f,  0.5f,  0.5f, -1.0f,  0.01f,  0.01f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.01f,  0.01f,
		 0.5f,  0.5f, -0.51f,  1.0f,  0.01f,  0.01f,
		 0.5f, -0.51f, -0.51f,  1.0f,  0.01f,  0.01f,
		 0.5f, -0.51f, -0.51f,  1.0f,  0.01f,  0.01f,
		 0.5f, -0.51f,  0.5f,  1.0f,  0.01f,  0.01f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.01f,  0.01f,

		-0.51f, -0.51f, -0.51f,  0.01f, -1.0f,  0.01f,
		 0.5f, -0.51f, -0.51f,  0.01f, -1.0f,  0.01f,
		 0.5f, -0.51f,  0.5f,  0.01f, -1.0f,  0.01f,
		 0.5f, -0.51f,  0.5f,  0.01f, -1.0f,  0.01f,
		-0.51f, -0.51f,  0.5f,  0.01f, -1.0f,  0.01f,
		-0.51f, -0.51f, -0.51f,  0.01f, -1.0f,  0.01f,

		-0.51f,  0.5f, -0.51f,  0.01f,  1.0f,  0.01f,
		 0.5f,  0.5f, -0.51f,  0.01f,  1.0f,  0.01f,
		 0.5f,  0.5f,  0.5f,  0.01f,  1.0f,  0.01f,
		 0.5f,  0.5f,  0.5f,  0.01f,  1.0f,  0.01f,
		-0.51f,  0.5f,  0.5f,  0.01f,  1.0f,  0.01f,
		-0.51f,  0.5f, -0.51f,  0.01f,  1.0f,  0.01f
	};
	for (int i = 0; i < 36; i++) {
		vertices[i * 6] *= Cube_l;
		vertices[i * 6+1] *= Cube_h;
		vertices[i * 6+2] *= Cube_w;
	}
	//for (int i = 0; i < 216; i++)cout << vertices[i] << endl;
	//——————————立方体——————
//绑定当前VAO
	glBindVertexArray(vao[3]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST); //启用深度测试
}

void setup_cube2() {
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		//front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		//left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,//1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,//1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,//0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,//0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,//1.0f,  0.0f,
		//right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,//1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,//1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,//0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,//0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,//0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,//1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		//up
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//3
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,//2
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,//2
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,//1
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//3

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	};
	
	for (int i = 0; i < 42; i++) {
		vertices[i * 8] *= Cube_l;
		vertices[i * 8 + 1] *= Cube_h;
		vertices[i * 8 + 2] *= Cube_w;
	}
	//for (int i = 0; i < 216; i++)cout << vertices[i] << endl;
	//——————————立方体——————
//绑定当前VAO
	glBindVertexArray(vao[5]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Tex
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	trunkTexture = Utils::loadTexture("trunk.jpg");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trunkTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glEnable(GL_DEPTH_TEST); //启用深度测试
	glDepthFunc(GL_LEQUAL);
}

void setup_groud() {
	float vertices[] = {
		-0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,//-0.5f, -0.5f,
		 0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // 0.5f, -0.5f,
		 0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // 0.5f,  0.5f,
		 0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // 0.5f,  0.5f,
		-0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //-0.5f,  0.5f,
		-0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, //-0.5f, -0.5f,
	};

//绑定当前VAO
	glBindVertexArray(vao[9]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[18]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Tex
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void setup_skybox() {
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

//绑定当前VAO
	glBindVertexArray(vao[8]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[16]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	cubemapTexture = Utils::loadCubeMap("skybox_grass");

}

void setup_cube_sky() {
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		//up
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//3
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,//2
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,//2
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,//1
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//3
		//0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,//2
		//-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f,//1
		//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f,//3

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	};

	float cubeTextureCoord[72] = {
		1.00f, 0.66f, 1.00f, 0.33f, 0.75f, 0.33f, // 背面右下角
		0.75f, 0.33f, 0.75f, 0.66f, 1.00f, 0.66f, // 背面左上角
		0.75f, 0.33f, 0.50f, 0.33f, 0.75f, 0.66f, // 右面右下角
		0.50f, 0.33f, 0.50f, 0.66f, 0.75f, 0.66f, // 右面左上角
		0.50f, 0.33f, 0.25f, 0.33f, 0.50f, 0.66f, // 正面右下角
		0.25f, 0.33f, 0.25f, 0.66f, 0.50f, 0.66f, // 正面左上角
		0.25f, 0.33f, 0.00f, 0.33f, 0.25f, 0.66f, // 左面右下角
		0.00f, 0.33f, 0.00f, 0.66f, 0.25f, 0.66f, // 左面左上角
		0.25f, 0.33f, 0.50f, 0.33f, 0.50f, 0.00f, // 下面右下角
		0.50f, 0.00f, 0.25f, 0.00f, 0.25f, 0.33f, // 下面左上角
		0.25f, 1.00f, 0.50f, 1.00f, 0.50f, 0.66f, // 上面右下角
		0.50f, 0.66f, 0.25f, 0.66f, 0.25f, 1.00f  // 上面左上角
	};


	for (int i = 0; i < 42; i++) {
		vertices[i * 8] *= Cube_l;
		vertices[i * 8 + 1] *= Cube_h;
		vertices[i * 8 + 2] *= Cube_w;
	}
	//for (int i = 0; i < 216; i++)cout << vertices[i] << endl;
	//——————————立方体——————
//绑定当前VAO
	glBindVertexArray(vao[7]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[15]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureCoord), cubeTextureCoord, GL_STATIC_DRAW);
	//Tex
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);


	glDepthFunc(GL_LEQUAL);
}

void setup_model()
{
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3> norm = myModel.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myModel.getNumVertices(); i++) {
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);
		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);
		nvalues.push_back((norm[i]).x);
		nvalues.push_back((norm[i]).y);
		nvalues.push_back((norm[i]).z);
	}


	glBindVertexArray(vao[10]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[19]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[20]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[21]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void init(GLFWwindow* window) 
{
	//renderingProgram1 = Utils::createShaderProgram("./BlinnPhongShaders/vertShader.glsl", "./BlinnPhongShaders/fragShader.glsl");
	//renderingProgram1 = Utils::createShaderProgram("./GouraudShaders/vertShader.glsl", "./GouraudShaders/fragShader.glsl");
	renderingProgram1 = Utils::createShaderProgram("./PhongShaders/vertShader.glsl", "./PhongShaders/fragShader.glsl");
	skybox_shader = Utils::createShaderProgram("./skybox_shader/sky_vertShader.glsl", "./skybox_shader/sky_fragShader.glsl");

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 1.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = -0.5f / SCALE;
	fov = 60;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);  
	pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);
	glGenBuffers(numEBOs, ebo);

	setupVertices();//环
	//setup_sphere();//球
	setup_sphere_light();
	setupVertices_pri();
	setup_cube();
	setup_cube2();
	setup_sphere2();//球
	setup_cube_sky();
	setup_skybox();
	setup_groud();
	setup_model();

	torusTexture = Utils::loadTexture("bread2.png");
	trunkTexture = Utils::loadTexture("trunk.jpg");
	sphereTexture = Utils::loadTexture("earth.jpg");
	skyTexture = Utils::loadTexture("sky.jpg");
	leavesTexture = Utils::loadTexture("leaves.jpg");
	groudTexture = Utils::loadTexture("grass.jpg");
	shuttleTexture = Utils::loadTexture("spstob_1.jpg");
	popTexture = Utils::loadTexture("pop.jpg");
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram1);

	mvLoc = glGetUniformLocation(renderingProgram1, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram1, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram1, "norm_matrix");

	glm::mat4 view = camera.GetViewMatrix();//相机的矩阵
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	vMat = view;

	currentLightPos = light.GetLight();//这个是我设置的变化的坐标
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 0.0f, 1.0f));
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));
	/**********************这里设置材质***********************************/
	bronze_material();
	installLights(vMat);
	/**********************这里设置材质***********************************/

	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	/************************************绘制圆环*********************************/
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, -0.75f, -0.8f));
	//mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.38f, 0.38f));
	//mMat = glm::mat4(1.0f);
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindTexture(GL_TEXTURE_2D, torusTexture);
	regular_material();
	installLights(vMat);
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
	
	/************************************绘制测试实体*********************************/

	//glBindTexture(GL_TEXTURE_2D, trunkTexture);
	//glBindVertexArray(vao[5]); //测试立方体
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(vao[4]);//测试四棱锥
	//glDrawArrays(GL_TRIANGLES, 0, 18);

	/*********************************绘制草地****************************************/
	
	//x--》右 y--》前
	for (int i = -40; i < 40; i++) {
		for (int j = -40; j < 40; j++) {
		mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
		mMat *= glm::rotate(mMat, toRadians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(j*1.0f, i*1.0f, 0.0f));
		mvMat = vMat * mMat;
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
		invTrMat = glm::transpose(glm::inverse(mvMat));
		glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
		glBindTexture(GL_TEXTURE_2D, groudTexture);
		regular_material();
		glBindVertexArray(vao[9]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	/************************************彩色球*********************************************/
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.75f, -0.8f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.48f, 0.48f, 0.48f));
	//mMat = glm::mat4(1.0f);
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	//球体
	glBindTexture(GL_TEXTURE_2D, popTexture);
	regular_material();
	installLights(vMat);
	glBindVertexArray(vao[6]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());

	/*************绘制场景中的球，用自己的坐标进行设置（圆环上的小点）***************/
	
	ruby_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(1.55f, -0.65f, -0.8f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.18f, 0.18f, 0.18f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[6]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	
	/***************圆环上的小点2，用自己的坐标进行设置*************************/
	
	jade_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(1.2f, -0.69f, -0.5f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.18f, 0.18f, 0.18f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[6]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	
	/***************圆环上的小点3，用自己的坐标进行设置*************************/
	
	ruby_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(1.3f, -0.67f, -1.08f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.18f, 0.18f, 0.18f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[6]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	
	/***************圆环上的小点4，用自己的坐标进行设置*************************/
	pearl_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, -0.66f, -0.95f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.18f, 0.18f, 0.18f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[6]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	
	/***************树干立方体，用自己的坐标进行设置*************************/
	bronze_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f,-1.0f,-0.8f));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindTexture(GL_TEXTURE_2D, trunkTexture);
	//trunkTexture
	glBindVertexArray(vao[5]); //测试立方体
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	/***************树叶四棱锥1，用自己的坐标进行设置*************************/
	glBindTexture(GL_TEXTURE_2D, leavesTexture);
	regular_material();
	//emerald_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.5f, -0.8f));
	//mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.16f, 0.3f, 0.09f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	
	/***************树叶四棱锥2，用自己的坐标进行设置*************************/

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.25f, -0.8f));
	//mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.13f, 0.2f, 0.04f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.38f, 0.38f));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	/***************树叶四棱锥3，用自己的坐标进行设置*************************/

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, -0.1f, -0.8f));
	//mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.11f, 0.1f, 0.00f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.45f, 0.45f, 0.45f));
	//mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	/********************************导入模型***********************************/
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(2.3f, 2.1f, -1.0f));
	//mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.45f, 0.45f, 0.45f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindTexture(GL_TEXTURE_2D, shuttleTexture);
	glBindVertexArray(vao[10]);
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
	/***************绘制光源处的球，要用光的坐标进行设置*************************/
	gold_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(currentLightPos.x, currentLightPos.y
		, currentLightPos.z));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	/****************************天空盒*****************************/
	vLoc_sky = glGetUniformLocation(skybox_shader, "view");
	projLoc_sky = glGetUniformLocation(skybox_shader, "projection");
	// draw skybox as last
	
	glUseProgram(skybox_shader);
	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	glUniformMatrix4fv(vLoc_sky, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc_sky, 1, GL_FALSE, glm::value_ptr(pMat));
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	// skybox cube
	glBindVertexArray(vao[8]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
	static float lastX = 800.0f / 2.0;
	static float lastY = 600.0 / 2.0;
	static bool firstMouse = true;

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset =
		lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	//update_lightloc(xpos, ypos, lightLoc.z);
	camera.ProcessMouseMovement(xoffset, yoffset);
}
void key_callback(GLFWwindow *window, int key, int scancode, int action,int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		camera.ProcessKeyboard(key, glfwGetTime());
		light.ProcessKeyboard(key, glfwGetTime());
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

int main(void) {
	cout <<( 180.0f * 2.0f * 3.14159f) / 360.0f << endl;
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "ShadingTorus", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
