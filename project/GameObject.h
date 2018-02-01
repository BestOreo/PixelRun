#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Transform.h"
#include "Model.h"
#include "Behaviour.h"
#include "Tags.h"
#include "Physicis.h"

using namespace std;
using namespace glm;
using namespace phy;

class GameObject
{
public:
    string name = "GO";
    TAG tag = _Default;
    Transform *trans;
    vec3 velocity = vec3(0);

    Behaviour *behaviour = nullptr;
    Collider *col = nullptr;

    Model *model = nullptr;
    Shader *shader = nullptr;

    ~GameObject()
    {
        if (trans != nullptr) delete trans;
        if (model != nullptr) delete model;
        if (shader != nullptr) delete shader;
        if (behaviour != nullptr) {
            behaviour->OnDestory();
            delete behaviour;
        }
        if (col != nullptr) delete col;
    }

    virtual bool BindBehaviour(Behaviour *beha) {
        if (this->behaviour != nullptr || beha == nullptr)
            return false;

        this->behaviour = beha;
        this->behaviour->obj = this;

        if (col != nullptr) {
            col->beha = behaviour;
        }

        this->behaviour->Start();

        if (active == false) {
            this->behaviour->OnDisable();
        }

        return true;
    }

    virtual bool AddCollider(Collider *col) {
        if (col == nullptr) {
            return false;
        }

        if (this->col != nullptr) {
            delete this->col;
        }

        this->col = col;

        col->obj = this;
        col->beha = behaviour;
        col->trans = trans;

        return true;
    }

    virtual bool GetActive() { return active; }
    virtual void SetActive(bool value) {
        if (value == active) return;

        active = value;
        if (behaviour != nullptr) {
            if (active) behaviour->OnEnable();
            else behaviour->OnDisable();
        }
    }

    virtual void SetNewShader(Shader *s);

    friend class GameWorld;

private:
    bool active = true;

    GameObject(vec3 position)
    {
        trans = new Transform(this, position);
        model = nullptr;
        shader = nullptr;
    }

    GameObject(vec3 position, const string& modelPath,
        Shader *s)
    {
        trans = new Transform(this, position);
        model = new Model();
        shader = s;
        model->loadModel(modelPath);
    }

    GameObject(vec3 position, const string& modelPath,
        const string& shaderVPath, const string& shaderFPath)
    {
        trans = new Transform(this, position);
        model = new Model();
        shader = new Shader(shaderVPath.c_str(), shaderFPath.c_str());
        model->loadModel(modelPath);
    }

    virtual void Draw(mat4& projection, mat4& view) {
        if (model == nullptr) {
            return;
        }

        if (behaviour != nullptr) {
            if (behaviour->BeforeDraw() == true) {
                return;
            }
        }

        shader->Use();
        glUniformMatrix4fv(glGetUniformLocation(shader->programID, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader->programID, "view"),
            1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader->programID, "model"),
            1, GL_FALSE, value_ptr(trans->GetModelMat()));

        model->draw(*(shader));

        if (behaviour != nullptr) {
            behaviour->AfterDraw();
        }
    }
};



#endif 