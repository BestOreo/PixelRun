#include <GLFW/glfw3.h>
#include "InputManager.h"


void InputManager::Init() {
	cout << "in:init" << endl;
	for (int i = 0; i < 1024; i++) {
		keys[i] = false;
		ReleasedKeys[i] = true;
	}
	MousePosX = 0.0;
	MousePosY = 0.0;
	MouseScrollY = 0.0;
	firstMouse = true;
}

void InputManager::Tick() {
	MouseScrollY = 0.0;
}

void InputManager::KeyPressed(int key) {
	keys[key] = true;
}

void InputManager::KeyReleased(int key) {
	keys[key] = false;
	ReleasedKeys[key] = true;
}

// Solve the bug of repressing KEY_ALT
void InputManager::ResetMouseInfo() {
	firstMouse = true;
}

void InputManager::Deal_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_RELEASE) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			ResetMouseInfo();	// Aim to solve the bug of repressing KEY_ALT
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (action == GLFW_PRESS) {
		InputManager::KeyPressed(key);
		/*if (InputManager::keys[key] == true) {
		cout << key << " pressed" << endl;
		}*/
	}
	else if (action == GLFW_RELEASE) {
		InputManager::KeyReleased(key);
		/*if (InputManager::keys[key] == false) {
		cout << key << " released" << endl;
		}*/
	}
}