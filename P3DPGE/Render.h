#pragma once
#include "olcPixelGameEngine.h"
#include "Entities.h"

namespace Render {
	//vector of pointers to vectors containing unique Enity pointers
	//static std::vector<std::vector<Entity>*> drawnEntities;
	//
	//static size_t AddEntityVector(std::vector<Entity>* entityVector) {
	//	drawnEntities.push_back(entityVector);
	//	return drawnEntities.size();
	//}
	//
	static void Init() {
		//drawnEntities = std::vector<std::vector<Entity>*>();
		//AddEntityVector(&Physics::hotEntities);
		//AddEntityVector(&Physics::coldEntities);
	}

	static void Update(olc::PixelGameEngine* p) {
		p->Clear(olc::BLACK);
		//for (std::vector<Entity>* vecPtr : drawnEntities) {
		//	for (auto &entity : *vecPtr) {
		//		entity.Draw(p);
		//	}
		//}
		for (Entity* ptr : Physics::hotEntities) {
			if (ptr) {
				ptr->Draw(p);
			} else {
				break;
			}
		}
		for (Entity* ptr : Physics::coldEntities) {
			if (ptr) {
				ptr->Draw(p);
			} else {
				break;
			}
		}
	}

	static void Cleanup() {
		//for (auto vecPtr : drawnEntities) {
		//	delete vecPtr;
		//	vecPtr = nullptr;
		//}
		//drawnEntities.clear();
	}

};