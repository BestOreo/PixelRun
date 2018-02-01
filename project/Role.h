#pragma once

#include "GameWorld.h"
#include "List.h"
#include "ParticleGenerator.h"
#include "SourceManager.h"

class GameManager;
class Player;

class Character : public Behaviour {
public:
    const float radius = 0.5f;
    Player *owner = nullptr;
    Node *destination;
    bool isLeader = false;
    int index = -1;
    bool needCalcDir = true;

    Character(vec3 pos)
    {
        obj = GameWorld::CreateObject(pos);
        obj->BindBehaviour(this);
        GetRandomModel();
        obj->trans->rotation.y = 180;
    }

    void Start() {
        InitDsv();
    }

    void Reset() {
        obj->AddCollider(new Collider(radius, phy::STRANGER));
        obj->velocity = vec3(0);
    }

    void SetOwner(Player *owner, bool isLeader = false);
    void Move();

    void OnCollisionEnter(Collider *other);

    vec3 GetBehindPos();
    bool inDsv = false;

    void FixedUpdate();
	void BackToPool();
private:
#define NUM_OF_MODELS 17
    const char* models_path[NUM_OF_MODELS] = {
        "_model/chr/chr_bow.obj",
        "_model/chr/chr_cat.obj",
        "_model/chr/chr_fox.obj",
        "_model/chr/chr_gumi.obj",
        "_model/chr/chr_jp.obj",
        "_model/chr/chr_knight.obj",
        "_model/chr/chr_man.obj",
        "_model/chr/chr_mom.obj",
        "_model/chr/chr_old.obj",
        "_model/chr/chr_poem.obj",
        "_model/chr/chr_rain.obj",
        "_model/chr/chr_sasami.obj",
        "_model/chr/chr_sol.obj",
        "_model/chr/chr_sword.obj",
        "_model/chr/chr_tale.obj",
        "_model/chr/chr_tama.obj",
        "_model/chr/chr_tsurugi.obj",
    };
    static vector<Model *> models;

    void InitModels() {
        for (size_t i = 0; i < NUM_OF_MODELS; i++)
        {
            auto m = new Model();
            m->loadModel(models_path[i]);
            models.push_back(m);
        }
    }
    void GetRandomModel() {
        if (models.size() == 0) {
            InitModels();
        }

        obj->model = models[random(models.size())];
        obj->shader = GameWorld::Instance->defaultShader;
    }

    vec3 GetNormalDir(vec3 p1, vec3 p2) {
        // from p2 to p1
        vec3 dir = p1 - p2;

        return GetNormalDir(dir);
    }
    vec3 GetNormalDir(vec3 dir) {
        dir.x = GetOne(dir.x);
        // dir.y = GetOne(dir.y);
        dir.z = GetOne(dir.z);

        return dir;
    }
    float GetOne(float x) {
        if (x > MINIUM_FLOAT) {
            return 1;
        }
        if (x < -MINIUM_FLOAT) {
            return -1;
        }
        return 0;
    }

    vec3 normalDir;
    void AdjustRotation() {
        if (normalDir.x >= 0.5) {
            obj->trans->rotation.y = 90;
        }
        if (normalDir.x <= -0.5) {
            obj->trans->rotation.y = -90;
        }
        if (normalDir.z <= -0.5) {
            obj->trans->rotation.y = 180;
        }
        if (normalDir.z >= 0.5) {
            obj->trans->rotation.y = 0;
        }
    }

    void MoveToDest(float time);


    // ----------- DSV -----------

    Shader *oriShader;
    Shader *dsvShader;
    int dsvTextId;

    float step1 = 0.12f;
    float step2 = 0.08f;
    float step3 = 0.05f;

    float dsvThreshold;
    float dsvRate;

    vec3 stage1_color, stage2_color, stage3_color;

    void InitDsv();
    void BeginDsv(float time);
    bool BeforeDraw();
    void AfterDraw();
    void EndDsv();
};

// ------------------------------------------------

class Player : public Behaviour {
public:
    int inland = 0;
	int goin = 0;
	float gap = 1.5f;
    vector<Character *> team;
    vector<Character *> newTeammate;
    Character *leader;
    vec3 oriVel;
    float speed = 6.5f; // meters per second
	float orispeed = 6.5;

    bool SpeedUp = false;
    int ReverseState = 0;
    int lastReverState = -1;
    bool Invincible = false;
    bool Frozen = false;

	bool CollideArch = false;
	bool CollideDirex = false;
	bool CollideDirez = false;
	
    Player(vec3 pos)
    {
        obj = GameWorld::CreateObject(pos);
        obj->BindBehaviour(this);
    }

    void Start() {
        if (Frozen == false) {
            leader = new Character(obj->trans->position);

            leader->SetOwner(this, true);
            team.push_back(leader);
            leader->index = 0;

            InitParticle();
        }
    }

    List path;

    void Freeze();

    void TestForTimer();
    void Recover_SpeedUp();
    void Recover_Reverse();
    void Recover_Invincible();
    void Recover_Frozen();

