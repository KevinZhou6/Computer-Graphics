#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"
#include <glm\glm.hpp>
#include <vector>
using namespace std;

#define numVAOs 1
#define numVBOs 2


//全局变量
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint vColorLoc, vPositionLoc;
int NumTimesToSubdivide=4; //细分次数
std::vector<glm::vec3> Points; //顶点
std::vector<glm::vec4> Colors; //对应的颜色

void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 CurColor)
/* specify one triangle */
{
	Colors.push_back(CurColor);
	Points.push_back(a);
	Colors.push_back(CurColor);
	Points.push_back(b);
	Colors.push_back(CurColor);
	Points.push_back(c);
}

void divide_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 CurColor, int m)
{

	/* triangle subdivision using vertex numbers */
	glm::vec3 v0, v1, v2;
	if (m > 0)
	{
		v0.x = (a.x + b.x) / 2.0;
		v0.y = (a.y + b.y) / 2.0;
		v0.z = (a.z + b.z) / 2.0;

		v1.x = (a.x + c.x) / 2.0;
		v1.y = (a.y + c.y) / 2.0;
		v1.z = (a.z + c.z) / 2.0;

		v2.x = (b.x + c.x) / 2.0;
		v2.y = (b.y + c.y) / 2.0;
		v2.z = (b.z + c.z) / 2.0;

		divide_triangle(a, v0, v1, CurColor, m - 1);
		divide_triangle(c, v1, v2, CurColor, m - 1);
		divide_triangle(b, v2, v0, CurColor, m - 1);
	}
	else
		triangle(a, b, c, CurColor); /* put points to the vertex list at end of recursion */
}

//导入着色器，初始化窗口
void init(GLFWwindow* window) 
{
	//编译、链接着色器程序
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glEnable(GL_DEPTH_TEST); //启用深度测试
	glClearColor(1.0, 1.0, 1.0, 0.0); //背景颜色

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	glm::vec3 v[4] = {
		{ 0.0, 0.0, -1.0 },
		{ 0.0, 0.942809, 0.33333 },
		{ -0.816497, -0.471405, 0.333333 },
		{ 0.816497, -0.471405, 0.333333 } }; //原始四面体

	glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };

	divide_triangle(v[0], v[1], v[2], red,NumTimesToSubdivide);
	divide_triangle(v[3], v[1], v[2], green, NumTimesToSubdivide);
	divide_triangle(v[0], v[1], v[3], blue, NumTimesToSubdivide);
	divide_triangle(v[0], v[3], v[2], black,NumTimesToSubdivide);

	int Num = Points.size();

	//绑定当前VAO
	glBindVertexArray(vao[0]);
	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, Num * 3 * sizeof(GLfloat), &Points[0], GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, Num * 4 * sizeof(GLfloat), &Colors[0], GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);

}
//函数绘制
void display(GLFWwindow* window, double currentTime) 
{
	glUseProgram(renderingProgram); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//绘制模型
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, Points.size());
}

int main(void) 
{
	//glfw初始化
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//窗口版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //次版本号

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(800, 800, "simple", NULL, NULL);
	glfwMakeContextCurrent(window);
	//设置缓存刷新时间
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)//glew初始化
	{ 
		exit(EXIT_FAILURE);
	}

	//导入着色器，创建和绑定VAO和VBO
	init(window);

	//事件循环，接收输入事件
	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime()); //绘制函数，主体
		glfwSwapBuffers(window); //交换颜色缓存
		glfwPollEvents(); // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}