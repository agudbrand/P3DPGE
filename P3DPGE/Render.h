#pragma once
#include "olcPixelGameEngine.h"
#include "Entities.h"

namespace Render {
	//vector of pointers to vectors containing unique Enity pointers
	static std::vector<std::vector<std::unique_ptr<Entity>>*> drawnEntities;

	static void Init() {
		drawnEntities = std::vector<std::vector<std::unique_ptr<Entity>>*>();
	}

	static void Update(olc::PixelGameEngine* p) {
		p->Clear(olc::BLACK);
		for (std::vector<std::unique_ptr<Entity>>* vecPtr : drawnEntities) {
			for (std::unique_ptr<Entity> const& entity : *vecPtr) {
				if (entity) {
					entity->Draw(p);
				} else {
					break;
				}
			}
		}
	}

	static void Cleanup() {
		for (auto vecPtr : drawnEntities) {
			delete vecPtr;
			vecPtr = nullptr;
		}
		drawnEntities.clear();
	}

	static size_t AddEntityUptrVector(std::vector<std::unique_ptr<Entity>>* entityPtrVector) {
		drawnEntities.push_back(entityPtrVector);
		return drawnEntities.size();
	}

};