#include "MeshSystem.h"
#include "../math/Math.h"
#include "../geometry/Edge.h"

#include "../components/Mesh.h"
#include "../components/Transform.h"
#include "../components/Physics.h"

#include "../utils/Command.h"
#include "../components/InputSingleton.h"
#include "../components/Camera.h"
#include "../components/Scene.h"
#include "../components/ScreenSingleton.h"

inline void AddSelectEntityCommand(EntityAdmin* admin) {
	admin->commands["select_entity"] = new Command([](EntityAdmin* admin) {
		admin->singletonInput->selectedEntity = nullptr;
		Vector3 pos = Math::ScreenToWorld(admin->singletonInput->mousePos, admin->currentCamera->projectionMatrix, 
											admin->currentCamera->viewMatrix, admin->singletonScreen->dimensions);
		LOG("Screen to world ", pos);
		pos.WorldToLocal(admin->currentCamera->position);
		LOG("World to local  ", pos);
		pos.normalize();
		LOG("Normalize       ", pos);
		pos *= 1000;
		LOG("Times 1000      ", pos);
		pos.LocalToWorld(admin->currentCamera->position);
		LOG("Local to world  ", pos);
		LOG(admin->currentCamera->position);

		//draw ray if debugging
		RenderedEdge3D* ray = new RenderedEdge3D(pos, admin->currentCamera->position);
		DEBUG ray->e = (Entity*)1; //to make it not delete every frame
		DEBUG admin->currentScene->lines.push_back(ray);

		for (Mesh* m : admin->currentScene->meshes) {
			if (MeshSystem::LineIntersect(m, ray)) {
				admin->singletonInput->selectedEntity = m->entity;
				break;
			}
		}

		if (!admin->singletonInput->selectedEntity) { ERROR("No object selected"); }
		//TODO(i,delle) change this to take in an ID once we figure that out
	}, "select_entity", "select_entity <EntityID>");
}

void MeshSystem::Init() {
	AddSelectEntityCommand(admin);
}

void MeshSystem::Update() {
	for(auto e : admin->entities) {
		Mesh* m = 0;
		Transform* t = 0;
		Physics* p = 0;
		for(auto c : e.second->components) {
			if(Mesh* mesh = dynamic_cast<Mesh*>(c)) {
				m = mesh;
			} else if(Transform* transform = dynamic_cast<Transform*>(c)) {
				t = transform;
			} else if(Physics* physics = dynamic_cast<Physics*>(c)) {
				p = physics;
			}
		}
		if(m && t && p) {
			if(t->position != t->prevPosition) {
				TranslateMesh(m, t->position - t->prevPosition);
				t->prevPosition = t->position;
			}
			if(t->rotation != t->prevRotation) {
				RotateMesh(m, Matrix4::RotationMatrixAroundPoint(t->position, t->rotation - t->prevRotation));
				t->prevRotation = t->rotation;
			}
			break;
		}
	}
}

bool MeshSystem::LineIntersect(Mesh* mesh, Edge3D* line) {
	for(Triangle& t : mesh->triangles) {
		if(t.line_intersect(line)) { return true; }
	}
	return false;
}

void MeshSystem::TranslateMesh(Mesh* mesh, Vector3 translation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p += translation;
		}
	}
}

void MeshSystem::RotateMesh(Mesh* mesh, Matrix4 rotation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= rotation;
		}
	}
}

void MeshSystem::ScaleMesh(Mesh* mesh, Matrix3 scale) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= scale; //TODO(,delle) test 3x3 scale matrices
		}
	}
}

void MeshSystem::TransformMesh(Mesh* mesh, Matrix4 transformation) {
	for(auto& t : mesh->triangles) {
		for(auto& p : t.points) {
			p *= transformation;
		}
	}
}

