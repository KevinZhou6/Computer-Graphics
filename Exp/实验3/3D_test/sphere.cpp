#include <cmath>
#include <vector>
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>
#include "Sphere.h"
using namespace std;


const float PI = 3.14159265358979323846f;
//这里是封装了一个球的类
Sphere::Sphere() {
	prec = 48; //精度
	r = 0.5f;
	init();
}

Sphere::Sphere(float rIn, int precIn)
{
	//构造函数
	prec = precIn;
	r = rIn;
	init();
}

float Sphere::toRadians(float degrees)
{
	//将度数转换为弧度
	return (degrees * 2.0f * 3.14159f) / 360.0f;
}

void Sphere::init()
{
	numVertices = (prec + 1) * (prec + 1);//计算顶点数
	numIndices = prec * prec * 6;//计算体数
	glm::vec4 light_y2 = { 0.984f, 0.973f, 0.819f, 1.0f };
	for (int y = 0; y <= prec; y++)
	{
		for (int x = 0; x <= prec; x++)
		{
			float xSegment = (float)x / (float)prec;
			float ySegment = (float)y / (float)prec;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			//vertices.push_back(glm::vec3(r*xPos, r*yPos, r*zPos));
			vertices.push_back(glm::vec3(r*xPos, r*zPos, r*yPos));
			//normals.push_back(glm::vec3(r*xPos, r*yPos, r*zPos));
			normals.push_back(glm::vec3(r*xPos, r*zPos, r*yPos));
			vers_normals.push_back(glm::vec3(-r*xPos, -r*zPos, -r*yPos));
		}
	}
	for (int i = 0; i < prec; i++)
	{
		for (int j = 0; j < prec; j++)
		{
			indices.push_back(i * (prec + 1) + j);
			indices.push_back((i + 1) * (prec + 1) + j);
			indices.push_back((i + 1) * (prec + 1) + j + 1);
			indices.push_back(i* (prec + 1) + j);
			indices.push_back((i + 1) * (prec + 1) + j + 1);
			indices.push_back(i * (prec + 1) + j + 1);
		}
	}

}
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
std::vector<int> Sphere::getIndices() { return indices; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }
std::vector<glm::vec4> Sphere::getColors() { return colors; }
std::vector<glm::vec3> Sphere::getvers_Normals() {return vers_normals;}