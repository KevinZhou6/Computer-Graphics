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
using namespace std;

//向弧度转换
float toRadians(float degrees) {
	return (degrees * 2.0f * 3.14159f) / 360.0f; 
}

#define numVAOs 10
#define numVBOs 30
#define numEBOs 10

#define SCALE 2.5
#define SCALE2 2.5

float cameraX, cameraY, cameraZ;//相机的坐标
float torLocX, torLocY, torLocZ;//环的坐标
float sphLocX, sphLocY, sphLocZ;//球的坐标

GLuint renderingProgram1;
GLuint renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];
GLuint vColorLoc;
GLuint vPositionLoc;

Sphere mySphere(0.05 / SCALE, 58);//定义一个球,中间的那个
Sphere mySphere2(0.06, 58);//定义一个球，光源
Torus myTorus(0.5f/ SCALE, 0.2f/ SCALE, 48);//定义一个环
int numTorusVertices = myTorus.getNumVertices();
int numTorusIndices = myTorus.getNumIndices();
int numTorusVertices2 = mySphere.getNumVertices();
int numTorusIndices2 = mySphere.getNumIndices();
int numTorusIndices3 = mySphere2.getNumIndices();


float amt = 0.0f;

// variable allocation for display
GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3];//光源的朝向
float fov;
float Cube_l = 0.1, Cube_w = 0.1, Cube_h = 0.5;

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
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
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(0.001f),
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

		Yaw += xoffset;
		Pitch += yoffset;

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
	float pyramidPositions3[] =
	{ -1.0f, -1.0f, 1.0f,  0.0f,2.0f,4.0f,
		1.0f, -1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,    //front

		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,    //right

		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,  //back

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,  //left

		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, //LF

		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f  //RR
	};
	float pyramidPositions[] =
	{  
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 
		1.0f, -1.0f, -1.0f, 
		-1.0f, -1.0f, -1.0f,
		0.0f, 1.0f, 0.0f, 

	};
	//normals[i] = glm::cross(tTangents[i], sTangents[i]);
	std::vector<glm::vec3> norm;
	//norm.push_back (normalize(glm::cross(glm::vec3(-1.0f, -2.0f, 1.0f), glm::vec3(1.0f, -2.0f, 1.0f))));
	//norm.push_back(normalize(glm::cross(glm::vec3(1.0f, -2.0f, 1.0f), glm::vec3(1.0f, -2.0f, -1.0f))));
	//norm.push_back(normalize(glm::cross(glm::vec3(1.0f, -2.0f, -1.0f), glm::vec3(-1.0f, -2.0f, -1.0f))));
	//norm.push_back(normalize(glm::cross(glm::vec3(-1.0f, -2.0f, -1.0f), glm::vec3(-1.0f, -2.0f, 1.0f))));
	norm.push_back(glm::cross(glm::vec3(-1.0f, -2.0f, 1.0f), glm::vec3(1.0f, -2.0f, 1.0f)));
	norm.push_back(glm::cross(glm::vec3(1.0f, -2.0f, 1.0f), glm::vec3(1.0f, -2.0f, -1.0f)));
	norm.push_back(glm::cross(glm::vec3(1.0f, -2.0f, -1.0f), glm::vec3(-1.0f, -2.0f, -1.0f)));
	norm.push_back(glm::cross(glm::vec3(-1.0f, -2.0f, -1.0f), glm::vec3(-1.0f, -2.0f, 1.0f)));
	norm.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	norm.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	float nor[54] =
	{
		norm[0].x,norm[0].y,norm[0].z,
		norm[0].x,norm[0].y,norm[0].z,
		norm[0].x,norm[0].y,norm[0].z,

		norm[1].x,norm[1].y,norm[1].z,
		norm[1].x,norm[1].y,norm[1].z,
		norm[1].x,norm[1].y,norm[1].z,

		norm[2].x,norm[2].y,norm[2].z,
		norm[2].x,norm[2].y,norm[2].z,
		norm[2].x,norm[2].y,norm[2].z,

		norm[3].x,norm[3].y,norm[3].z,
		norm[3].x,norm[3].y,norm[3].z,
		norm[3].x,norm[3].y,norm[3].z,

		norm[4].x,norm[4].y,norm[4].z,
		norm[4].x,norm[4].y,norm[4].z,
		norm[4].x,norm[4].y,norm[4].z,

		norm[5].x,norm[5].y,norm[5].z,
		norm[5].x,norm[5].y,norm[5].z,
		norm[5].x,norm[5].y,norm[5].z,
	};
	int ind[18] = {
		0,1,4,
		1,2,4,

		0,1,3,
		1,2,3,
		//2,3,4,
		//3,0,4,
		//1,0,3,
		//1,3,2,
	};
	for (int i = 0; i < 54; i++) {
		cout << nor[i] << endl;
	}
	glBindVertexArray(vao[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions2), pyramidPositions2, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[2]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);
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
	cout << pvalues.size() << endl;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);//useless?
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
	cout << nvalues.size() << endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);//useless?
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void setup_sphere() {

	std::vector<glm::vec3> vert = mySphere.getVertices(); //顶点
	std::vector<glm::vec3> norm = mySphere.getNormals(); //法向量
	std::vector<int> ind = mySphere.getIndices(); //
	//getIndices()
	
	//——————————球1（粉色的）——————
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

void setup_sphere_light() {

	std::vector<glm::vec3> vert = mySphere2.getVertices(); //顶点
	std::vector<glm::vec3> norm = mySphere2.getvers_Normals(); //法向量
	std::vector<int> ind = mySphere2.getIndices(); //
	//getIndices()

	//——————————球1（粉色的）——————
//绑定当前VAO
	glBindVertexArray(vao[2]);
	//Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);/****顶点****/
	glBufferData(GL_ARRAY_BUFFER, vert.size() * 4 * sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);/****法向量****/
	glBufferData(GL_ARRAY_BUFFER, norm.size() * 4 * sizeof(GLfloat), &norm[0], GL_STATIC_DRAW);
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

void init(GLFWwindow* window) 
{
	//renderingProgram1 = Utils::createShaderProgram("./BlinnPhongShaders/vertShader.glsl", "./BlinnPhongShaders/fragShader.glsl");
	//renderingProgram1 = Utils::createShaderProgram("./GouraudShaders/vertShader.glsl", "./GouraudShaders/fragShader.glsl");
	renderingProgram1 = Utils::createShaderProgram("./PhongShaders/vertShader.glsl", "./PhongShaders/fragShader.glsl");

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
	setup_sphere();//球
	setup_sphere_light();
	setupVertices_pri();
	setup_cube();
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
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//mMat = glm::mat4(1.0f);
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	pMat = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	/************************************绘制圆环*********************************/
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
	/************************************绘制测试实体*********************************/
	//glBindVertexArray(vao[3]); //测试立方体
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(vao[4]);//测试四棱锥
	//glDrawArrays(GL_TRIANGLES, 0, 18);
	/*************绘制场景中的球，用自己的坐标进行设置（圆环上的小点）***************/
	ruby_material();
	installLights(vMat);

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.13f / SCALE, -0.19f / SCALE, 0.06f / SCALE));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, numTorusIndices2*3, GL_UNSIGNED_INT, 0);
	/***************圆环上的小点2，用自己的坐标进行设置*************************/
	jade_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.42f / SCALE, 0.21f / SCALE, -0.46f / SCALE));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, numTorusIndices2 * 3, GL_UNSIGNED_INT, 0);
	/***************圆环上的小点3，用自己的坐标进行设置*************************/
	ruby_material();
	installLights(vMat);
	
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(-0.14f / SCALE, 0.34f / SCALE, -0.66f / SCALE));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, numTorusIndices2 * 3, GL_UNSIGNED_INT, 0);
	/***************圆环上的小点4，用自己的坐标进行设置*************************/
	pearl_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(-0.42f / SCALE, 0.21f / SCALE, -0.46f / SCALE));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, numTorusIndices2 * 3, GL_UNSIGNED_INT, 0);
	/***************树干立方体，用自己的坐标进行设置*************************/
	bronze_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f,0.0f,0.0f));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[3]); //测试立方体
	glDrawArrays(GL_TRIANGLES, 0, 36);
	/***************树叶四棱锥1，用自己的坐标进行设置*************************/
	emerald_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.16f, 0.3f, 0.09f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	/***************树叶四棱锥2，用自己的坐标进行设置*************************/
	emerald_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.13f, 0.2f, 0.04f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.38f, 0.38f, 0.38f));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	/***************树叶四棱锥3，用自己的坐标进行设置*************************/
	emerald_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 0.0f));
	mMat *= glm::translate(glm::mat4(1.0f), glm::vec3(0.11f, 0.1f, 0.00f));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.45f, 0.45f, 0.45f));
	mMat *= glm::rotate(mMat, toRadians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	invTrMat = glm::transpose(glm::inverse(mvMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glBindVertexArray(vao[4]);//测试四棱锥
	glDrawArrays(GL_TRIANGLES, 0, 18);
	/***************绘制光源处的球，要用光的坐标进行设置*************************/
	gold_material();
	installLights(vMat);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(currentLightPos.x, currentLightPos.y
		, currentLightPos.z));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(vao[2]);
	glDrawElements(GL_TRIANGLES, numTorusIndices3, GL_UNSIGNED_INT, 0);
	
	
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