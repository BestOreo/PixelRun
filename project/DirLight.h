#pragma once
#ifndef DIRLIGHT_H_
#define DIRLIGHT_H_
#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class DirLight : public Light 
{
public:
	static int DirIndex;
	DirLight(glm::vec3 dir):Light(_Directional),direction(dir){
		
		NameStr << "dirLights[" << DirIndex++ << "]";
		cout << NameStr.str() << " initialed!" << endl;
	}
	void Use(Shader* _shader) {
		string str = NameStr.str() + ".enabled";
		glUniform1i(glGetUniformLocation(_shader->programID, str.c_str()), true);
		str = NameStr.str() + ".direction";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), direction.x, direction.y, direction.z);
		str = NameStr.str() + ".ambient";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), ambient.x, ambient.y, ambient.z);
		str = NameStr.str() + ".diffuse";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), diffuse.x, diffuse.y, diffuse.z);
		str = NameStr.str() + ".specular";
		glUniform3f(glGetUniformLocation(_shader->programID, str.c_str()), specular.x, specular.y, specular.z);
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
	void setDirection(glm::vec3 direction) {
		this->direction = direction;
	}
private:
	glm::vec3 direction;
};

#endif // !DIRLIGHT_H_

