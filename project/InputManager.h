#pragma once
#include <iostream>
using namespace std;
#include <GLFW/glfw3.h>


class InputManager {
public:
	static bool forbid[4];
	static bool keys[1024];
	static bool ReleasedKeys[1024];
	static double MousePosX;
	static double MousePosY;
	static double MouseScrollY;
	static bool firstMouse;

	static void Init();
	static void Tick();
	static void KeyPressed(int key);
	static void KeyReleased(int key);

	static void ResetMouseInfo();
	static void Deal_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

};


