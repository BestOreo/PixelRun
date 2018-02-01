#pragma once

#include "Role.h"

using namespace glm;
using namespace phy;
using namespace std;
enum ItemType{
	_SpeedUp,
	_Reverse,
	_Invincible,
	_Wall,
};

class Item : public Behaviour
{
public:
	string MODEL_PATH;
	float rotationSpeed = 60;
	ItemType itemType;
	Item(vec3 pos){
		itemType = getRandomType();
		initItem(pos, itemType);
	}
    Item(vec3 pos, ItemType type):itemType(type)
    {
		initItem(pos, itemType);
    }
	void Start() {
		obj->AddCollider(new Collider(0.5, phy::ITEM));
	}

	void FixedUpdate() {
		obj->trans->rotation.y += rotationSpeed * GameWorld::GetFixedDeltaTime();
	}

	void OnCollisionEnter(phy::Collider *other) {
		if (other->layer == phy::LEADER) {
			auto player = ((Character *)(other->obj->behaviour))->owner;
			switch (itemType)
			{
			case _SpeedUp:
				player->SpeedUp = true;
				player->ChangeSpeed(player->speed + 3.5f);
				player->AddTimer_SpeedUp();
				break;
			case _Reverse:
				player->ReverseState = 1;
				player->AddTimer_Reverse();
				
				break;
			case _Invincible:
				player->Invincible = true;
				player->AddTimer_Invincible();
				break;
			case _Wall:

				break;
			default:
				break;
			}
			

			DestroyItem();
		}
	}
	void DestroyItem();
    ~Item()
    {
    }

private:
	const int numOfTypes = 3;
	const ItemType types[3] = {
		_SpeedUp,
		_Invincible,
		_Reverse
	};
	ItemType getRandomType() {
		return types[random(numOfTypes)];
	}
	void initItem(vec3 pos,ItemType itemType) {
		vec3 scale = vec3(1.0, 1.0, 1.0);
		float y = 0;
		switch (itemType)
		{
		case _SpeedUp:
			MODEL_PATH = "_model/obj/Hamburger.obj";
			scale = vec3(0.2, 0.2, 0.2);
			break;
		case _Reverse:
			MODEL_PATH = "_model/obj/reverse.obj";
			scale = vec3(0.2, 0.2, 0.2);
			break;
		case _Invincible:
			MODEL_PATH = "_model/others/Ani_Bird.obj";
			y += 0.4;
			scale = vec3(0.8, 0.8, 0.8);
			break;
		case _Wall:

			break;
		default:
			break;
		}
		obj = GameWorld::CreateObject(pos, MODEL_PATH);
		obj->trans->scale = scale;
		obj->BindBehaviour(this);
		obj->trans->position.y += y;
	}
};


//class SpeedItem : public Item
//{
//public:
//    const string MODEL_PATH = "_model/obj/speed_up.obj";
//    float rotationSpeed = 60;
//
//    SpeedItem(vec3 pos)
//    {
//        obj = GameWorld::CreateObject(pos, MODEL_PATH);
//        obj->BindBehaviour(this);
//    }
//
//    void Start() {
//        obj->AddCollider(new Collider(0.5, phy::ITEM));
//    }
//
//    void FixedUpdate() {
//        obj->trans->rotation.y += rotationSpeed * GameWorld::GetFixedDeltaTime();
//    }
//
//    void OnCollisionEnter(phy::Collider *other) {
//        if (other->layer == phy::LEADER) {
//            auto player = ((Character *)(other->obj->behaviour))->owner;
//            player->ChangeSpeed(player->speed * 2.5f);
//
//            obj->SetActive(false);
//        }
//    }
//
//    ~SpeedItem() {
//        // TODO
//    }
//
//private:
//
//};
