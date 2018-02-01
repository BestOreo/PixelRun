#pragma once

#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <list>

#include "Transform.h"
#include "Behaviour.h"
#include "MiniMath.h"

#define XZ(v3) (vec2(v3.x, v3.z))

namespace phy {

    enum COLLIDER_SHAPE
    {
        CIRCLE = 0,
        RECT,
    };

    enum COLLIDER_LAYER {
        LEADER = 0,
        BLOCK,
        ITEM,
        EFFECT,
        TEAMMATE,
        STRANGER,

        _LAYER_MAX,
    };

    class CollisionRule {
    public:
        COLLIDER_LAYER first;
        COLLIDER_LAYER second;
        CollisionRule(COLLIDER_LAYER f, COLLIDER_LAYER s) {
            first = f;
            second = s;
        }
    };

    enum COLLISION_STATE {
        ENTER,
        STAY,
    };

    class Collider
    {
    public:
        COLLIDER_SHAPE shape;
        COLLIDER_LAYER layer;
        vec2 size;  // for cir, size.x is radius.
        GameObject *obj;
        Transform *trans;
        Behaviour *beha;

        Collider(float radius, COLLIDER_LAYER layer);
        Collider(float width, float height, COLLIDER_LAYER layer);

        ~Collider();

        int GetId() { return id; }

    private:
        int id;
        static int nexdId;
        Collider() {}
        void Init(COLLIDER_LAYER layer);
    };

    class PhyManager
    {
    public:
        static PhyManager *Instance;

        vector<CollisionRule *> rules;
        map<int, Collider *> colliders;
        map<int, COLLISION_STATE> collisionState;
        map<COLLIDER_LAYER, list<Collider *> *> collidersOfLayer;

        static void Init() {
            if (ins != nullptr) return;

            ins = new PhyManager();
            Instance = ins;

            for (size_t i = 0; i < _LAYER_MAX; i++)
            {
                ins->collidersOfLayer.insert(make_pair((COLLIDER_LAYER)i,
                    new list<Collider *>()));
            }
        }

        static void AddCollider(Collider *col) {
            ins->colliders.insert(make_pair(col->GetId(), col));
            ins->collidersOfLayer[col->layer]->push_back(col);
        }

        static void RemoveCollider(Collider *col) {
            ins->colliders.erase(col->GetId());
            ins->collidersOfLayer[col->layer]->remove(col);
        }

        static void Tick();

        ~PhyManager()
        {
            for (size_t i = 0; i < _LAYER_MAX; i++)
            {
                delete ins->collidersOfLayer[(COLLIDER_LAYER)i];
            }

            delete ins;
            ins = Instance = nullptr;
        }
    private:
        static PhyManager *ins;

        PhyManager() {}

        static bool IsCollided(Collider *col1, Collider *col2);
        static bool IsCollidedCC(Collider *cir1, Collider *cir2);
        static bool IsCollidedRR(Collider *rect1, Collider *rect2);
        static bool IsCollidedCR(Collider *cir, Collider *rect);

        static vec2 GetLbPoint(Collider *rect) {
            return GetRectVertex(rect, 3);
        }

        // vertexIndex: 0~3, left-top, right-top, right-bottom, left-bottom
        static vec2 GetRectVertex(Collider *rect, int vertexIndex);

        static const int KEY_SHIFT = 10;
        static int GetKey(int id1, int id2) {
            if (id1 == id2) return -1;
            if (id1 > id2) {
                return (id2 << KEY_SHIFT) + id1;
            }
            else {
                return (id1 << KEY_SHIFT) + id2;
            }
        }
    };
}

