#pragma once
#include "Entities.h"
#include <tuple>

namespace Physics {
	//TODO(ou,delle,11/9/20) optimize by using arrays rather than vectors,
	static std::vector<PhysEntity*> physEntities;

	static float airFriction = .8f; //completely arbitrary number

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
			if (TIMER_GET > g_fixedDeltaTime) {
				TIMER_E;
				BUFFERLOG(TIMER_GET);
				for (PhysEntity* ptr : physEntities) {
					if (ptr) {
						if (discreteCollision) {
							ptr->AddFrictionForce(nullptr, airFriction, deltaTime);
							ptr->Update(deltaTime);
							for (PhysEntity* target : physEntities) {
								if (target && target->id != ptr->id) {
									if (ptr->CheckCollision(target)) {
										ptr->ResolveCollision(target);
									}
								}
								else { break; }
							}
						}
					}
					else { break; }
				}
				if (frame) { frame = !frame; }
				TIMER_S;
			}
			else {
				for (PhysEntity* ptr : physEntities) {
					ptr->Interpolate(TIMER_GET / g_fixedDeltaTime);
				}
			}
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