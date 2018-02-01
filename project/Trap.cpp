#include "Trap.h"
#include "TimerManager.h"	
void Trap::DestroyTrap() {
	obj->SetActive(false);
}
void Fire::AddTimer() {
	TimerManager::AddTimer_Traps(this, &Trap::ExecutionForTimer, 5);
}

Fire::Fire(vec3 pos)
{
    pos.y += 0.35;
    if (particle == nullptr) {
        particle = new ParticleGenerator(
            new Shader("_shader/particle.vs", "_shader/particle.fs"),
            TextureHelper::loadDDS("_shader/particle.DDS"),
            1600,
            0,	// Center X
            0,	// Center Y
            0,	// Center Z
            1.0f,	// Radius
            vec3(0, -3.2, 0), // Acceleration 
            0,	// BlendMode 0-GL_ONE 1-GL_ONE_MINUS_SRC_ALPHA
            2	// DrawMode 
        );
        particle->lifeTime = 1.0f;
        particle->width = 8;
        particle->offset = vec3(0);
        particle->startScaleRandomPercent = 0.0f;
        particle->rate = 600;
        particle->endColorPoint = 0.1f;
        particle->velocity = vec3(0, 3, 0);

        GLfloat InputStep[4] = { (GLfloat) 0,(GLfloat)0.3,(GLfloat)0.7,(GLfloat) 1 };
        particle->SetStep(InputStep);
        vec4 InputStepColor[4] = { MiniMath::GetColor(0xFFCC00) ,MiniMath::GetColor(0xFF3300), MiniMath::GetColor(0xFF6600), MiniMath::GetColor(0xFF3300) };
        particle->SetStepColor(InputStepColor);
        GLfloat InputStepSize[4] = { (GLfloat)0.005,(GLfloat)0.004,(GLfloat)0.003,(GLfloat)0.002 };
        particle->SetStepSize(InputStepSize);

        SourceManager::ParticleVector.push_back(particle);
    }

    //TimerManager::AddTimer_Particles(particle, &ParticleGenerator::ExecutionForTimer, 5);
    //AddTimer();
    obj = GameWorld::CreateObject(pos, "_model/others/OBJ_Trigger_A.obj");
    //particle->bindObj = obj;
    obj->BindBehaviour(this);
}

void Fire::ExecutionForTimer() {
	if (particle->rate > 50) {
		//cout << "Down" << endl;
		show = false;
		particle->rate = 50;
		particle->velocity[1] -= 4;
	}
	else {
		show = true;
		//cout << "Up" << endl;
		particle->rate = 600;
		particle->velocity[1] += 4;
	}
}
void Ice::AddTimer() {
	TimerManager::AddTimer_Traps(this, &Trap::ExecutionForTimer, 5);
}

Ice::Ice(vec3 pos)
{
    pos.y += 0.35;

    if (particle == nullptr) {
        particle = new ParticleGenerator(
            new Shader("_shader/particle.vs", "_shader/particle.fs"),
            TextureHelper::loadDDS("_shader/particle.DDS"),
            1600,
            0,	// Center X
            0,	// Center Y
            0,	// Center Z
            1.0f,	// Radius
            vec3(0, -3.2, 0), // Acceleration 
            0,	// BlendMode 0-GL_ONE 1-GL_ONE_MINUS_SRC_ALPHA
            2	// DrawMode 
        );
        particle->lifeTime = 1.0f;
        particle->width = 8;
        particle->offset = vec3(0);
        particle->startScaleRandomPercent = 0.0f;
        particle->rate = 600;
        particle->endColorPoint = 0.1f;
        particle->velocity = vec3(0, 3, 0);

        GLfloat InputStep[4] = { (GLfloat) 0,(GLfloat)0.3,(GLfloat)0.7,(GLfloat) 1 };
        particle->SetStep(InputStep);
        vec4 InputStepColor[4] = { MiniMath::GetColor(0x00CCFF) ,MiniMath::GetColor(0x0033FF), MiniMath::GetColor(0x0066FF), MiniMath::GetColor(0x0033FF) };
        particle->SetStepColor(InputStepColor);
        GLfloat InputStepSize[4] = { (GLfloat)0.005,(GLfloat)0.004,(GLfloat)0.003,(GLfloat)0.002 };
        particle->SetStepSize(InputStepSize);

        SourceManager::ParticleVector.push_back(particle);
    }

    //TimerManager::AddTimer_Particles(particle, &ParticleGenerator::ExecutionForTimer, 5);
    //AddTimer();
    obj = GameWorld::CreateObject(pos, "_model/others/OBJ_Trigger_A.obj");
    //particle->bindObj = obj;
    obj->BindBehaviour(this);
}

void Ice::ExecutionForTimer() {
	if (particle->rate > 50) {
		//cout << "Down" << endl;
		show = false;
		particle->rate = 50;
		particle->velocity[1] -= 4;
	}
	else {
		show = true;
		//cout << "Up" << endl;
		particle->rate = 600;
		particle->velocity[1] += 4;
	}
}

 Arch::Arch(vec3 pos) {
	pos[1] = 2;
	obj = GameWorld::CreateObject(pos, "_model/others/BUD_Building_B.obj");
	//particle->bindObj = obj;
	obj->BindBehaviour(this);
}