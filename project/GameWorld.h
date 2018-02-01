#ifndef _GAMEWORLD_H_
#define _GAMEWORLD_H_

#include <string>
#include <vector>

#include "Transform.h"
#include "GameObject.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"
#include "Physicis.h"
#include "DirLight.h"
#include "PointLight.h"
class GameWorld
{
public:
	const string DEFAULT_MODEL_PATH = "_model/chr/chr_man.obj";
	const string DEFAULT_SHADER_V_PATH = "_shader/loadmodel.vs";
	const string DEFAULT_SAHDER_F_PATH = "_shader/loadmodel.fs";

	static GameWorld *Instance;
    static Camera *MainCamera;
    static Light *MainLight;
    
    // Shadow
    static Shader *simpleDepthShader;
    static const GLuint SHADOW_WIDTH = 1600, SHADOW_HEIGHT = 1600;
    static GLuint depthMapFBO;
    static GLuint depthMap;
    static void InitShadow();

	vector<GameObject *> objects;
	vector<Light *> lights;

    Shader *defaultShader;
    Shader *dsvShader;
    Shader *lineDsvShader;
    GLuint dsvTextId;
    GLuint lineDsvTextId;

	~GameWorld();

	static void Init();

    // Create Object
	static GameObject* CreateObject();
	static GameObject* CreateObject(vec3 position);
	static GameObject* CreateObject(vec3 position, const string& modelPath);

	static Light* CreateLight(vec3 position);
	static Light* CreateLight(vec3 position, LightType type, bool isMainLight = false);

	static void Draw();
	static void FixedUpdate();

    // Time Related
	static float GetFixedDeltaTime();
	static float GetElapsedTime(bool inGame = true);
	static float GetFixedFPS();
	static void SetFixedFPS(float newFPS);
	
    // Find
    static GameObject* Find(string name);
    static GameObject* Find(TAG tag);
    static vector<GameObject*> FindAll(string name);
    static vector<GameObject*> FindAll(TAG tag);

	static void GameOver();
protected:
	float elapsedTime = 0;	// Time elapsed in game world since GameWorld inited.
	float startTime = 0;
	float fixedDeltaTime = 0.03;

    float lastFixedUpdateTime = -1;
    float nextFixedUpdateTime = -1;
	
	SkyBox *skyBox;

	static GameWorld *ins;
	GameWorld();
};

#endif