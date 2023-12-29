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


//ȫ�ֱ���
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint vColorLoc, vPositionLoc;
int NumTimesToSubdivide=4; //ϸ�ִ���
std::vector<glm::vec3> Points; //����
std::vector<glm::vec4> Colors; //��Ӧ����ɫ

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

//������ɫ������ʼ������
void init(GLFWwindow* window) 
{
	//���롢������ɫ������
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glEnable(GL_DEPTH_TEST); //������Ȳ���
	glClearColor(1.0, 1.0, 1.0, 0.0); //������ɫ

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	glm::vec3 v[4] = {
		{ 0.0, 0.0, -1.0 },
		{ 0.0, 0.942809, 0.33333 },
		{ -0.816497, -0.471405, 0.333333 },
		{ 0.816497, -0.471405, 0.333333 } }; //ԭʼ������

	glm::vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
	glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };

	divide_triangle(v[0], v[1], v[2], red,NumTimesToSubdivide);
	divide_triangle(v[3], v[1], v[2], green, NumTimesToSubdivide);
	divide_triangle(v[0], v[1], v[3], blue, NumTimesToSubdivide);
	divide_triangle(v[0], v[3], v[2], black,NumTimesToSubdivide);

	int Num = Points.size();

	//�󶨵�ǰVAO
	glBindVertexArray(vao[0]);
	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, Num * 3 * sizeof(GLfloat), &Points[0], GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	vPositionLoc = glGetAttribLocation(renderingProgram, "vPosition");
	glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionLoc);

	// Load the data into the GPU  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, Num * 4 * sizeof(GLfloat), &Colors[0], GL_STATIC_DRAW);
	//��VBO������������ɫ������Ӧ�Ķ�������
	GLuint vColorLoc = glGetAttribLocation(renderingProgram, "vColor");
	glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorLoc);

}
//��������
void display(GLFWwindow* window, double currentTime) 
{
	glUseProgram(renderingProgram); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//����ģ��
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, Points.size());
}

int main(void) 
{
	//glfw��ʼ��
	if (!glfwInit()) 
	{ 
		exit(EXIT_FAILURE); 
	}
	//���ڰ汾
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //�ΰ汾��

	//��������
	GLFWwindow* window = glfwCreateWindow(800, 800, "simple", NULL, NULL);
	glfwMakeContextCurrent(window);
	//���û���ˢ��ʱ��
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)//glew��ʼ��
	{ 
		exit(EXIT_FAILURE);
	}

	//������ɫ���������Ͱ�VAO��VBO
	init(window);

	//�¼�ѭ�������������¼�
	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime()); //���ƺ���������
		glfwSwapBuffers(window); //������ɫ����
		glfwPollEvents(); // �����û�д���ʲô�¼�������������롢����ƶ��ȣ�
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}