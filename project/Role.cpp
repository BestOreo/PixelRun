#include "Role.h"
#include "GameManager.h"
#include "TimerManager.h"

using namespace std;
using namespace phy;
using namespace glm;
using namespace mmath;

void Player::TestForTimer() {
    cout << "Time Over" << endl;
}
void Player::Recover_SpeedUp() {
    if (SpeedUp) {
        SpeedUp = false;
        ChangeSpeed(speed - 3.5f);
    }

    cout << "SpeedUp Time Over" << endl;
}
void Player::Recover_Reverse() {
    if (ReverseState == 1) {
        ReverseState = 0;
    }

    cout << "Reverse Time Over" << endl;
}
void Player::Recover_Invincible() {
    if (Invincible) {
        Invincible = false;
    }

    cout << "Invincible Time Over" << endl;
}

void Player::Recover_Frozen() {
    if (Frozen) {

        obj->velocity = oriVel;
        for (int i = 0; i < team.size(); i++) {
            team[i]->obj->shader = new Shader("_shader/loadmodel.vs", "_shader/loadmodel.fs");
        }
        Frozen = false;
    }
    cout << "Frozen Time Over" << endl;
}

void Player::Freeze() {
    Frozen = true;
    oriVel = obj->velocity;
    obj->velocity = vec3(0);
}


void Player::AddTimer_SpeedUp() {
    TimerManager::AddTimer_Player(this, &Player::Recover_SpeedUp, 3);
}
void Player::AddTimer_Reverse() {
    TimerManager::AddTimer_Player(this, &Player::Recover_Reverse, 3);
}
void Player::AddTimer_Invincible() {
    TimerManager::AddTimer_Player(this, &Player::Recover_Invincible, 3);
}
void Player::AddTimer_Frozen() {
    TimerManager::AddTimer_Player(this, &Player::Recover_Frozen, 3);
}
void Player::AddTimer_Frozen(float time) {
	TimerManager::AddTimer_Player(this, &Player::Recover_Frozen, time);
}
void Player::ChangeSpeed(float value) {
    //cout << "Speed Change!" << endl;


    speed = value;
    if (Frozen == false) {
        switch (lastCommand)
        {
        case GLFW_KEY_W: {
            if (ReverseState == 0)
                obj->velocity = vec3(0, 0, -speed);
            else
                obj->velocity = vec3(0, 0, speed);
            break;
        }
        case GLFW_KEY_S: {
            if (ReverseState == 0)
                obj->velocity = vec3(0, 0, speed);
            else
                obj->velocity = vec3(0, 0, -speed);
            break;
        }
        case GLFW_KEY_A: {
            if (ReverseState == 0)
                obj->velocity = vec3(-speed, 0, 0);
            else
                obj->velocity = vec3(speed, 0, 0);
            break;
        }
        case GLFW_KEY_D: {
            if (ReverseState == 0)
                obj->velocity = vec3(speed, 0, 0);
            else
                obj->velocity = vec3(-speed, 0, 0);
            break;
        }
        default:
            break;
        }
    }
}


void Character::SetOwner(Player *owner, bool isLeader /*= false*/) {
    this->owner = owner;

    this->isLeader = isLeader;
    if (isLeader) {
        obj->AddCollider(new Collider(radius, phy::LEADER));
        destination = nullptr;
        obj->velocity = vec3(0);

        obj->trans->position = owner->obj->trans->position;
        normalDir = GetNormalDir(owner->obj->velocity);
    }
    else
    {
        obj->AddCollider(new Collider(radius, phy::TEAMMATE));
        destination = owner->path.front;
        obj->velocity = vec3(0);

        obj->trans->position = owner->team.back()->GetBehindPos();
        normalDir = owner->team.back()->normalDir;
    }
    AdjustRotation();
}

void Character::Move()
{
    if (owner != nullptr) {
        if (isLeader) {
            obj->trans->position = owner->obj->trans->position;

            if (needCalcDir) {
                normalDir = GetNormalDir(owner->obj->velocity);
                AdjustRotation();
                needCalcDir = false;
            }
        }
        else {
            MoveToDest(GameWorld::GetFixedDeltaTime());
        }

    }
}

void Character::OnCollisionEnter(Collider *other)
{
    if (isLeader) {
        if (other->layer == phy::STRANGER) {
            owner->newTeammate.push_back((Character *) (other->beha));
        }
    }
}

glm::vec3 Character::GetBehindPos()
{
    return obj->trans->position - normalDir * owner->gap;
}

