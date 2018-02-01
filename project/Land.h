#pragma once
#include <vector>
#include <map>
#include <list>

#include "Items.h"
#include "Trap.h"

using namespace std;

class Land : public Behaviour {
public:
	vec2 size = vec2(10);

	vector<Item *> items;
	vector<Trap *> traps;
	// traps
	list<Character *> chraracters;

	const string MODEL_PATH = "_model/env/grass_1.obj";

	Land(vec3 pos = vec3(0)) {
		obj = GameWorld::CreateObject(pos, MODEL_PATH);
		obj->BindBehaviour(this);
		InitGrids();
        InitDsv();
    }
	void Start() {
		obj->AddCollider(new Collider(10, 10, phy::ITEM));
	}
	void Reset(vec3 pos) {
		obj->SetActive(true);
		obj->trans->position = pos;
		ResetGrids();
		// Generate items, traps, characters.
		// ...
		if (random(10) <= 3) {//40%
			items.push_back(GetItem());
		}
		if (random(10) <= 2) {//50%
			traps.push_back(GetTrap());
		}
		if (random(10) <= 1) {
			traps.push_back(new Arch(GetRandomPos()));
		}
		for (size_t i = 0; i < random(2); i++)
		{
			auto teamBh = GetCharacter();
            teamBh->obj->trans->position = GetRandomPos();
            teamBh->Reset();
            teamBh->obj->SetActive(true);
			chraracters.push_back(teamBh);
		}
	}

    void FixedUpdate() {
        if (inDsv) {
            dsvThreshold += dsvRate;
        }
    }

	void OnCollisionEnter(phy::Collider *other) {
		if (other->layer == phy::LEADER) {
			auto player = ((Character*)(other->obj->behaviour))->owner;
			player->inland += 1;
			player->goin += 1;
			//cout << "!!Inland: " << player->inland << endl;
		}
	}
	void OnCollisionExit(phy::Collider *other) {
		if (other->layer == phy::LEADER) {
			auto player = ((Character*)(other->obj->behaviour))->owner;
			player->inland -= 1;
			//cout << "!!Inland: " << player->inland << endl;
		}
	}
	void OnDisable() {
		// Return unused items, traps, characters.
		// ...
	}
	
	void BackToPool();
    void BeginDsv(float time);
private:
	vec2 itemSize = vec2(1.5f, 1.5f);
	vec2 gapSize = vec2(0.5f, 0.5f);
	typedef struct GRID_INFO
	{
		vec2 pos; // world space
		bool occupied = false;
	} GridInfo;
	vector<GridInfo> grids;
	int numfreeGrids = 0;
	int numGridX;
	int numGridZ;
	void InitGrids() {
		numGridX = size.x / (itemSize.x + gapSize.x);
		numGridZ = size.y / (itemSize.y + gapSize.y);

		int count = numGridX * numGridZ;
		for (int i = 0; i < count; i++) {
			GridInfo g;
			grids.push_back(g);
		}

		ResetGrids();
	}
	void ResetGrids();
    vec3 GetRandomPos();
	Item* GetItem();
	Trap* GetTrap();
	Character* GetCharacter();

    // ----------- DSV -----------
    bool inDsv = false;

    Shader *oriShader;
    Shader *dsvShader;
    GLuint noiseTextId;

    float dsvRate;
    float dsvThreshold;

    void InitDsv();
    bool BeforeDraw();
    void EndDsv();
};