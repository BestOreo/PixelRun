#ifndef _BEHAVIOUR_H_
#define _BEHAVIOUR_H_

#include <iostream>

class GameObject;
namespace phy {
    class Collider;
}

using namespace std;

class Behaviour
{
public:
    GameObject *obj;

    virtual void Start() {}
    virtual void Update() {}
    virtual void FixedUpdate() {}
    virtual void OnDestory() {}

    virtual void OnEnable() {}
    virtual void OnDisable() {}

    virtual void OnCollisionEnter(phy::Collider *other) {
       // cout << "OnCollisionEnter" << endl;
    }
    virtual void OnCollisionStay(phy::Collider *other) {
       // cout << "OnCollisionStay" << endl;
    }
    virtual void OnCollisionExit(phy::Collider *other) {
       // cout << "OnCollisionExit" << endl;
    }

    // true means behaviour will totally render model,
    // GameObject::Draw() will return after get true.
    virtual bool BeforeDraw() { return false; };
    virtual void AfterDraw() {};

private:

};

#endif