#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include "glm\glm.hpp"
#include <vector>

//函数声明
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

#define numVAOs 1
#define numVBOs 1
#define PI 3.14159f

//全局变量
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

//导入着色器，初始化窗口
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	//各参数
	light = glm::vec3(0.0, 2.0, 0.0);

	// matrix for shadow projection
	//投影矩阵
	//这边要注意glm库存储是按行的，而OpenGL矩阵是按列展开的
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

	//绑定当前VAO
	glBindVertexArray(vao[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pointsArray.size() * 4 * sizeof(GLfloat), &pointsArray[0], GL_STATIC_DRAW);

	//将VBO关联给顶点着色器中相应的顶点属性
	GLuint vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	//获取uniform变量地址
	fColorLoc = glGetUniformLocation(renderingProgram, "fColor");
	modelViewMatrixLoc = glGetUniformLocation(renderingProgram, "modelViewMatrix");
	projectionMatrixLoc = glGetUniformLocation(renderingProgram, "projectionMatrix");

	projectionMatrix = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glEnable(GL_DEPTH_TEST); //启用深度测试
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //背景颜色

}
//绘制函数
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
	//glm乘法规则是按照OpenGL的规则
	modelViewMatrix = modelViewMatrix * glm::translate(glm::mat4(1.0), light) * m * glm::translate(glm::mat4(1.0), -light);

	// send color and matrix for shadow
	glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	glUniform4fv(fColorLoc, 1, glm::value_ptr(black));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int main(void) 
{
	//glfw初始化
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//窗口版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//创建窗口
	GLFWwindow* window = glfwCreateWindow(800, 800, "Shadow", NULL, NULL);
	glfwMakeContextCurrent(window);

	//窗口改变大小的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//glew初始化
	if (glewInit() != GLEW_OK)
	{ 
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);

	//导入着色器，创建和绑定VAO和VBO
	init(window);

	//事件循环
	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window); //切换前后缓存
		glfwPollEvents();
	}

	glfwDestroyWindow(window); //销毁窗口对象及关联的OpenGL环境
	glfwTerminate(); //关闭glfw库
	exit(EXIT_SUCCESS);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

