#include <vector>

#include "GameManager.h"
#include "Items.h"
#include "Trap.h"

using namespace std;

void DeathLine::Start()
{
    obj->velocity = vec3(0, 0, -1.5f);
}

// 删除队友有问题
// 人过线就要删除人、道具、陷阱
// GameOver


void DeathLine::FixedUpdate()
{
}

void GameManager::FixedUpdate()
{
	if (dLine != nullptr) {

        DestroyAll(dLine->obj->trans->position.z);

		if (dLine->obj->trans->position.z < backZ + 5) {
            BeginDsvLastLand();
        }

		player->KillTeammate(dLine->obj->trans->position.z);
	}

    // Create
    if (player != nullptr) {
        if (player->obj->trans->position.z < frontZ + createDist) {
            float x = lastLandX - (randomXRange) * landSize.x;
            x += random((randomXRange + 1) * 2) * landSize.x;
            GenerateLandsAt(vec3(x, 0, frontZ - landSize.y));
        }
    }
}
