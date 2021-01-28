#include "MeshSystem.h"
#include "ConsoleSystem.h"
#include "../math/Math.h"
#include "../geometry/Edge.h"

#include "../components/Mesh.h"
#include "../components/Transform.h"
#include "../components/Physics.h"

#include "../utils/Command.h"
#include "../components/Input.h"
#include "../components/Camera.h"
#include "../components/Scene.h"
#include "../components/Screen.h"

inline void AddSelectEntityCommand(EntityAdmin* admin) {
	admin->commands["select_entity"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->input->selectedEntity = nullptr;

		//ortho proj matrices can't be inverted so need to set up a method for selecting
		//when using ortho
		//plus I think I need to fix selecting objects period so
		//TODO(i, sushi) fix selecting objects
		if (!USE_ORTHO) {
			Vector3 pos = Math::ScreenToWorld(admin->input->mousePos, admin->currentCamera->projectionMatrix,
				admin->currentCamera->viewMatrix, admin->screen->dimensions);
			pos *= Math::WorldToLocal(admin->currentCamera->position);
			pos.normalize();
			pos *= 1000;
			pos *= Math::LocalToWorld(admin->currentCamera->position);

			//draw ray if debugging
			RenderedEdge3D* ray = new RenderedEdge3D(pos, admin->currentCamera->position);
			ray->e = (Entity*)1; // to make it not delete every frame
			admin->currentScene->lines.push_back(ray);
			
			for (Mesh* m : admin->currentScene->meshes) {
				if (MeshSystem::LineIntersect(m, ray)) {
					admin->input->selectedEntity = m->entity;
					break;
				}
			}
		}
		else {
			LOG("\nWarning: ScreenToWorld not yet implemented for orthographic projection. World interaction with mouse will not work.\n");
		}
		
		if (!admin->input->selectedEntity) { CERROR("No object selected"); }
		return "";
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
			} 
			else if(Transform* transform = dynamic_cast<Transform*>(c)) {
				t = transform;
			}
		}
		if(m && t) {
			RotateMesh(m, Matrix4::RotationMatrix(t->rotation));
			TranslateMesh(m, t->position);
			
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
		for (int i = 0; i < 3; i++) {
			t.points[i] += translation;
		}
	}
}

void MeshSystem::RotateMesh(Mesh* mesh, Matrix4 rotation) {
	for (auto& t : mesh->triangles) {
		BUFFERLOG(0, t);
		for (int i = 0; i < 3; i++) {
			
			t.points[i] = t.poffsets[i] * rotation;
		}
		BUFFERLOG(0, t);
	}
}

void MeshSystem::ScaleMesh(Mesh* mesh, Matrix3 scale) {
	
	for (auto& t : mesh->triangles) {
		for (auto& p : t.points) {
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

