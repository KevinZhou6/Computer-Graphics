#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Utils.h"

//��������
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

#define numVAOs 1
#define numVBOs 2
#define numEBOs 1

//ȫ�ֱ���
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint ebo[numEBOs];

GLuint vColorLoc;
GLuint vPositionLoc;
GLuint thetaLoc;

//�任����
GLuint axis = 0;
enum { xAxis = 0, yAxis, zAxis };
GLfloat theta[3] = { 0.0, 0.0, 0.0 };
GLfloat speed = 2.0;

///////////������//////////////
//������������飬ֱ������
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

//������ɫ������ʼ������
void init(GLFWwindow* window) 
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);
	glGenBuffers(numEBOs, ebo);

	//�����������
	//�󶨵�ǰVAO
	glBindVertexArray(vao[0]);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);	

	// Load the vertex data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the index into the GPU  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	thetaLoc = glGetUniformLocation(renderingProgram, "theta");

	glEnable(GL_DEPTH_TEST); //������Ȳ���

}
//���ƺ���
void display(GLFWwindow* window, double currentTime) 
{
	glUseProgram(renderingProgram); 

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //������ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	theta[axis] += speed;
	glUniform3fv(thetaLoc, 1, theta);

	//����������
	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
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
	GLFWwindow* window = glfwCreateWindow(800, 800, "Cube", NULL, NULL);
	glfwMakeContextCurrent(window);

	//���ڸı��С�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//���ص�����
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//�����ص�����
	glfwSetKeyCallback(window, key_callback);

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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: //�������
			axis = xAxis;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE: //�н�����
			axis = yAxis;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT: //�Ҽ�����
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