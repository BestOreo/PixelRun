#pragma once

// Std. Includes
#include <string>
#include <algorithm>
using namespace std;

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
#include "Camera.h"
#include "Model.h"

class SkyBox
{
public:
	SkyBox()
	{
		shader = new Shader("_shader/skybox.vs", "_shader/skybox.fs");

		// Set the object data (buffers, vertex attributes)
		GLfloat skyboxVertices[] = {
			// Positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		// Setup skybox VAO
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);

		// Cubemap (Skybox)
		vector<const GLchar*> faces;

        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/right.png");
        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/left.png");
        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/up.png");
        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/down.png");
        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/back.png");
        // faces.push_back("_skybox/Cartoon_Airbrush_Day_NoSun/front.png");

        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/right.png");
        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/left.png");
        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/up.png");
        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/down.png");
        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/back.png");
        faces.push_back("_skybox/Cartoon_BlueSkyPainted_NoSun/front.png");

		// faces.push_back("_skybox/Space_Nebula_Green/right.png");
		// faces.push_back("_skybox/Space_Nebula_Green/left.png");
		// faces.push_back("_skybox/Space_Nebula_Green/up.png");
		// faces.push_back("_skybox/Space_Nebula_Green/down.png");
		// faces.push_back("_skybox/Space_Nebula_Green/back.png");
		// faces.push_back("_skybox/Space_Nebula_Green/front.png");

		cubemapTexture = loadCubemap(faces);
	}

	void Draw(Camera& camera, GLuint WIN_WIDTH, GLuint WIN_HEIGHT) {
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		shader->Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader->programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->programID, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
	}

	~SkyBox()
	{
		if (shader != NULL) delete shader;
	}

private:
	Shader *shader;
	GLuint vao, vbo;
	GLuint cubemapTexture;

	// Loads a cubemap texture from 6 individual texture faces
	// Order should be:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front)
	// -Z (back)
	GLuint loadCubemap(vector<const GLchar*> faces)
	{
		GLuint textureID;
		glGenTextures(1, &textureID);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for (GLuint i = 0; i < faces.size(); i++)
		{
			image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textureID;
	}

	// This function loads a texture from file. Note: texture loading functions like these are usually
	// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
	// For learning purposes we'll just define it as a utility function.
	GLuint loadTexture(GLchar* path)
	{
		//Generate texture ID and load texture data
		GLuint textureID;
		glGenTextures(1, &textureID);
		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		// Assign texture to ID
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);
		return textureID;
	}
};


