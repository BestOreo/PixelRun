#include "Camera.h"
#include "GameManager.h"

void Camera::Movement(GLfloat deltaTime) {
	// 摄像机控制
	if (glfwGetInputMode(Global::window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
		GLfloat cameraSpeed = 0.01f;
		if (InputManager::keys[GLFW_KEY_W])
			ProcessKeyboard(FORWARD, deltaTime);
		if (InputManager::keys[GLFW_KEY_S])
			ProcessKeyboard(BACKWARD, deltaTime);
		if (InputManager::keys[GLFW_KEY_A])
			ProcessKeyboard(LEFT, deltaTime);
		if (InputManager::keys[GLFW_KEY_D])
			ProcessKeyboard(RIGHT, deltaTime);
		if (InputManager::keys[GLFW_KEY_Q])
			ProcessKeyboard(DOWN, deltaTime);
		if (InputManager::keys[GLFW_KEY_E])
			ProcessKeyboard(UP, deltaTime);
	}
	else {
        if (GameManager::player != nullptr) {
            // 相机跟随人物移动
            this->Position[0] = GameManager::player->obj->trans->position[0];
            this->Position[2] = GameManager::player->obj->trans->position[2] + 30;
            /*if (this->Position[2] - GameManager::player->obj->trans->position[2] < 25) {
                this->Position[2] += GameManager::player->speed * deltaTime;
            }
            if (this->Position[2] - GameManager::player->obj->trans->position[2] > 40) {
                this->Position[2] -= GameManager::player->speed * deltaTime;
            }*/
        }
    }
}