#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Utils
{
private:
	static std::string readShaderFile(const char *filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(int prog);
	static GLuint prepareShader(int shaderTYPE, const char *shaderPath);
	static int finalizeShaderProgram(GLuint sprogram);

public:
	Utils();
	static bool checkOpenGLError();
	static GLuint createShaderProgram(const char *vp, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *gp, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, char *gp, const char *fp);
	static GLuint loadTexture(const char *texImagePath);
	static GLuint loadCubeMap(const char *mapDir);

	static float* goldAmbient();
	static float* goldDiffuse();
	static float* goldSpecular();
	static float goldShininess();

	static float* silverAmbient();
	static float* silverDiffuse();
	static float* silverSpecular();
	static float silverShininess();

	static float* bronzeAmbient();
	static float* bronzeDiffuse();
	static float* bronzeSpecular();
	static float bronzeShininess();

	// YELLOW_R material - ambient, diffuse, specular, and shininess
	static float* yellowAmbient();
	static float* yellowDiffuse();
	static float* yellowSpecular();
	static float yellowShininess();

	static float* pearlAmbient();
	static float* pearlDiffuse();
	static float* pearlSpecular();
	static float pearlShininess();

	static float* jadeAmbient();
	static float* jadeDiffuse();
	static float* jadeSpecular();
	static float jadeShininess();

	static float* rubyAmbient();
	static float* rubyDiffuse();
	static float* rubySpecular();
	static float rubyShininess();

	static float* emeraldAmbient();
	static float* emeraldDiffuse();
	static float* emeraldSpecular();
	static float emeraldShininess();

	static float* regularAmbient();
	static float* regularDiffuse();
	static float* regularSpecular();
	static float regularShininess();
};