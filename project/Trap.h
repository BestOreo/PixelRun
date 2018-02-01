#pragma once
#include "Role.h"
#include "ParticleGenerator.h"

using namespace glm;
using namespace phy;
using namespace std;

class Trap : public Behaviour
{
public:
	Trap() 
	{
	}
	virtual void ExecutionForTimer()
	{
	}
	virtual void AddTimer() 
	{
	}
	~Trap()
	{
	}
	void DestroyTrap();
private:

};


class Fire : public Trap
{
public:
	static ParticleGenerator *particle;
	GameObject* obj;
	//float rotationSpeed = 60;
	bool show = true;
    int id;
    static int nextId;
    Fire(vec3 pos);
	void ExecutionForTimer();
	void AddTimer();
	
    void Start() {
        obj->AddCollider(new Collider(0.5, phy::ITEM));
        OnEnable();
    }

    void OnEnable() {
        id = nextId;
        particle->AddCopy(nextId, obj->trans->position - vec3(0, 0.4, 0));
        nextId++;
    }

    void OnDisable() {
        particle->RemoveCopy(id);
    }

    void FixedUpdate() {
        //obj->trans->rotation.y += rotationSpeed * GameWorld::GetFixedDeltaTime();
    }

	void OnCollisionEnter(phy::Collider *other) {
		if (show && ((Character *)(other->obj->behaviour))->owner->Invincible == false) {
			if (other->layer == phy::LEADER) {
				//cout << "Game Over!" << endl;
				//TODO
				GameWorld::GameOver();
			}
			else if (other->layer == phy::TEAMMATE) {
				auto chr = (Character *)(other->obj->behaviour);
				auto player = ((Character *)(other->obj->behaviour))->owner;
				int i;
				bool found = false;
				for (i = 0; i < player->team.size(); i++) {
					if (chr == player->team[i]) {
						found = true;
						break;
					}
				}
				if (found) {
					for (int j = player->team.size() - 1; j >= i; j--) {
						player->team[j]->obj->SetActive(false);

						
						player->KillLastTeammate();
						//player->team.pop_back();
						

						//TODO: ªÿ ’
						cout << "byebye! index at" << j << endl;
					}
				}

			}
		}
		
	}

    ~Fire() {
        // TODO
    }

private:

};

class Ice : public Trap {
public:
	const string MODEL_PATH = "_model/trap/ice.obj";
	static ParticleGenerator *particle;
	GameObject* obj;
    int id;
    static int nextId;
	//float rotationSpeed = 60;
	bool show = true;
    Ice(vec3 pos);
	void ExecutionForTimer();
	void AddTimer();

	void Start() {
		obj->AddCollider(new Collider(0.5, phy::ITEM));
		OnEnable();
	}

    void OnEnable() {
        id = nextId;
        particle->AddCopy(nextId, obj->trans->position - vec3(0, 0.4, 0));
        nextId--;
    }

    void OnDisable() {
        particle->RemoveCopy(id);
    }

	void FixedUpdate() {
		//obj->trans->rotation.y += rotationSpeed * GameWorld::GetFixedDeltaTime();
	}

	void OnCollisionEnter(phy::Collider *other) {
		if (other->layer == phy::LEADER) {
			if (show && ((Character *)(other->obj->behaviour))->owner->Invincible == false) {
				auto player = ((Character *)(other->obj->behaviour))->owner;
				player->Freeze();
				for (int i = 0; i < player->team.size(); i++) {
					player->team[i]->obj->shader = new Shader("_shader/loadmodel.vs", "_shader/frozen.fs");
				}
				player->AddTimer_Frozen();
			}
		}

	}

	~Ice() {
		// TODO
	}

private:
};

class Arch : public Trap
{
public:
	GameObject* obj;
	//float rotationSpeed = 60;
	bool show = true;
	int id;
	static int nextId;
	Arch(vec3 pos);
	int collide = 0;


	void Start() {
		obj->AddCollider(new Collider(3.5 , phy::ITEM));
		OnEnable();
	}

	void OnEnable() {

	}

	void OnDisable() {
		//particle->RemoveCopy(id);
	}

	void FixedUpdate() {
		//obj->trans->rotation.y += rotationSpeed * GameWorld::GetFixedDeltaTime();
	}

	void OnCollisionEnter(phy::Collider *other) {
			if (show && ((Character *)(other->obj->behaviour))->owner->Invincible == false) {
				if (other->layer == phy::LEADER) {
					auto player = ((Character *)(other->obj->behaviour))->owner;
					player->Freeze();
					player->CollideArch = true;
				}
			}
		
	}
				
				


	~Arch() {
		// TODO
	}

private:

};
