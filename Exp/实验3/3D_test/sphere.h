#pragma once
#include <cmath>
#include <vector>
#include <glm\glm.hpp>
class Sphere
{
private:
	int numVertices;
	int numIndices;
	int prec; //精度
	float r;//半径
	float outer;
	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> colors; //对应的颜色
	std::vector<glm::vec3> vers_normals;

	void init();
	float toRadians(float degrees);

public:
	Sphere();
	Sphere(float r, int prec);
	int getNumVertices();
	int getNumIndices();
	std::vector<int> getIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec4> getColors();
	std::vector<glm::vec3> getvers_Normals();
};