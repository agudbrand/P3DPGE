#include "WorldSystem.h"
#include "../EntityAdmin.h"
#include "../utils/Debug.h"

#include "../components/WorldSingleton.h"

Entity* WorldSystem::CreateEntity(Component* singleton) {
	WorldSingleton* world = admin->singletonWorld;
	Entity* e = new Entity;
	e->components.push_back(singleton);
	world->creationBuffer.push_back(e);
	return e;
}

Entity* WorldSystem::CreateEntity(std::vector<Component*> components) {
	WorldSingleton* world = admin->singletonWorld;
	Entity* e = new Entity;
	e->components = components;
	world->creationBuffer.push_back(e);
	return e;
}

int32 WorldSystem::AddEntityToDeletionBuffer(Entity* entity) {
	WorldSingleton* world = admin->singletonWorld;
	try {
		admin->entities.at(entity->id);
		world->deletionBuffer.push_back(entity);
		return world->deletionBuffer.size()-1;
	} catch(const std::out_of_range& oor) {
		return -1;
	}
}

int32 WorldSystem::AddAComponentToEntity(Entity* entity, Component* component) {
	WorldSingleton* world = admin->singletonWorld;
	try {
		Entity* e = admin->entities.at(entity->id);
		e->components.push_back(component);
		return e->components.size()-1;
	} catch(const std::out_of_range& oor) {
		return -1;
	}
}

std::vector<Component*>* WorldSystem::GetComponentsOnEntity(Entity* entity) {
	WorldSingleton* world = admin->singletonWorld;
	try {
		return &admin->entities.at(entity->id)->components;
	} catch(const std::out_of_range& oor) {
		return 0;
	}
}

bool WorldSystem::RemoveAComponentFromEntity(Entity* entity, Component* component) {
	WorldSingleton* world = admin->singletonWorld;
	try {
		std::vector<Component*>* components = &admin->entities.at(entity->id)->components;
		for(int i = 0; i < components->size(); ++i) {
			if(components->at(i) == component) {
				delete components->at(i);
				components->erase(components->begin()+i);
				return true;
			}
		}
		return false;
	} catch(const std::out_of_range& oor) {
		return false;
	}
}

void WorldSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	WorldSingleton* world = admin->singletonWorld;

	//deletion buffer
	for(Entity* entity : world->deletionBuffer) {
		uint32 id = entity->id;
		try {
			delete admin->entities.at(id);
		} catch(const std::out_of_range& oor) {
			ASSERT(false, "No entity at id when there should have been");
		}
		admin->entities.erase(id);
	}
	world->deletionBuffer.clear();

	//creation buffer
	for(Entity* entity : world->creationBuffer) {
		entity->id = !admin->entities.empty() ? admin->entities.rbegin()->second->id + 1 : 1; //set id to be one greater than the last
		admin->entities.insert(admin->entities.end(), {entity->id, entity}); //TODO(o,delle) see if this is worth it vs regular insert
		entity->admin = admin;
	}
	world->creationBuffer.clear();
}
