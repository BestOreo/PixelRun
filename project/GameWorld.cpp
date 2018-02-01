#include "GameWorld.h"
#include "GameManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

using namespace std;
using namespace glm;

void GameWorld::InitShadow()
{
    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    simpleDepthShader = new Shader("_shader/shadow_mapping_depth.vs", "_shader/shadow_mapping_depth.fs");

    // Configure depth map FBO
    glGenFramebuffers(1, &depthMapFBO);
    // - Create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
     //GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GameWorld::~GameWorld()
{
    size_t len = objects.size();
    for (size_t i = 0; i < len; i++) {
        auto obj = objects[i];
        if (obj != nullptr) {
            delete obj;
        }
    }

    Instance = nullptr;
    ins = nullptr;
}

void GameWorld::Init() {
    srand((unsigned)time(NULL));
    if (Instance == NULL) {
        Instance = new GameWorld();
        ins = Instance;

        ins->elapsedTime = 0;
        ins->startTime = (float)glfwGetTime();
        ins->lastFixedUpdateTime = glfwGetTime();
        ins->nextFixedUpdateTime = ins->lastFixedUpdateTime + ins->fixedDeltaTime;
        
        ins->defaultShader = new Shader(ins->DEFAULT_SHADER_V_PATH.c_str(),
            ins->DEFAULT_SAHDER_F_PATH.c_str());
        
        ins->lineDsvShader = new Shader("_shader/loadmodel.vs", "_shader/line_dsv.fs");
        ins->lineDsvTextId = TextureHelper::load2DTexture("_shader/disolve.png", TEXTURE_FILTER_TYPE_MAL_MILM);

        ins->dsvShader = new Shader("_shader/loadmodel ori.vs", "_shader/disolve.fs");
        ins->dsvTextId = TextureHelper::load2DTexture("_shader/disolve.png", TEXTURE_FILTER_TYPE_MAL_MILM);

        MainCamera = new Camera(vec3(10.0f, 10.0f, 30.0f));

        PhyManager::Init();
        InputManager::Init();
        InitShadow();
    }
}

void GameWorld::Draw() {
    glm::mat4 projection = glm::perspective(MainCamera->Zoom, (float) Global::WIN_WIDTH / (float) Global::WIN_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = MainCamera->GetViewMatrix();

    vec3 lightPos(0, 20.0f, -20.0f);
    lightPos += GameManager::player->obj->trans->position;

    glm::mat4 lightSpaceMatrix;
    //if (MainLight != nullptr)
    {
        // 1. Render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
        glm::mat4 lightProjection, lightView;
        GLfloat near_plane = 0.0f, far_plane = 300.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        //lightProjection = glm::perspective(45.0f, (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, 0.1f, 1000.0f);
        lightView = glm::lookAt(lightPos, GameManager::player->obj->trans->position, glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        // - render scene from light's point of view

        simpleDepthShader->Use();
        simpleDepthShader->SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        size_t len = ins->objects.size();
        GameObject *obj;
        for (size_t i = 0; i < len; i++) {
            obj = ins->objects[i];
            if (obj->GetActive() == false || obj->model == nullptr || obj->shader == nullptr) {
                continue;
            }

            simpleDepthShader->SetMatrix4("model", obj->trans->GetModelMat(), true);

            obj->model->draw(*(simpleDepthShader));
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Reset viewport
        glViewport(0, 0, Global::WIN_WIDTH, Global::WIN_WIDTH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    // Draw skybox first
    ins->skyBox->Draw(*MainCamera, Global::WIN_WIDTH, Global::WIN_HEIGHT);

    size_t len = ins->objects.size();
    GameObject *obj;
	Shader *shader;

    ins->defaultShader->Use();
    ins->defaultShader->SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
    ins->defaultShader->SetVector3f("lightPos", lightPos);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    ins->defaultShader->SetInteger("shadowMap", 2);

    ins->lineDsvShader->Use();
    ins->lineDsvShader->SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
    ins->lineDsvShader->SetVector3f("lightPos", lightPos);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    ins->lineDsvShader->SetInteger("shadowMap", 2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ins->lineDsvTextId);
    ins->lineDsvShader->SetInteger("texture_noise", 1);

    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
		shader = obj->shader;
        if (obj->GetActive() == false || obj->model == nullptr || obj->shader == nullptr) {
            continue;
        }

		shader->Use();
		GLfloat timeValue = glfwGetTime() / 4.0f;
		GLfloat lightValue = (0.6 * (sin(timeValue) / 2) + 0.7);
		//cout << lightValue<<endl;
		Light::init(shader, vec3(MainCamera->Position.x, MainCamera->Position.y, MainCamera->Position.z));
		for (int j = 0; j < ins->lights.size(); j++) {
			auto light = ins->lights[j];
			light->Use(shader);
			light->setAmbient(vec3(lightValue));
			light->setDiffuse(vec3(lightValue));
		}

        if (shader == ins->dsvShader) {
            ins->dsvShader->Use();
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, ins->dsvTextId);
            ins->dsvShader->SetInteger("texture_disolve", 3);
        }

        obj->Draw(projection, view);
    }
}

void GameWorld::FixedUpdate()
{
    float curTime = glfwGetTime();
    if (ins->nextFixedUpdateTime > curTime) {
        return;
    }
    ins->lastFixedUpdateTime = curTime;
    ins->nextFixedUpdateTime = curTime + ins->fixedDeltaTime;
    ins->elapsedTime += ins->fixedDeltaTime;

    // cout << "GameWorld::FixedUpdate" << endl;
    // cout << ins->elapsedTime << " seconds elapsed in game." << endl;
    // cout << GetElapsedTime(false) << " seconds elapsed in real world." << endl;
    // cout << "----------------------\n" << endl;


    size_t len = ins->objects.size();
    GameObject *obj;
    Behaviour *be;
    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
        be = obj->behaviour;

        if (obj->active) {
            obj->trans->position += obj->velocity * GameWorld::Instance->GetFixedDeltaTime();
            if (be != NULL) {
                be->FixedUpdate();
            }
        }
    }


    PhyManager::Tick();
    InputManager::Tick();
    MainCamera->Movement(ins->fixedDeltaTime); // camera do_movement();
}

void GameWorld::GameOver() {
    cout << "GameOver" << endl;

    SetFixedFPS(FLT_MAX);
    
    GameManager::Instance->Label_over->visiable = 1;
}
GameWorld::GameWorld()
{
    skyBox = new SkyBox();
}


float GameWorld::GetFixedDeltaTime()
{
    return ins->fixedDeltaTime;
}

float GameWorld::GetElapsedTime(bool inGame) {
    if (inGame) {
        return ins->elapsedTime;
    }
    else {
        return (float)glfwGetTime() - ins->startTime;
    }
}

float GameWorld::GetFixedFPS()
{
    return 1.0f / ins->fixedDeltaTime;
}

void GameWorld::SetFixedFPS(float newFPS)
{
    ins->fixedDeltaTime = 1.0f / newFPS;
}

GameObject* GameWorld::Find(string name)
{
    size_t len = ins->objects.size();
    GameObject *obj;
    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
        if (obj->name == name) {
            return obj;
        }
    }
    return nullptr;
}

GameObject* GameWorld::Find(TAG tag)
{
    size_t len = ins->objects.size();
    GameObject *obj;
    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
        if (obj->tag == tag) {
            return obj;
        }
    }
    return nullptr;
}

vector<GameObject*> GameWorld::FindAll(string name)
{
    vector<GameObject *> result;
    size_t len = ins->objects.size();
    GameObject *obj;
    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
        if (obj->name == name) {
            result.push_back(obj);
        }
    }

    return result;
}

