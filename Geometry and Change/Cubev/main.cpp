#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"

//函数声明
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

#define numVAOs 1
#define numVBOs 2
#define numEBOs 1

//全局变量
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

GLuint vColorLoc;
GLuint vPositionLoc;
GLuint thetaLoc;

//变换参数
GLuint axis = 0;
enum { xAxis = 0, yAxis, zAxis };
GLfloat theta[3] = { 0.0, 0.0, 0.0 };
GLfloat speed = 2.0;

///////////立方体//////////////
//立方体相关数组，直接设置
float vertices[24] = {
	-0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, -0.5, 0.5,
	-0.5, -0.5, -0.5,
	-0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, -0.5, -0.5
};

float vertexColors[32] = {
	0.0, 0.0, 0.0, 1.0,  // black
	1.0, 0.0, 0.0, 1.0,  // red
	1.0, 1.0, 0.0, 1.0,  // yellow
	0.0, 1.0, 0.0, 1.0,  // green
	0.0, 0.0, 1.0, 1.0,  // blue
	1.0, 0.0, 1.0, 1.0,  // magenta
	1.0, 1.0, 1.0, 1.0,  // white
	0.0, 1.0, 1.0, 1.0   // cyan
};

// indices of the 12 triangles that compise the cube

GLubyte indices[36] = {
	1, 0, 3,
	3, 2, 1,
	2, 3, 7,
	7, 6, 2,
	3, 0, 4,
	4, 7, 3,
	6, 5, 1,
	1, 2, 6,
	4, 5, 6,
	6, 7, 4,
	5, 4, 0,
	0, 1, 5
};

//导入着色器，初始化窗口
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);
	glGenBuffers(numEBOs, ebo);

	//立方体各参数
	//绑定当前VAO
	glBindVertexArray(vao[0]);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);	

	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//将VBO关联给顶点着色器中相应的顶点属性
	vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the index into the GPU  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	thetaLoc = glGetUniformLocation(renderingProgram, "theta");

	glEnable(GL_DEPTH_TEST); //启用深度测试

}
//绘制函数
void display(GLFWwindow* window, double currentTime) 
{
	glUseProgram(renderingProgram); 

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //背景颜色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	theta[axis] += speed;
	glUniform3fv(thetaLoc, 1, theta);

	//绘制立方体
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
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
	GLFWwindow* window = glfwCreateWindow(800, 800, "Cube", NULL, NULL);
	glfwMakeContextCurrent(window);

	//窗口改变大小的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//鼠标回调函数
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//按键回调函数
	glfwSetKeyCallback(window, key_callback);

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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: //左键按下
			axis = xAxis;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE: //中健按下
			axis = yAxis;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT: //右键按下
			axis = zAxis;
			break;
		default:
			break;
		}
	}
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch(key)
	{
	case GLFW_KEY_A:
		if (speed > 180.0)
			speed = 0.0;
		speed += 0.5;
		break;
	case GLFW_KEY_S:
		if (speed < -180.0)
			speed = 0.0;
		speed -= 0.5;
		break;
	default:
		break;
	}
}