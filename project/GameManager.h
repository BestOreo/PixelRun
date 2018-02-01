#pragma once

#include <vector>
#include <map>
#include <list>

#include "Items.h"
#include "Trap.h"
#include "Land.h"
using namespace std;

class DeathLine : public Behaviour {
public:
    //const string MODEL_PATH = "_model/env/line.obj";

    DeathLine(vec3 pos = vec3(0, 0, 8)) {
        obj = GameWorld::CreateObject(pos);
        obj->BindBehaviour(this);
    }

    void Start();

    void FixedUpdate();

};

class GameManager : public Behaviour
{
public:
    static GameManager *Instance;

    float createDist = 25;   // from player
    float destoryDist = 15; // from dLine

    /*vector<Item *> items;
    vector<Trap *> traps;*/
    list<vector<Land *> *> lands;
    static vector<Land *> landsPool;
	static vector<Item *> itemsPool;
	static vector<Trap *> trapsPool;
	static vector<Character *> charPool;
    static DeathLine *dLine;
    static Player *player;
    LabelManager *Label_over;
    LabelManager *Label;
    ScoreManager *ScoreLabel;

    static void Init() {
        if (Instance == nullptr) {
            Instance = new GameManager();
        }
        else {
            cout << "GameManager alright inited!" << endl;
            return;
        }
    }

    void Start() {
        PhyManager::Instance->rules.push_back(new phy::CollisionRule(phy::LEADER, phy::STRANGER));
        PhyManager::Instance->rules.push_back(new phy::CollisionRule(phy::LEADER, phy::ITEM));
        PhyManager::Instance->rules.push_back(new phy::CollisionRule(phy::TEAMMATE, phy::ITEM));

        StartGame();
    }

    void StartGame() {
        player = new Player(vec3(0, 0, 0));
        dLine = new DeathLine();

        Label_over = new LabelManager(
            new Shader("_shader/Billboard_word.vertexshader", "_shader/Billboard.fragmentshader"),
            //TextureHelper::load2DTexture("_shader/flame.bmp", TEXTURE_FILTER_TYPE_MAL_MILM, GL_RGBA, GL_RGBA, SOIL_LOAD_LA),
            TextureHelper::load2DTexture("_shader/g1.png", TEXTURE_FILTER_TYPE_MAN_MIN, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA)
        );
        Label_over->SetPositon(vec3(-2.0, 4.0, -5.0));
        Label_over->scale = 1;
        Label_over->id = 1;
        Label_over->visiable = 0;
        SourceManager::LabelVector.push_back(Label_over);
        ScoreLabel = new ScoreManager();

        GenerateLandsAt(vec3(0));
    }
	
    void FixedUpdate();

    ~GameManager()
    {
        // Clean Pool
    }

private:
    GameManager()
    {
        obj = GameWorld::CreateObject();
        obj->BindBehaviour(this);
    }

    vector<Land *> *dsvingRow;
    int landsNumOfRow = 3;
    vec2 landSize = vec2(10, 10);
    float backZ = 0;
    float frontZ = 0;
    float lastLandX = 0;
    int randomXRange = 1;
    void GenerateLandsAt(vec3 pos) {
        lastLandX = pos.x;
        frontZ = pos.z;

        vector<Land *> *row = new vector<Land *>();
        for (int i = 0; i < landsNumOfRow; i++) {
            auto tmp = GetLand();
            tmp->Reset(pos);
            pos.x += landSize.x;
            row->push_back(tmp);
        }
        lands.push_back(row);
    }
	void DestroyAll(float z) {
        if (dsvingRow == nullptr) return;

		int count = dsvingRow->size();
		for (size_t j = 0; j < count; j++)
		{
			list<Character*> temp_ch = (*dsvingRow)[j]->chraracters;
			for (list<Character*>::iterator i = temp_ch.begin(); i != temp_ch.end(); i++) {
				if ((*i)->obj->trans->position.z > z) {
                    if ((*i)->inDsv == false) {
                        (*i)->BackToPool();
                    }
				}
			}
			vector<Item*> items = (*dsvingRow)[j]->items;
			for (int i = 0; i < items.size(); i++) {
				if (items[i]->obj->trans->position.z > z)
					items[i]->DestroyItem();
			}
			vector<Trap*> traps = (*dsvingRow)[j]->traps;
			for (int i = 0; i < traps.size(); i++) {
				if (traps[i]->obj->trans->position.z > z)
					traps[i]->DestroyTrap();
			}
		}
	}
    void BeginDsvLastLand() {
        if (dsvingRow != nullptr) {
            delete dsvingRow;
        }

        vector<Land *> *row = lands.front();
        int count = row->size();
        for (size_t j = 0; j < count; j++)
        {
			(*row)[j]->BeginDsv(landSize.y / (-dLine->obj->velocity.z));
        }

        dsvingRow = row;
        lands.pop_front();
        auto f = lands.front();
        if (f != nullptr) {
            backZ = (*f)[0]->obj->trans->position.z;
		}
    }

    Land *GetLand() {
        if (landsPool.empty() == true) {
            //cout << "NEW Land" << endl;
            return new Land();
        }
        else {
            Land * tmp = landsPool.back();
            landsPool.pop_back();
            return tmp;
        }
    }

};