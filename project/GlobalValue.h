#ifndef _GLOBAL_VALUE_H_
#define _GLOBAL_VALUE_H_

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

class Global {
public:
    static GLFWwindow* window;
    static GLuint WIN_WIDTH, WIN_HEIGHT;
};

#endif