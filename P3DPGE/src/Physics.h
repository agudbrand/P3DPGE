#pragma once
#include "Entities.h"
#include <tuple>

namespace Physics {
	//TODO(ou,delle,11/9/20) optimize by using arrays rather than vectors,
	static std::vector<PhysEntity*> physEntities;

	static float airFriction = .2f; //completely arbitrary number

	static bool discreteCollision = true;
	static bool paused = false;
	static bool frame = false;

	static Timer* timer;

	static void Init() {
		physEntities = std::vector<PhysEntity*>();
		timer = new Timer;
		TIMER_S;
	}

	static void Update(float deltaTime) {
		if (!paused || frame) {
			float dTime = TIMER_GET;
			if (dTime > g_fixedDeltaTime) { TIMER_E; }
			for (PhysEntity* ptr : physEntities) {
				if (ptr) {
					if (discreteCollision) {
						//TODO(p, sushi) figure out why friction does what its doing in 3D

						ptr->PhysUpdate(dTime);
						BUFFERLOG(7, F_AVG(10, dTime));
						//
						//for (PhysEntity* target : physEntities) {
						//	if (target && target->id != ptr->id) {
						//		if (ptr->CheckCollision(target)) {
						//			ptr->ResolveCollision(target);
						//		}
						//	}
						//	else { break; }
						//}
					}
				}
				else { break; }
			}
			if (frame) { frame = !frame; }
			if (dTime > g_fixedDeltaTime) { TIMER_S; }
		}
	}

	static void Cleanup() {
		for (PhysEntity*& ptr : physEntities) {
			delete ptr; ptr = nullptr;
		}
		physEntities.clear();
	}

	//TODO(upo,delle,11/17/20) this could maybe be optimized by having an id counter instead of checking back element
	static size_t AddEntity(PhysEntity* entity) {
		if (physEntities.size() > 0) {
			entity->id = physEntities.back()->id + 1;
			physEntities.push_back(entity);
			return physEntities.size();
		}
		else {
			physEntities.push_back(entity);
			return physEntities.size();
		}
	}
};