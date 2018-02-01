#pragma once
#include "Role.h"
#include <vector>
#include "GameManager.h"
#include "ParticleGenerator.h"
#include "Trap.h"
class PlayerTimer {
public:
	Player *player;
	void (Player::*funcpoint)();
	GLfloat duration;
};

class ParticlesTimer {
public:
	ParticleGenerator* master;
	void (ParticleGenerator::*funcpoint)();
	GLfloat duration;
};

class TrapTimer {
public:
	Trap* trap;
	void (Trap::*funcpoint)();
	GLfloat duration;
};

class TimerManager {
public:

	static vector<PlayerTimer*> PlayerTimers;
	static vector<ParticlesTimer*> ParticlesTimers;
	static vector<TrapTimer*> TrapTimers;
	static void AddTimer_Player(Player* player, void (Player::*p)(void), GLfloat duration) {
		cout << "Player timer: " << endl;
		PlayerTimer *a = new PlayerTimer();
		a->player = player;
		a->duration = duration;
		a->funcpoint = p;
		PlayerTimers.push_back(a);
	}

	static void AddTimer_Particles(ParticleGenerator* masterpoint , void (ParticleGenerator::*functionpointer)(void), GLfloat duration) {
		cout << "Particles timer: " << endl;
		ParticlesTimer *a = new ParticlesTimer();
		a->master = masterpoint;
		a->duration = duration;
		a->funcpoint = functionpointer;
		ParticlesTimers.push_back(a);
	}
	static void AddTimer_Traps(Trap* trap, void (Trap::*p)(void), GLfloat duration) {
		cout << "Particles timer: " << endl;
		TrapTimer *a = new TrapTimer();
		a->trap = trap;
		a->duration = duration;
		a->funcpoint = p;
		TrapTimers.push_back(a);
	}
	static void Tick(GLfloat dt) {

		vector<PlayerTimer*>::iterator it = PlayerTimers.begin();
		for (it = PlayerTimers.begin(); it != PlayerTimers.end();) {
			(*it)->duration -= dt;
			//cout << (*it)->duration << endl;
			if ((*it)->duration < 0) {
				Player* player = (*it)->player;
				(player->*(*it)->funcpoint)();
				it = PlayerTimers.erase(it);
			}
			else {
				it++;
			}
		}

		// Timer Loop for particle system
		for (int i = 0; i < ParticlesTimers.size(); i++) {
			ParticlesTimers[i]->duration -= dt;
			//cout << i << " : " << ParticlesTimers[i]->duration << endl;
			if (ParticlesTimers[i]->duration < 0) {
				ParticleGenerator* master = ParticlesTimers[i]->master;
				(master->*ParticlesTimers[i]->funcpoint)(); // execute the function being pointed
				ParticlesTimers[i]->duration = 5;
				//ParticlesTimers.pop_back();
			}
		}

		// Timer Loop for particle system
		for (int i = 0; i < TrapTimers.size(); i++) {
			TrapTimers[i]->duration -= dt;
			//cout << i << " : " << ParticlesTimers[i]->duration << endl;
			if (TrapTimers[i]->duration < 0) {
				Trap* trap = TrapTimers[i]->trap;
				(trap->*TrapTimers[i]->funcpoint)(); // execute the function being pointed
				TrapTimers[i]->duration = 5;
				//ParticlesTimers.pop_back();
			}
		}
	};

	// 其余陷阱类的timer到时候再写
};




