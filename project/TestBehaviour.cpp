#include "GameWorld.h"

using namespace std;
using namespace phy;
using namespace glm;
using namespace mmath;

class TB1 : public Behaviour
{
public:
    TB1()
    {
        obj = GameWorld::CreateObject(vec3(0, 5, 0), "_model/env/grass_1.obj");
        obj->SetNewShader(new Shader("_shader/loadmodel.vs", "_shader/line_dsv.fs"));
        obj->BindBehaviour(this);
    }

    void Start() {
        noiseTextId = TextureHelper::load2DTexture("_shader/disolve.png", TEXTURE_FILTER_TYPE_MAL_MILM);
        //noiseTextId = TextureHelper::load2DTexture("_shader/noise.png", TEXTURE_FILTER_TYPE_MAL_MILM);

        obj->shader->Use();
        obj->shader->SetVector3f("stage1_color", MiniMath::GetColorV3(0xffd300));
    }

    void FixedUpdate() {
        dsvThreshold += dsvRate;
        if (dsvThreshold <= 0  || dsvThreshold >= 1.1) {
            dsvRate = -dsvRate;
            //cout << "REVERSED" << endl;
        }
    }

    bool BeforeDraw() {
       obj->shader->Use();

       glActiveTexture(GL_TEXTURE1);
       glBindTexture(GL_TEXTURE_2D, noiseTextId);
       glUniform1i(glGetUniformLocation(obj->shader->programID,
           "texture_noise"), 1);

       glUniform1f(glGetUniformLocation(obj->shader->programID, 
           "disolve_threshold"), dsvThreshold);

       return false;
    }

    void AfterDraw() {
    }

    GLuint noiseTextId;
    float dsvThreshold = 0;
    float dsvRate = 0.005f;
private:
};