vector<GameObject*> GameWorld::FindAll(TAG tag)
{
    vector<GameObject *> result;
    size_t len = ins->objects.size();
    GameObject *obj;
    for (size_t i = 0; i < len; i++) {
        obj = ins->objects[i];
        if (obj->tag == tag) {
            result.push_back(obj);
        }
    }

    return result;
}

GameObject* GameWorld::CreateObject() {
    return CreateObject(vec3(0));
}

GameObject* GameWorld::CreateObject(vec3 position) {
    return CreateObject(position, "");
}

GameObject* GameWorld::CreateObject(vec3 position, const string& modelPath) {
    GameObject *obj;
    if (modelPath == "") {
        obj = new GameObject(position);
    }
    else
    {
        obj = new GameObject(position, modelPath,
            ins->defaultShader);
    }
    GameWorld::ins->objects.push_back(obj);
    return obj;
}
int DirLight::DirIndex = 0;
int Light::index = 0;
int PointLight::PointIndex = 0;
Light* GameWorld::CreateLight(vec3 pos_or_dir) {
	return CreateLight(pos_or_dir, _Directional);
}
Light* GameWorld::CreateLight(vec3 pos_or_dir, LightType type, bool isMainLight/* = false*/) {
    Light* light;

    if (type == _Directional) {
        light = new DirLight(pos_or_dir);
    }
    else if (type == _Point) {
        light = new PointLight(pos_or_dir);
    }

    ins->lights.push_back(light);

    if (isMainLight) {
        MainLight = light;
    }

    return light;
}

