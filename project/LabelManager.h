#pragma once
#include <vector>
#include <map>

// GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GLEW/glew.h>
#endif

// GLFW
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Render
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "MiniMath.h"
using namespace mmath;



class LabelManager {
public:
	int id = 0;
	bool visiable = 1;
	vec3 offset;
	int scale = 1;
	LabelManager(Shader *shader, GLuint textureId);
	void DrawNameLabel();
	void SetPositon(vec3 &positon);
	~LabelManager();
	bool IsScore = false;
	bool digit = 0;
	void UpdateScore();
private:

	Shader *shader;
	GLuint textureId;
	GLuint VAO;
	vec3 Position;
};

class ScoreManager {
public:
	LabelManager *Digit0;
	LabelManager *Digit1;
	static int score;
	int scale = 1;
	ScoreManager();
};