#pragma once
#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_
#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
class PointLight : public Light
{
public:
	static int PointIndex;
	PointLight(glm::vec3 pos) :Light(_Directional), position(pos) {
		
		NameStr << "pointLights[" << PointIndex++ << "]";
		//cover distance:100
		quadratic = 0.0075f;
		linear = 0.045;
		constant = 1.0f;
		cout << NameStr.str() << " initialed!" << endl;
	}
	
	void Use(Shader* _shader) {
		string str = NameStr.str() + ".enabled";
		glUniform1i(glGetUniformLocation(_shader->programID, str.c_str()), true);
		str = NameStr.str() + ".position";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), position.x, position.y, position.z);
		str = NameStr.str() + ".ambient";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), ambient.x, ambient.y, ambient.z);
		str = NameStr.str() + ".diffuse";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), diffuse.x, diffuse.y, diffuse.z);
		str = NameStr.str() + ".specular";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), specular.x, specular.y, specular.z);
		str = NameStr.str() + ".constant";
		glUniform1f(glGetUniformLocation(_shader->programID, str.c_str()), constant);
		str = NameStr.str() + ".linear";
		glUniform1f(glGetUniformLocation(_shader->programID, str.c_str()), linear);
		str = NameStr.str() + ".quadratic";
		glUniform1f(glGetUniformLocation(_shader->programID, str.c_str()), quadratic);

	}
	void setAmbient(glm::vec3 ambient) {
		this->ambient = ambient;
	}
	void setDiffuse(glm::vec3 diffuse) {
		this->diffuse = diffuse;
	}
	void setSpecular(glm::vec3 specular) {
		this->specular = specular;
	}
	void setConstant(float constant) {
		this->constant = constant;
	}
	void setLinear(float linear) {
		this->linear = linear;
	}
	void setQuadratic(float quadratic) {
		this->quadratic = quadratic;
	}
	void setPosition(glm::vec3 position) {
		this->position = position;
	}
	
private:
	
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
};

#endif // !POINTLIGHT_H_
