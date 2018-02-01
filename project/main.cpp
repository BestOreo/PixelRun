#include <iostream>
#include <cmath>
#include <cstdlib>

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
#include "ParticleGenerator.h"

// Engine Logic
#include "GameWorld.h"
#include "GlobalValue.h"
#include "InputManager.h"
#include "MiniMath.h"
#include "LabelManager.h"
#include "SourceManager.h"

// Game
#include "TestBehaviour.cpp"
#include "Role.h"
#include "GameManager.h"
#include "TimerManager.h"


using namespace mmath;

// -------------------------------------------------

GameWorld* GameWorld::Instance = NULL;
GameWorld* GameWorld::ins = NULL;
Camera* GameWorld::MainCamera = NULL;
Light* GameWorld::MainLight = NULL;


Shader *GameWorld::simpleDepthShader;
//const GLuint GameWorld::SHADOW_WIDTH = 1024;
//const GLuint GameWorld::SHADOW_HEIGHT = 1024;
GLuint GameWorld::depthMapFBO;
GLuint GameWorld::depthMap;

int Fire::nextId = 0;
int Ice::nextId = -1;
ParticleGenerator *Fire::particle = nullptr;
ParticleGenerator *Ice::particle = nullptr;

int Collider::nexdId = 0;
PhyManager *PhyManager::Instance = NULL;
PhyManager *PhyManager::ins = NULL;

bool InputManager::keys[] = {};
bool InputManager::forbid[] = {};
bool InputManager::ReleasedKeys[] = {};

bool InputManager::firstMouse = true;
double InputManager::MousePosX = 0.0;
double InputManager::MousePosY = 0.0;
double InputManager::MouseScrollY = 0.0;

GLFWwindow* Global::window = NULL;
GLuint Global::WIN_WIDTH = 1024;
GLuint Global::WIN_HEIGHT = 800;

vector<ParticleGenerator*> SourceManager::ParticleVector;
vector<LabelManager*> SourceManager::LabelVector;

// -------------------------------------------------
ParticleGenerator   *Particles2;


int ScoreManager::score = 0;

GameManager *GameManager::Instance;
DeathLine *GameManager::dLine;

vector<PlayerTimer*> TimerManager::PlayerTimers;
vector<ParticlesTimer*> TimerManager::ParticlesTimers;
vector<TrapTimer*> TimerManager::TrapTimers;
Player* GameManager::player = nullptr;
vector<Land *> GameManager::landsPool;
vector<Item *> GameManager::itemsPool;
vector<Trap *> GameManager::trapsPool;
vector<Character *> GameManager::charPool;
vector<Model *> Character::models;

void SceneInitG() {
	InputManager::Init();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	Particles2 = new ParticleGenerator(
		new Shader("_shader/particle.vs", "_shader/particle.fs"),
		//TextureHelper::load2DTexture("_shader/flame.bmp", TEXTURE_FILTER_TYPE_MAL_MILM, GL_RGBA, GL_RGBA, SOIL_LOAD_LA),
		//TextureHelper::load2DTexture("_shader/flame.png", TEXTURE_FILTER_TYPE_MAN_MIN, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA),
		TextureHelper::loadDDS("_shader/particle.DDS"),
		//TextureHelper::load2DTexture("_shader/particle.png", TEXTURE_FILTER_TYPE_MAL_MILM, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA),
		//TextureHelper::load2DTexture("_shader/block.png", TEXTURE_FILTER_TYPE_MAL_MILM),
		//TextureHelper::load2DTexture("_model/box/disolve.png", TEXTURE_FILTER_TYPE_MAL_MILM, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA),
		700,
		0,	// Center X
		0,	// Center Y
		4,	// Center Z
		1.0f,	// Radius
		vec3(0, -3.2, 0), // Acceleration
		0,	// BlendMode 0-GL_ONE 1-GL_ONE_MINUS_SRC_ALPHA
		2	// DrawMode
	);
	Particles2->lifeTime = 1.0f;
	Particles2->width = 8;
	Particles2->offset = vec3(0, 2, 0);
	Particles2->startScaleRandomPercent = 0.0f;
	Particles2->rate = 600;
	Particles2->endColorPoint = 0.1f;
	Particles2->velocity = Particles2->ori_velocity = vec3(0, 4.0, 0);
	//Particles->startColor = MiniMath::GetColor(0xff3500);
	//Particles->startScale = 0.8;
	// Particles->endScale = 0.4;
	//Particles->velocity
	GLfloat InputStep[4] = { (GLfloat)0,(GLfloat)0.3,(GLfloat)0.7,(GLfloat)1 };
	Particles2->SetStep(InputStep);
	vec4 InputStepColor[4] = { MiniMath::GetColor(0xFFCC00) ,MiniMath::GetColor(0xFF3300), MiniMath::GetColor(0xFF6600), MiniMath::GetColor(0xFF3300) };
	Particles2->SetStepColor(InputStepColor);
	GLfloat InputStepSize[4] = { (GLfloat)0.005,(GLfloat)0.004,(GLfloat)0.003,(GLfloat)0.002 };
	Particles2->SetStepSize(InputStepSize);
	Particles2->needSort = true;
	SourceManager::ParticleVector.push_back(Particles2);
	Particles2->AddTimer(8);






	//GameObject *home1 = GameWorld::CreateObject(vec3(8, 0, 3), "./_model/others/BUD_Building_A.obj");
	//GameObject *home2 = GameWorld::CreateObject(vec3(15, 2, -5), "./_model/others/BUD_Building_B.obj");
	Particles2->AddCopy(1,vec3(13, 0, -3));
	//Particles2->AddCopy(2,vec3(16.5, 0, -3));
	//Particles2->RemoveCopy(2);

	//GameObject *home3 = GameWorld::CreateObject(vec3(6, 1, -13), "./_model/others/BUD_Building_E_01.obj");



}

