#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>
#include <map>

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
#include "Texture.h"
#include "GameObject.h"
#include "MiniMath.h"
using namespace mmath;



#define delPI (3.1415926/180*2)

// Represents a single particle and its state
struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec2 RandomOffset;
	glm::vec4 Color;
	glm::vec4 ColorArray[4];
	GLfloat Life;
	GLfloat Scale;
	GLfloat OriginalLife;
	void SetLifeTime(GLfloat LifeTime);
	void SetStepChange(GLfloat StepRate[4], glm::vec4  StepColor[4], float Size[4]);
	glm::vec4 UpdateColor();
	float UpdateScale();
	GLfloat StepArray[4];
	float SizeArray[4];
	float phase; //œ‡Œª
};



// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:

	int idd = 0;
	
	// Constructor
	ParticleGenerator(Shader *shader, GLuint textureId, GLuint amount, GLfloat CenterX,
		GLfloat CenterY, GLfloat CenterZ, GLfloat Radius, vec3 SpeedUp, int BlendMode, int DrawPatternMode);
	// Update all particles
	void Update(GLfloat dt);
	// Render all particles
	void Draw();

	~ParticleGenerator();

    bool needSort = false;
	GameObject *bindObj = nullptr;
	vec3 offset = vec3(0);
	vec3 velocity = vec3(0.0f, 2.0f, 0.0f);
	vec3 ori_velocity;

	// TODO: 
	// burst
	// rotation

	// Ge:
	// velocity change over time(gravity)
	// more color: 4
	// more scale: 4
	// emmision shape: circle angle
	// velocity random

	GLfloat rate = 10; // num of new particles per secon
	GLfloat width = 5.0f;

	GLfloat startScale = 2.0f;
	GLfloat endScale = 0.5f;
	float startScaleRandomPercent = 0.1f;
	GLfloat scaleChangeSpeed = 0.3f; // per seconds

	GLfloat lifeTime = 10.0f;

	vec4 startColor = vec4(0, 0, 1.0, 1);
	vec4 endColor = vec4(1, 1, 1, 1);
	float endColorPoint = 0.5f;

	GLfloat CenterX = 0.0f;
	GLfloat CenterY = 0.0f;
	GLfloat CenterZ = 0.0f;
	GLfloat Radius = 0.0f;
	int RadianCounter = 0;
	vec3 SpeedUp = vec3(0, 0.01, 0);

	struct copynode {
		vec3 offset;
		int id;
	};
	int id = 0;
	vector<struct copynode> copyoffset;


	int BlendMode; // 0:GL_ONE , 1:GL_ONE_MINUS_SRC_ALPHA

	int DrawPatternMode;

	void SetStep(GLfloat step[4]);
	void SetStepColor(vec4 stepcolor[4]);
	void SetStepSize(GLfloat stepsize[4]);

	void Init_Pattern_Default_Respawn(Particle &p);	// Set Emittor Behavior
	void UpdatePattern_Default(Particle &p, float dt);	// Set Particle Behavior  
	void Init_Pattern_LightLoop_Respawn(Particle &p);
	void UpdatePattern_LightLoop(Particle &p, float dt);
	void Init_Pattern_Fire_Respawn(Particle &p);
	void UpdatePattern_Fire(Particle &p, float dt);

	void ExecutionForTimer();
	void AddTimer(GLfloat time);
	void AddCopy(int id, vec3 offset);
	void RemoveCopy(int id);

	std::vector<Particle> particles;

private:
	// State
	std::map<float, int> sorted;
	GLuint amount;
	// Render state
	Shader *shader;
	GLuint textureId;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle);

	void Sort();
	GLfloat accumulateTime = 0;
	float GetRandomScale();
	vec4 GetLerpColor(float percent);

	// Contorl Step Color and Size change
	GLfloat DefaultStep[4] = { (GLfloat)0,(GLfloat)0.3,(GLfloat)0.7,(GLfloat)1 };
	//vec4 DefaultStepColor[4] = { MiniMath::GetColor(0x00ff00) ,MiniMath::GetColor(0xffffff), MiniMath::GetColor(0xffcc23), MiniMath::GetColor(0xff3500) };
	vec4 DefaultStepColor[4] = { MiniMath::GetColor(0x0000FF) ,MiniMath::GetColor(0xFF0000), MiniMath::GetColor(0xFF0000), MiniMath::GetColor(0xFF0000) };
	GLfloat DefaultStepSize[4] = { (GLfloat)1.5,(GLfloat)1.1,(GLfloat)0.7,(GLfloat)0.2 };
};




#endif