void Character::BackToPool() {
	this->BeginDsv(0.8f);
	
	
	//this->obj->SetActive(false);
}
void Character::FixedUpdate()
{
    if (inDsv) {
        dsvThreshold += dsvRate;
        if (dsvThreshold >= 1.1) {
            EndDsv();
            return;
        }
    }
}

void Character::MoveToDest(float time)
{
    float totalDistCanMove = owner->speed * time;

    if (destination == NULL) {
        if (needCalcDir) {
            normalDir = GetNormalDir(owner->obj->trans->position, obj->trans->position);
            AdjustRotation();
            needCalcDir = false;
        }
        obj->trans->position += totalDistCanMove * normalDir;
        needCalcDir = true;
        return;
    }


    float curDist = MiniMath::Dist((*destination).content, obj->trans->position);

    if (needCalcDir) {
        normalDir = GetNormalDir((*destination).content, obj->trans->position);
        AdjustRotation();
        needCalcDir = false;
    }

    if (totalDistCanMove < curDist) {
        obj->trans->position += totalDistCanMove * normalDir;
    }
    else {
        obj->trans->position = (*destination).content;
        time -= curDist / owner->speed;
        destination = destination->next;
        if (index == owner->team.size() - 1) {
            owner->path.PopFront();
        }
        needCalcDir = true;
        MoveToDest(time);
    }
}


void Character::InitDsv()
{
    oriShader = obj->shader;
    dsvShader = GameWorld::Instance->dsvShader;
    dsvTextId = GameWorld::Instance->dsvTextId;
}

void Character::BeginDsv(float time)
{
    inDsv = true;
    dsvThreshold = 0.2;
    dsvRate = 1.0f / time * GameWorld::GetFixedDeltaTime();
    obj->shader = dsvShader;

    stage1_color = MiniMath::GetColorV3(0xffd300);
    stage2_color = MiniMath::GetColorV3(0xa60400);
    stage3_color = MiniMath::GetColorV3(0x333333);

    dsvShader->Use();
    dsvShader->SetVector3f("stage1_color", stage1_color);
    dsvShader->SetVector3f("stage2_color", stage2_color);
    dsvShader->SetVector3f("stage3_color", stage3_color);
}

bool Character::BeforeDraw()
{
    if (!inDsv) return false;

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    obj->shader->Use();

    glUniform1f(glGetUniformLocation(obj->shader->programID, "disolve_threshold"),
        dsvThreshold);
    glUniform1f(glGetUniformLocation(obj->shader->programID, "stage3_threshold"),
        dsvThreshold + step3);
    glUniform1f(glGetUniformLocation(obj->shader->programID, "stage2_threshold"),
        dsvThreshold + step3 + step2);
    glUniform1f(glGetUniformLocation(obj->shader->programID, "stage1_threshold"),
        dsvThreshold + step3 + step2 + step1);

    return false;
}



void Character::AfterDraw()
{
    if (!inDsv) return;

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Character::EndDsv()
{
    inDsv = false;
    obj->shader = oriShader;
    obj->SetActive(false);
    // back pool

}

// 逻辑上等同于先抛弃，再溶解
void Player::KillTeammate(GLfloat Z) {
    if (obj->trans->position.z > Z) {
		GameWorld::GameOver();
    }
    else {
        int numLeftChr = team.size();
        for (int i = team.size() - 1; i >= 1; i--) {
            if (team[i]->obj->trans->position.z > Z) {
                numLeftChr = i;
            }
        }
        int numDeadChr = team.size() - numLeftChr;
        for (size_t i = 0; i < numDeadChr; i++)
        {
            KillLastTeammate();
        }
        //cout << "numDeadChr: " << numDeadChr << endl;
        //cout << "Now path: " << path.count << endl;
    }
}

void Player::KillLastTeammate()
{
    cout << "kill" << endl;
    if (team.size() > 1) {
        Character* lastone = team.back();

        if (lastone->destination != NULL) {
            auto aheadTm = team[team.size() - 2];
            if (aheadTm->destination == nullptr) {
                path.Clear();
            }
            else {
                auto node = lastone->destination;
                while (true) {
                    if (node == aheadTm->destination) {
                        break;
                    }
                    node = node->next;
                    path.PopFront();
                }
            }
        }

		lastone->BackToPool();
        team.pop_back();
		
    }

}

void Player::FixedUpdate() {
	
	if (inland == 0&&goin!=0)
		GameWorld::GameOver();
		//cout << "inland " << inland << endl;
    //if (lastpath != path.count) {
        //cout << "PPPP:" << path.count << endl;
        //lastpath = path.count;
    //}

    DealNewTeammate();
    ChangeVelocity();

    for (int i = 0; i < team.size(); i++) {
        if (Frozen == false) {
            team[i]->Move();
        }
    }
}