    void AddTimer_SpeedUp();
    void AddTimer_Reverse();
    void AddTimer_Invincible();
    void AddTimer_Frozen();
	void AddTimer_Frozen(float time);
    void ChangeSpeed(float value);

    void KillTeammate(GLfloat Z);
    void KillLastTeammate();


    int lastCommand = 0;
    int command = 0;
    //int lastpath = 0;

    void FixedUpdate();

private:
    void DealNewTeammate() {
        for (int i = 0; i < newTeammate.size(); i++) {
            newTeammate[i]->SetOwner(this);

            team.push_back(newTeammate[i]);
            newTeammate[i]->index = team.size() - 1;
            //cout << "Get Teamate: " << newTeammate[i]->obj->name << endl;
        }
        newTeammate.clear();
    }

    void ChangeVelocity() {
		if (CollideArch == true) {
			if (InputManager::keys[GLFW_KEY_W] || InputManager::keys[GLFW_KEY_S] || InputManager::keys[GLFW_KEY_A] || InputManager::keys[GLFW_KEY_D]) {
				Recover_Frozen();
				CollideArch = false;
			}
		}

        if (Frozen == false && 
            glfwGetInputMode(Global::window, GLFW_CURSOR) ==
            GLFW_CURSOR_NORMAL) {
            if (InputManager::keys[GLFW_KEY_W]) {
				if (Frozen == false && speed == 0) {
					speed = orispeed;
				}
                command = GLFW_KEY_W;
                if (ReverseState == 0)
                    obj->velocity = vec3(0, 0, -speed);
                else
                    obj->velocity = vec3(0, 0, speed);
            }
            if (InputManager::keys[GLFW_KEY_S]) {
				if (Frozen == false && speed == 0) {
					speed = orispeed;
				}
                command = GLFW_KEY_S;
                if (ReverseState == 0)
                    obj->velocity = vec3(0, 0, speed);
                else
                    obj->velocity = vec3(0, 0, -speed);
            }
            if (InputManager::keys[GLFW_KEY_A]) {
				if (Frozen == false && speed == 0) {
					speed = orispeed;
				}
                command = GLFW_KEY_A;
                if (ReverseState == 0)
                    obj->velocity = vec3(-speed, 0, 0);
                else
                    obj->velocity = vec3(speed, 0, 0);
            }
            if (InputManager::keys[GLFW_KEY_D]) {
				if (Frozen == false && speed == 0) {
					speed = orispeed;
				}
                command = GLFW_KEY_D;
                if (ReverseState == 0)
                    obj->velocity = vec3(speed, 0, 0);
                else
                    obj->velocity = vec3(-speed, 0, 0);

            }
        }

        if (command != lastCommand || ReverseState != lastReverState /*&& command != lastCommand*/) {
            if (team.size() > 1) {
                path.PushBack(obj->trans->position);
                for (int i = 1; i < team.size(); i++) {
                    if (team[i]->destination == NULL) {
                        team[i]->destination = path.back;
                        team[i]->needCalcDir = true;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            leader->needCalcDir = true;
            lastCommand = command;
            lastReverState = ReverseState;
        }
    }

    void InitParticle() {
        ParticleGenerator   *particle;
        particle = new ParticleGenerator(
            new Shader("_shader/particle.vs", "_shader/particle.fs"),
            TextureHelper::load2DTexture("_shader/block.png", TEXTURE_FILTER_TYPE_MAL_MILM),
            700,
            0,	// Center X
            0.2,	// Center Y
            0,	// Center Z
            0,	// Radius
            vec3(0, 0, 0), // Acceleration
            1,	// BlendMode 0-GL_ONE 1-GL_ONE_MINUS_SRC_ALPHA
            0	// DrawMode
        );
        particle->bindObj = obj;
        particle->lifeTime = 1.5f;
        particle->width = 2;
        particle->startScaleRandomPercent = 0.0f;
        particle->rate = 30;
        particle->endColorPoint = 0.1f;
        particle->velocity = vec3(0, 0, 0);
        //Particles->startColor = MiniMath::GetColor(0xff3500);
        //Particles->startScale = 0.8;
        // Particles->endScale = 0.4;
        //Particles->velocity
        GLfloat InputStep[4] = { (GLfloat) 0,(GLfloat)0.3,(GLfloat)0.7,(GLfloat) 1 };
        particle->SetStep(InputStep);
        vec4 InputStepColor[4] = { MiniMath::GetColor(0xFF0000) ,MiniMath::GetColor(0xFF8888), MiniMath::GetColor(0xCCCCCCFF, 0.5), MiniMath::GetColor(0xFFFFFF, 0) };
        particle->SetStepColor(InputStepColor);
        GLfloat InputStepSize[4] = { (GLfloat) 1,(GLfloat)0.6,(GLfloat)0.5,(GLfloat)0.3 };
        particle->SetStepSize(InputStepSize);

        SourceManager::AddParticle(particle);

        particle->AddCopy(0, vec3(0));
    }
};
