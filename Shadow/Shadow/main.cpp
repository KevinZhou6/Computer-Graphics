#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include "glm\glm.hpp"
#include <vector>

//��������
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

#define numVAOs 1
#define numVBOs 1
#define PI 3.14159f

//ȫ�ֱ���
GLuint renderingProgram;
GLuint vbo[numVBOs];
GLuint vao[numVAOs];

std::vector<glm::vec4> pointsArray;

GLfloat fNear = -4.0f;
GLfloat fFar = 4.0f;
GLfloat fLeft = -2.0;
GLfloat fRight = 2.0;
GLfloat fTop = 2.0;
GLfloat fBottom = -2.0;

GLfloat theta = 0.0f;
glm::mat4 modelViewMatrix, projectionMatrix;
GLuint modelViewMatrixLoc, projectionMatrixLoc;
GLuint fColorLoc;

glm::vec3 eye, at, up;
glm::vec3 light;
glm::mat4 m;
glm::vec4 red;
glm::vec4 black;

//������ɫ������ʼ������
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	//������
	light = glm::vec3(0.0, 2.0, 0.0);

	// matrix for shadow projection
	//ͶӰ����
	//���Ҫע��glm��洢�ǰ��еģ���OpenGL�����ǰ���չ����
	m = glm::mat4(1.0);
	m[3][3] = 0;
	m[1][3] = -1 / light[1]; 

	at = glm::vec3(0.0, 0.0, 0.0);
	up = glm::vec3(0.0, 1.0, 0.0);
	eye = glm::vec3(1.0, 1.0, 1.0);

	// color square red and shadow black
	red = glm::vec4(1.0, 0.0, 0.0, 1.0);
	black = glm::vec4(0.0, 0.0, 0.0, 1.0);

	// square
	pointsArray.push_back(glm::vec4(-0.5, 0.5, -0.5, 1.0));
	pointsArray.push_back(glm::vec4(-0.5, 0.5, 0.5, 1.0));
	pointsArray.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0));
	pointsArray.push_back(glm::vec4(0.5, 0.5, -0.5, 1.0));

	//
	//  Load shaders and initialize attribute buffers
	//
	glUseProgram(renderingProgram);
	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	//�󶨵�ǰVAO
	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pointsArray.size() * 4 * sizeof(GLfloat), &pointsArray[0], GL_STATIC_DRAW);

	//��VBO������������ɫ������Ӧ�Ķ�������
	GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	//��ȡuniform������ַ
	fColorLoc = glGetUniformLocation(renderingProgram, "fColor");
	modelViewMatrixLoc = glGetUniformLocation(renderingProgram, "modelViewMatrix");
	projectionMatrixLoc = glGetUniformLocation(renderingProgram, "projectionMatrix");

	projectionMatrix = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glEnable(GL_DEPTH_TEST); //������Ȳ���
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //������ɫ

}
//���ƺ���
void display(GLFWwindow* window, double currentTime) 
{
	glUseProgram(renderingProgram);
	glBindVertexArray(vao[0]);

	theta += 0.1f;
	if (theta > 2 * PI) theta -= 2 * PI;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// model-view matrix for square
	modelViewMatrix = glm::lookAt(eye, at, up);

	// send color and matrix for square then render

	glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniform4fv(fColorLoc, 1, glm::value_ptr(red));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// rotate light source

	light[0] = glm::sin(theta);
	light[2] = glm::cos(theta);

	// model-view matrix for shadow then render
	//glm�˷������ǰ���OpenGL�Ĺ���
	modelViewMatrix = modelViewMatrix * glm::translate(glm::mat4(1.0), light) * m * glm::translate(glm::mat4(1.0), -light);

	// send color and matrix for shadow
	glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniform4fv(fColorLoc, 1, glm::value_ptr(black));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main(void) 
{
	//glfw��ʼ��
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//���ڰ汾
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//��������
	GLFWwindow* window = glfwCreateWindow(800, 800, "Shadow", NULL, NULL);
	glfwMakeContextCurrent(window);

	//���ڸı��С�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//glew��ʼ��
	if (glewInit() != GLEW_OK)
	{ 
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);

	//������ɫ���������Ͱ�VAO��VBO
	init(window);

	//�¼�ѭ��
	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window); //�л�ǰ�󻺴�
		glfwPollEvents();
	}

	glfwDestroyWindow(window); //���ٴ��ڶ��󼰹�����OpenGL����
	glfwTerminate(); //�ر�glfw��
	exit(EXIT_SUCCESS);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