void SceneInitW() {
    GameManager::Init();

    //TB1 *t = new TB1();

    //auto tt = GameWorld::CreateObject(vec3(0, 0, -0), "_model/test/tt.obj");
    //auto t1 = GameWorld::CreateObject(vec3(0, 0, -5), "_model/test/t1.obj");
    //auto t3 = GameWorld::CreateObject(vec3(3, 0, -3), "_model/MouseDrago Blue.obj");
    //auto t3 = GameWorld::CreateObject(vec3(3, 0, -3), "_model/chr_old.obj");
    //t2->trans->scale = vec3(30);
    //test->trans->scale.x = 10;
    //test->trans->scale.y = 10;
    //test->trans->scale.z = 10;
}

void SceneInitM() {
	auto light1 = GameWorld::CreateLight(vec3(50, 50, -50), _Point);

	//light1->setAmbient(vec3(0.8f, 0.8f, 0.8f));
	light1->setDiffuse(vec3(0.8f, 0.8f, 0.8f));
	light1->setSpecular(vec3(0.0f, 0.0f, 0.0f));
	light1->setQuadratic(0.000007);
	light1->setLinear(0.0014f);
}

void SceneInit() {
    GameWorld::Init();

	//GameWorld::CreateObject(vec3(1,2,-5), "_model/others/BUD_Building_B.obj");
    //SceneInitG();
    SceneInitW();
    SceneInitM();
}

// -------------------------------------------------

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// -------------------------------------------------

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a GLFWwindow object that we can use for GLFW's functions
    Global::window = glfwCreateWindow(Global::WIN_WIDTH, Global::WIN_HEIGHT, "GameWorld", nullptr, nullptr);
    glfwMakeContextCurrent(Global::window);

    // Set the required callback functions
    glfwSetKeyCallback(Global::window, key_callback);
    glfwSetCursorPosCallback(Global::window, mouse_callback);
    glfwSetScrollCallback(Global::window, scroll_callback);
    //glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, Global::WIN_WIDTH, Global::WIN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

    SceneInit();

    while (!glfwWindowShouldClose(Global::window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        GameWorld::FixedUpdate();

        // Update particles
        for (auto iter = SourceManager::ParticleVector.cbegin(); iter != SourceManager::ParticleVector.cend(); iter++) {
			(*iter)->Update(deltaTime);
        }

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GameWorld::Draw();

		for (auto iter = SourceManager::ParticleVector.cbegin(); iter != SourceManager::ParticleVector.cend(); iter++) {
			(*iter)->Draw();
		}

		for (auto iter = SourceManager::LabelVector.cbegin(); iter != SourceManager::LabelVector.cend(); iter++) {
			if ((*iter)->visiable == true)
				(*iter)->DrawNameLabel();
		}

		TimerManager::Tick(deltaTime);



        // Swap the screen buffers
        glfwSwapBuffers(Global::window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// -------------------------------------------------

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameWorld::MainCamera->ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GameWorld::MainCamera->Deal_mouse_callback(window, xpos, ypos);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    InputManager::Deal_key_callback(window, key, scancode, action, mode);
}