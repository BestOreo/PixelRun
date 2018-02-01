#pragma once
#include "ParticleGenerator.h"
#include "LabelManager.h"

class SourceManager {
public:
	static vector<ParticleGenerator*> ParticleVector;
	static vector<LabelManager*> LabelVector;

    static void AddParticle(ParticleGenerator* p) {
        ParticleVector.push_back(p);
    }
};