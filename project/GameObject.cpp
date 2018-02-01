#include "GameWorld.h"

void GameObject::SetNewShader(Shader *s) {
    if (s == nullptr) {
        return;
    }

    if (shader == nullptr) {
        shader = s;
    }
    else
    {
        if (shader == GameWorld::Instance->defaultShader) {
            shader = s;
        }
        else
        {
            delete shader;
            shader = s;
        }
    }
}
