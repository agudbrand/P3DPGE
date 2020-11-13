#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Entities.h"


namespace Physics {
		//TODO(ou,delle,11/9/20) optimize by using arrays rather than vectors, 
		static std::vector<PhysEntity*> physEntities;
		static std::vector<std::pair<PhysEntity*, PhysEntity*>> collidingEntities;

		static float airFriction = .5f; //completely arbitrary number
		
		static bool discreteCollision = true;
		static bool paused = false;
		static bool frame = false;

		static void Init() {
			physEntities = std::vector<PhysEntity*>();
		}

		//TODO(sp,delle,11/13/20) add friction
		static void Update(float deltaTime) {
			collidingEntities = std::vector<std::pair<PhysEntity*, PhysEntity*>>();
			if (!paused || frame) {
				for (PhysEntity* ptr : physEntities) {
					if (ptr) {
						if (discreteCollision) {
							ptr->Update(deltaTime);
							//ptr->AddForce(nullptr, -(ptr->velocity) * airFriction * deltaTime);
							for (PhysEntity* target : physEntities) {
								if (target && target->id != ptr->id) {
									if (ptr->CheckCollision(target)) {
										collidingEntities.push_back({ptr, target});
									}
								} else { break; }
							}
						}
					} else { break; }
				}
				for (auto pair : collidingEntities) {
					
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
			} else {
				physEntities.push_back(entity);
				return physEntities.size();
			}
		}

};