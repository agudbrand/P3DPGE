#include "CommandSystem.h"
#include "../internal/olcPixelGameEngine.h"
#include "../EntityAdmin.h"

#include "../components/InputSingleton.h"
#include "../components/Command.h"
#include "../components/Transform.h"
#include "../components/Mesh.h"
#include "../components/TimeSingleton.h"
#include "../components/Scene.h"

inline void AddSpawnCommands(EntityAdmin* admin) {
	admin->commands["spawn_box"] = new Command([](EntityAdmin* admin) {
		Entity* box = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = MeshSystem::BuildBoxMesh(box, t, Vector3::ONE);
		WorldSystem::AddComponentsToEntity(box, {t, m});
	}, "spawn_box", "spawn_box <halfDimensions: Vector3> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = MeshSystem::BuildComplexMesh(c, t, "objects/bmonkey.obj", false);
		WorldSystem::AddComponentsToEntity(c, {t, m});
	}, "spawn_complex", "spawn_box <filePath: String> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex1"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = MeshSystem::BuildComplexMesh(c, t, "objects/whale_ship.obj", false);
		WorldSystem::AddComponentsToEntity(c, {t, m});
	}, "spawn_complex1", "spawn_box <filePath: String> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex2"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = MeshSystem::BuildComplexMesh(c, t, "objects/24K_Triangles.obj", false);
		WorldSystem::AddComponentsToEntity(c, {t, m});
	}, "spawn_complex2", "spawn_box <filePath: String> <position: Vector3> [rotation: Vector3] [scale: Vector3]");
}

inline void AddTimeCommands(EntityAdmin* admin) {
	admin->commands["time_pause_engine"] = new Command([](EntityAdmin* admin) {
		admin->singletonTime->paused = !admin->singletonTime->paused;
		if(admin->singletonTime->paused) {
			admin->singletonTime->deltaTime = 0.f;
		}
	}, "time_pause_engine", "time_pause_engine");

	admin->commands["time_next_frame"] = new Command([](EntityAdmin* admin) {
		if(admin->singletonTime->paused) {
			admin->singletonTime->frame = true;
		}
	}, "time_next_frame", "time_next_frame");
}

inline void AddRenderCommands(EntityAdmin* admin) {
	admin->commands["render_wireframe"] = new Command([](EntityAdmin* admin) {
		admin->tempScene->WIRE_FRAME = !admin->tempScene->WIRE_FRAME;
	}, "render_wireframe", "render_wireframe");

	admin->commands["render_wireframe_notexture"] = new Command([](EntityAdmin* admin) {
		admin->tempScene->WIRE_FRAME_NO_TEXTURE = !admin->tempScene->WIRE_FRAME_NO_TEXTURE;
		admin->tempScene->LOCAL_AXIS = !admin->tempScene->LOCAL_AXIS;
	}, "render_wireframe_notexture", "render_wireframe_notexture");

	admin->commands["render_display_edges"] = new Command([](EntityAdmin* admin) {
		admin->tempScene->DISPLAY_EDGES = !admin->tempScene->DISPLAY_EDGES;
	}, "render_display_edges", "render_display_edges");

	admin->commands["render_global_axis"] = new Command([](EntityAdmin* admin) {
		admin->tempScene->GLOBAL_AXIS = !admin->tempScene->GLOBAL_AXIS;
	}, "render_global_axis", "render_global_axis");
}

//add default commands here
void CommandSystem::Init() {
	admin->commands["debug_global"] = new Command([](EntityAdmin* admin) {
		GLOBAL_DEBUG = !GLOBAL_DEBUG;
	}, "debug_global", "debug_global");

	admin->commands["reset_camera"] = new Command([](EntityAdmin* admin) {
		admin->tempCamera->position = Vector3::ZERO;
		admin->tempCamera->rotation = Vector3::ZERO;
	}, "reset_camera", "reset_camera");
	
	AddSpawnCommands(admin);
	AddTimeCommands(admin);
	AddRenderCommands(admin);
}

inline void HandleCanvasInteractions(EntityAdmin* admin, InputSingleton* input) {
	Canvas* canvas = admin->tempCanvas;

	if(input->MousePressed(MOUSE_BUTTON_LEFT)) {
		if(!canvas->hideAll) {
			for(UI* ui : canvas->elements) {
				if(Button* b = dynamic_cast<Button*>(ui)){
					if(b->Clicked(input->mousePos)) {
						input->ui_clicked = true;
						break;
					}
				}
				if(Menu* m = dynamic_cast<Menu*>(ui)) {
					if(m->Clicked(input->mousePos)) {
						if(m->ClickedInTitle(input->mousePos)) {
							input->selectedUI = m;
						}
						input->ui_clicked = true;
						break;
					}
				}
			}
		}
	} else if(input->MouseHeld(MOUSE_BUTTON_LEFT)) {
		static_internal Vector2 offset;
		if(input->selectedUI) {
			if(!input->ui_drag_latch) {
				offset = input->selectedUI->pos - input->mousePos;
				input->ui_drag_latch = true;
			}
			input->selectedUI->pos = input->mousePos + offset;
		}
	} else if(input->MouseReleased(MOUSE_BUTTON_LEFT)) {
		if(input->selectedUI) {
			input->selectedUI = 0;
			input->ui_drag_latch = false;
		}
	}
}

void CommandSystem::Update() {
	InputSingleton* input = admin->singletonInput;

	input->mousePos = admin->p->GetMousePos();

	//execute all triggered commands
	for(auto& c : admin->commands) {
		if(c.second->triggered) {
			c.second->action(admin);
			c.second->triggered = false;
		}
	}

	HandleCanvasInteractions(admin, input);

	//TODO(i,delle) re-implement object selection
}