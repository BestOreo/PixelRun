#include "Physicis.h"
#include "GameObject.h"

using namespace std;
using namespace mmath;

namespace phy {
    void PhyManager::Tick()
    {
        for (auto ruleIr = ins->rules.cbegin(); ruleIr != ins->rules.cend(); ruleIr++) {
            list<Collider *> *l1 = ins->collidersOfLayer[(*ruleIr)->first];
            list<Collider *> *l2 = ins->collidersOfLayer[(*ruleIr)->second];
            for (auto col1Ir = l1->cbegin(); col1Ir != l1->cend(); col1Ir++) {
                for (auto col2Ir = l2->cbegin(); col2Ir != l2->cend(); col2Ir++) {
                    int key = GetKey((*col1Ir)->GetId(), (*col2Ir)->GetId());
                    auto itr = ins->collisionState.find(key);
                    if (IsCollided(*col1Ir, *col2Ir)) {
                        if (itr == ins->collisionState.end()) {
                            ins->collisionState.insert(make_pair(key, ENTER));
                            if ((*col1Ir)->beha != nullptr) {
                                (*col1Ir)->beha->OnCollisionEnter(*col2Ir);
                            }
                            if ((*col2Ir)->beha != nullptr) {
                                (*col2Ir)->beha->OnCollisionEnter(*col1Ir);
                            }
                        }
                        else
                        {
                            ins->collisionState[key] = STAY;
                            if ((*col1Ir)->beha != nullptr) {
                                (*col1Ir)->beha->OnCollisionStay(*col2Ir);
                            }
                            if ((*col2Ir)->beha != nullptr) {
                                (*col2Ir)->beha->OnCollisionStay(*col1Ir);
                            }
                        }
                    }
                    else
                    {
                        if (itr != ins->collisionState.end()) {
                            ins->collisionState.erase(key);
                            if ((*col1Ir)->beha != nullptr) {
                                (*col1Ir)->beha->OnCollisionExit(*col2Ir);
                            }
                            if ((*col2Ir)->beha != nullptr) {
                                (*col2Ir)->beha->OnCollisionExit(*col1Ir);
                            }
                        }
                    }
                }
            }
        }
    }

    bool PhyManager::IsCollided(Collider *col1, Collider *col2)
    {
        if (col1->obj->GetActive() == false ||
            col2->obj->GetActive() == false ||
            col1 == col2) {
            return false;
        }

        if (col1->shape == CIRCLE) {
            if (col2->shape == CIRCLE) {
                return IsCollidedCC(col1, col2);
            }
            else
            {
                return IsCollidedCR(col1, col2);
            }
        }
        else
        {
            if (col2->shape == CIRCLE) {
                return IsCollidedCR(col2, col1);
            }
            else
            {
                return IsCollidedRR(col1, col2);
            }
        }
    }

    bool PhyManager::IsCollidedCC(Collider *cir1, Collider *cir2)
    {
        float distSqr = SQR((cir1->trans->position.x - cir2->trans->position.x)) +
            SQR((cir1->trans->position.z - cir2->trans->position.z));
        if (distSqr <= SQR((cir1->size.x + cir2->size.x))) {
            return true;
        }
        return false;
    }

    bool PhyManager::IsCollidedRR(Collider *rect1, Collider *rect2)
    {
        vec2 lbP1 = GetLbPoint(rect1);
        vec2 lbP2 = GetLbPoint(rect2);

        if (lbP1.x >= lbP2.x && lbP1.x >= lbP2.x + rect2->size.x) {
            return false;
        }
        else if (lbP1.x <= lbP2.x && lbP1.x + rect1->size.x <= lbP2.x) {
            return false;
        }
        else if (lbP1.y >= lbP2.y && lbP1.y >= lbP2.y + rect2->size.y) {
            return false;
        }
        else if (lbP1.y <= lbP2.y && lbP1.y + rect1->size.y <= lbP2.y) {
            return false;
        }

        return true;
    }

    bool PhyManager::IsCollidedCR(Collider *cir, Collider *rect)
    {
        vec2 lbPoint = GetLbPoint(rect);

        vec2 pos;
        pos.x = cir->trans->position.x;
        pos.y = cir->trans->position.z;

        vec2 nearPos;

        if (pos.x <= lbPoint.x) {
            nearPos.x = lbPoint.x;
        }
        else if (pos.x >= lbPoint.x + rect->size.x) {
            nearPos.x = lbPoint.x + rect->size.x;
        }
        else {
            nearPos.x = pos.x;
        }

        if (pos.y <= lbPoint.y) {
            nearPos.y = lbPoint.y;
        }
        else if (pos.y >= lbPoint.y + rect->size.y) {
            nearPos.y = lbPoint.y + rect->size.y;
        }
        else {
            nearPos.y = pos.y;
        }

        float dist = MiniMath::SqrDist(pos, nearPos);
        if (dist <= SQR(cir->size.x)) {
            return true;
        }
        else
        {
            return false;
        }
    }

    glm::vec2 PhyManager::GetRectVertex(Collider *rect, int vertexIndex)
    {
        // vertexIndex: 0~3, left-top, right-top, right-bottom, left-bottom
        if (rect->shape == CIRCLE) {
            return XZ(rect->trans->position);
        }

        vec2 result;
        switch (vertexIndex)
        {
            case 0: {
                    result.x = rect->trans->position.x - rect->size.x / 2;
                    result.y = rect->trans->position.z + rect->size.y / 2;
                    break;
                }
            case 1: {
                    result.x = rect->trans->position.x + rect->size.x / 2;
                    result.y = rect->trans->position.z + rect->size.y / 2;
                    break;
                }
            case 2: {
                    result.x = rect->trans->position.x + rect->size.x / 2;
                    result.y = rect->trans->position.z - rect->size.y / 2;
                    break;
                }
            case 3: {
                    result.x = rect->trans->position.x - rect->size.x / 2;
                    result.y = rect->trans->position.z - rect->size.y / 2;
                    break;
                }
            default: {
                    result = XZ(rect->trans->position);
                    break;
                }
        }

        return result;
    }

    Collider::Collider(float radius, COLLIDER_LAYER layer)
    {
        Init(layer);

        shape = CIRCLE;
        size.x = radius;
    }

    Collider::Collider(float width, float height, COLLIDER_LAYER layer)
    {
        Init(layer);

        shape = RECT;
        size.x = width;
        size.y = height;
    }

    void Collider::Init(COLLIDER_LAYER layer)
    {
        id = nexdId;
        nexdId++;

        this->layer = layer;

        PhyManager::AddCollider(this);
    }

    Collider::~Collider()
    {
        PhyManager::RemoveCollider(this);
    }
}

