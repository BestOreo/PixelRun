#pragma once
#ifndef LIGHT_H_
#define LIGHT_H_

#include "Shader.h"

#include <string>
#include <sstream>
using namespace std;
using namespace glm;
enum LightType {
	_Directional,
	_Point,
};
class Light
{
public:
	static int index;
	LightType type;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool enabled;
	
	std::stringstream NameStr;
	Light(LightType lightType):type(lightType), enabled(false)
	{
		index++;
		ambient = vec3(0.8, 0.8, 0.8);
		diffuse = vec3(0.8, 0.8, 0.8);
		specular = vec3(0);
	}
	
	static void init(Shader* shader, vec3 viewPos, float shininess = 32.0f, bool blinn = true) {
		GLint viewPosLoc = glGetUniformLocation(shader->programID, "viewPos");
		glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
		// Set material properties
		glUniform1f(glGetUniformLocation(shader->programID, "material.shininess"), shininess);
		glUniform1i(glGetUniformLocation(shader->programID, "blinn"), blinn);
		for (int i = 0; i < 10; i++) {
			std::stringstream str;
			str << "pointLights[" << i << "].enabled";
			glUniform1i(glGetUniformLocation(shader->programID, str.str().c_str()), false);
		}
		for (int i = 0; i < 10; i++) {
			std::stringstream str;
			str << "dirLights[" << i << "].enabled";
			glUniform1i(glGetUniformLocation(shader->programID, str.str().c_str()), false);
		}
	}
	virtual void Use(Shader* shader) {
		
	}
	virtual void setAmbient(vec3 ambient) {
		this->ambient = ambient;

	}
	virtual void setDiffuse(vec3 diffuse) {
		this->diffuse = diffuse;
	}
	virtual void setSpecular(vec3 specular) {
		this->specular = specular;
	}
	virtual void setConstant(float constant) {
		
	}
	virtual void setLinear(float linear) {
		 
	}
	virtual void setQuadratic(float quadratic) {
		
	}
	virtual void setPosition(vec3 position) {

	}
	virtual void setDirection(vec3 direction) {

	}
private:
	
};


#endif // !LIGHT_H_
