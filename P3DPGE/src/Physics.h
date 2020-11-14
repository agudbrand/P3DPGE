#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Entities.h"

namespace Physics {
	//TODO(ou,delle,11/9/20) optimize by using arrays rather than vectors,
	static std::vector<PhysEntity*> physEntities;
	static std::vector<std::pair<PhysEntity*, PhysEntity*>> collidingEntities;

	static float airFriction = .8f; //completely arbitrary number

	static bool discreteCollision = true;
	static bool paused = false;
	static bool frame = false;

	static void Init() {
		physEntities = std::vector<PhysEntity*>();
	}

	static void Update(float deltaTime) {
		collidingEntities = std::vector<std::pair<PhysEntity*, PhysEntity*>>();
		if (!paused || frame) {
			for (PhysEntity* ptr : physEntities) {
				if (ptr) {
					if (discreteCollision) {
						ptr->AddFrictionForce(nullptr, airFriction);
						ptr->Update(deltaTime);
						for (PhysEntity* target : physEntities) {
							if (target && target->id != ptr->id) {
								if (ptr->CheckCollision(target)) {
									collidingEntities.push_back({ ptr, target });
								}
							}
							else { break; }
						}
					}
				}
				else { break; }
			}
			for (auto pair : collidingEntities) {
				PhysEntity* e1 = pair.first;
				PhysEntity* e2 = pair.second;
				//From wikipedia without rotation
				//https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects

				Vector3 normal = (e2->position - e1->position) / e1->position.distanceTo(e2->position);
				float p = 2.f * (normal.dot(e1->velocity - e2->velocity)) / (e1->mass + e2->mass);
				e1->velocity -= normal * p * e2->mass;
				e2->velocity += normal * p * e1->mass;
			}
			if (frame) { frame = !frame; }
		}
	}

	static void Cleanup() {
		for (PhysEntity*& ptr : physEntities) {
			delete ptr; ptr = nullptr;
		}
		physEntities.clear();
	}

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