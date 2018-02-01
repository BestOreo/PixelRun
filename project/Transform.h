#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GlobalValue.h"

using namespace glm;

class GameObject;

class Transform
{
public:
	// relative, i.e. local
	vec3 position;
	vec3 rotation;
	vec3 scale;

	Transform *parent;
	GameObject *gameObject;

	mat4 GetModelMat() {
		mat4 result;
		result = glm::translate(result, position);
		result = glm::scale(result, scale);
		result = glm::rotate(result, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		result = glm::rotate(result, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		result = glm::rotate(result, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return result;
	}

	friend class GameObject;
private:
	Transform() = delete;

	Transform(GameObject *gameObject, vec3 position = vec3(0),
		vec3 rotation = vec3(0), vec3 scale = vec3(1))
	{
		this->gameObject = gameObject;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
	}
};



#endif