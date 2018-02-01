#include "Land.h"
#include "GameManager.h"

using namespace std;
using namespace glm;
void Land::ResetGrids() {
	int k = 0;
	float x = obj->trans->position.x - size.x / 2;
	float z = obj->trans->position.z + size.y / 2;
	float oriZ = z;
	for (int i = 0; i < numGridX; i++) {
		for (int j = 0; j < numGridZ; j++)
		{
			grids[k].pos = vec2(x, z);

			grids[k].pos.x += itemSize.x / 2 +
				MiniMath::GetRandomFloatBetween(0, gapSize.x / 2);
			grids[k].pos.y -= itemSize.y / 2 +
				MiniMath::GetRandomFloatBetween(0, gapSize.y / 2);

			grids[k].occupied = false;
			k++;
			z -= itemSize.y + gapSize.y;
		}
		x += itemSize.x + gapSize.x;
		z = oriZ;
	}

	numfreeGrids = grids.size();
}

glm::vec3 Land::GetRandomPos() {
    if (numfreeGrids > 0) {
        while (true) {
            int index = random(grids.size());
            if (grids[index].occupied == false) {
                grids[index].occupied = true;
                numfreeGrids--;
                return MiniMath::ToVec3(grids[index].pos);
            }
        }
    }
    else {
        return obj->trans->position;
    }
}

Item* Land::GetItem() {
	if (GameManager::itemsPool.empty() == true) {
		//cout << "NEW ITEM" << endl;
		return new Item(GetRandomPos());
	}
	else {
		Item * tmp = GameManager::itemsPool.back();
		GameManager::itemsPool.pop_back();
		tmp->obj->trans->position = GetRandomPos();
        tmp->obj->SetActive(true);
		return tmp;
	}
}
Trap* Land::GetTrap() {
	if (GameManager::trapsPool.empty() == true) {
		//cout << "NEW TRAP" << endl;
		int random = random(2) ;
		if (random == 0)
			return new Fire(GetRandomPos());
		else if (random == 1)
			return new Ice(GetRandomPos());

	}
	else {
		Trap * tmp = GameManager::trapsPool.back();
		GameManager::trapsPool.pop_back();
		tmp->obj->trans->position = GetRandomPos();
        tmp->obj->SetActive(true);
        return tmp;
	}
}
Character* Land::GetCharacter() {
	if (GameManager::charPool.empty() == true) {
		//cout << "NEW CHARACTER" << endl;
		return new Character(vec3(0));
	}
	else {
		Character * tmp = GameManager::charPool.back();
        tmp->inDsv = false;
		GameManager::charPool.pop_back();
		
		return tmp;
	}
}

void Land::InitDsv()
{
    oriShader = obj->shader;
    dsvShader = GameWorld::Instance->lineDsvShader;
    noiseTextId = GameWorld::Instance->lineDsvTextId;
}

void Land::BeginDsv(float time)
{
    inDsv = true;
    dsvThreshold = 0;
    dsvRate = 1.0f / time * GameWorld::GetFixedDeltaTime();
    obj->shader = dsvShader;
}

bool Land::BeforeDraw()
{
    obj->shader->Use();

    glUniform1f(glGetUniformLocation(obj->shader->programID,
        "disolve_threshold"), dsvThreshold);

    return false;
}

void Land::EndDsv()
{
    inDsv = false;
    obj->shader = oriShader;
	GameManager::landsPool.push_back(this);

    for each (auto v in items)
    {
        GameManager::itemsPool.push_back(v);
    }
    for each (auto v in traps)
    {
        GameManager::trapsPool.push_back(v);
    }
    for each (auto v in chraracters)
    {
        GameManager::charPool.push_back(v);
    }
    items.clear();
    traps.clear();
    chraracters.clear();

    obj->SetActive(false);
}
void Land::BackToPool()
{
}