#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"
#include "Entities.h"


namespace Physics {
		//TODO: optimize by using arrays rather than vectors, 
		static std::vector<PhysEntity*> hotEntities;
		static std::vector<PhysEntity*> coldEntities;

		static float airFriction = 5; //completely arbitrary number

		static bool paused = false;
		static bool frame = false;

		static void Init() {
			hotEntities = std::vector<PhysEntity*>();
			coldEntities = std::vector<PhysEntity*>();
		}

		//TODO add friction
		static void Update(float deltaTime) {
			if (!paused || frame) {
				for (PhysEntity* ptr : hotEntities) {
					if (ptr) {
						//ptr->AddForce(nullptr, -(ptr->velocity).normalized() * airFriction);
						ptr->Update(deltaTime);
					} else { break; }
				}
				for (PhysEntity* ptr : coldEntities) {
					if (ptr) {
						//ptr->AddForce(nullptr, -(ptr->velocity).normalized() * airFriction);
						ptr->Update(deltaTime);
					} else { break; }
				}
				if (frame) { frame = !frame; }
			}
		}
		
		static void Cleanup() {
			for (PhysEntity*& ptr : hotEntities) {
				delete ptr; ptr = nullptr;
			}
			hotEntities.clear();
			for (PhysEntity*& ptr : coldEntities) {
				delete ptr; ptr = nullptr;
			}
			coldEntities.clear();
		}

		//TODO: check if max amount of entities is in hot
		static size_t AddEntity(PhysEntity* entity, bool bHot = false) {
			if (bHot) {
				hotEntities.push_back(entity);
				return hotEntities.size();
			} else {
				coldEntities.push_back(entity);
				return coldEntities.size();
			}
		}

};