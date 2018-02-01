#ifndef _MINI_MATH_H_
#define _MINI_MATH_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

using namespace glm;

#define SQR(x) (x*x)
#define random(x) (rand()%(x))
#define FLOAT_PRECISION 10000.0f
#define MINIUM_FLOAT 0.0001f

namespace mmath {
    class MiniMath {
    public:
        static float SqrDist(vec2 p1, vec2 p2) {
            return SQR((p1.x - p2.x)) + SQR((p1.y - p2.y));
        }

        static double SqrDist(vec3 p1, vec3 p2) {
            return SQR((p1.x - p2.x)) + SQR((p1.y - p2.y)) +
                SQR((p1.z - p2.z));
        }

        static float Dist(vec3 p1, vec3 p2) {
            return sqrt(SqrDist(p1, p2));
        }

        static float GetRandomFloat(float mainValue, float randomPercent) {
            return mainValue * (1 + GetRandomFloatBetween(-randomPercent, randomPercent));
        }

        // left-include & right-include
        static float GetRandomFloatBetween(float leftValue, float rightValue) {
            int tmp = (int)((rightValue - leftValue) * FLOAT_PRECISION) + 1;
            return leftValue + random(tmp) / FLOAT_PRECISION;
        }

        static vec4 GetColor(int value, float alpha = 1) {
            vec4 color;
            color.r = ((value >> 16) & 0xFF) * 1.0f / 0xFF;
            color.g = ((value >> 8) & 0xFF) * 1.0f / 0xFF;
            color.b = (value & 0xFF) * 1.0f / 0xFF;
            color.a = alpha;
            return color;
        }

        static vec3 GetColorV3(int value) {
            vec3 color;
            color.r = ((value >> 16) & 0xFF) * 1.0f / 0xFF;
            color.g = ((value >> 8) & 0xFF) * 1.0f / 0xFF;
            color.b = (value & 0xFF) * 1.0f / 0xFF;
            return color;
        }

        static vec3 ToVec3(vec2 v) {
            return vec3(v.x, 0, v.y);
        }

        static vec3 ToVec3(float x, float y) {
            return vec3(x, 0, y);
        }

        static vec2 ToVec2(vec3 v) {
            return vec2(v.x, v.z);
        }
    };
}

#endif