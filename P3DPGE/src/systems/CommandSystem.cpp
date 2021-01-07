#include "CommandSystem.h"
#include "../utils/Command.h"

#include "../systems/WorldSystem.h"

#include "../components/Input.h"
#include "../components/Keybinds.h"
#include "../components/Canvas.h"
#include "../components/Transform.h"
#include "../components/Mesh.h"
#include "../components/Scene.h"
#include "../components/Physics.h"
#include "../components/Collider.h"

inline void AddSpawnCommands(EntityAdmin* admin) {
	admin->commands["spawn_box"] = new Command([](EntityAdmin* admin) {
		Entity* box = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateBox(box, Vector3::ONE, t->position);
		Physics* p = new Physics(t->position, t->rotation);
		WorldSystem::AddComponentsToEntity(box, {t, m, p});
		admin->singletonInput->selectedEntity = box;
	}, "spawn_box", "spawn_box <halfDimensions: Vector3> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/bmonkey.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
	}, "spawn_complex", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex1"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/whale_ship.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
	}, "spawn_complex1", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex2"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/24K_Triangles.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
	}, "spawn_complex2", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");
}

inline void AddRenderCommands(EntityAdmin* admin) {
	admin->commands["render_wireframe"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_WIREFRAME = !admin->currentScene->RENDER_WIREFRAME;
	}, "render_wireframe", "render_wireframe");

	admin->commands["render_textures"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_TEXTURES = !admin->currentScene->RENDER_TEXTURES;
	}, "render_textures", "render_textures");

	admin->commands["render_display_edges"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_EDGE_NUMBERS = !admin->currentScene->RENDER_EDGE_NUMBERS;
	}, "render_display_edges", "render_display_edges");

	admin->commands["render_local_axis"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_LOCAL_AXIS = !admin->currentScene->RENDER_LOCAL_AXIS;
	}, "render_local_axis", "render_local_axis");

	admin->commands["render_global_axis"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_GLOBAL_AXIS = !admin->currentScene->RENDER_GLOBAL_AXIS;
	}, "render_global_axis", "render_global_axis");

	admin->commands["render_transforms"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_TRANSFORMS = !admin->currentScene->RENDER_TRANSFORMS;
	}, "render_transforms", "render_transforms");

	admin->commands["render_physics"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_PHYSICS = !admin->currentScene->RENDER_PHYSICS;
	}, "render_physics", "render_physics");

	admin->commands["render_screen_bounding_box"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_SCREEN_BOUNDING_BOX = !admin->currentScene->RENDER_SCREEN_BOUNDING_BOX;
	}, "render_screen_bounding_box", "render_screen_bounding_box");

	admin->commands["render_mesh_vertices"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_MESH_VERTICES = !admin->currentScene->RENDER_MESH_VERTICES;
	}, "render_mesh_vertices", "render_mesh_vertices");

	admin->commands["render_grid"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_GRID = !admin->currentScene->RENDER_GRID;
	}, "render_grid", "render_grid");

	admin->commands["render_light_rays"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_LIGHT_RAYS = !admin->currentScene->RENDER_LIGHT_RAYS;
	}, "render_light_rays", "render_light_rays");

	admin->commands["render_mesh_normals"] = new Command([](EntityAdmin* admin) {
		admin->currentScene->RENDER_MESH_NORMALS = !admin->currentScene->RENDER_MESH_NORMALS;
	}, "render_mesh_normals", "render_mesh_normals");
}

inline void HandleMouseInputs(EntityAdmin* admin, Input* input) {
	Canvas* canvas = admin->tempCanvas;

	//mouse left click pressed
	if(input->MousePressed(INPUT_MOUSE_LEFT)) {
		//check if mouse clicked on a UI element
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

		//if the click wasnt on a UI element, trigger select_entity command
		if(!input->ui_clicked) {
			admin->TriggerCommand("select_entity"); //TODO(i,delle) test that you can select an entity
		}

		//set click pos to mouse pos
		input->mouseClickPos = input->mousePos;
	}
	//mouse left click held
	else if(input->MouseHeld(INPUT_MOUSE_LEFT)) {
		static_internal Vector2 offset;
		if(input->selectedUI) {
			if(!input->ui_drag_latch) {
				offset = input->selectedUI->pos - input->mousePos;
				input->ui_drag_latch = true;
			}
			input->selectedUI->pos = input->mousePos + offset;
		}
	} 
	//mouse left click released
	else if(input->MouseReleased(INPUT_MOUSE_LEFT)) {
		if(input->selectedUI) {					//deselect UI
			input->selectedUI = 0;
			input->ui_drag_latch = false;
		} else if(input->selectedEntity && input->mousePos != input->mouseClickPos) { //add force to selected entity
			admin->ExecCommand("add_force");
		}

		//reset click pos to null
		input->mouseClickPos = Vector2(admin->p->ScreenWidth(), admin->p->ScreenHeight());
	}
}

inline void HandleSelectedEntityInputs(EntityAdmin* admin, Input* input) {
	//translation
	if(input->KeyDown(olc::L, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_right");
	}

	if(input->KeyDown(olc::J, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_left");
	}

	if(input->KeyDown(olc::O, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_up");
	}

	if(input->KeyDown(olc::U, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_down");
	}

	if(input->KeyDown(olc::I, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_forward");
	}

	if(input->KeyDown(olc::K, INPUT_NONE_HELD)) {
		admin->ExecCommand("translate_backward");
	}

	//rotation
	if(input->KeyDown(olc::L, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_+x");
	}

	if(input->KeyDown(olc::J, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_-x");
	}

	if(input->KeyDown(olc::O, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_+y");
	}

	if(input->KeyDown(olc::U, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_-y");
	}

	if(input->KeyDown(olc::I, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_+z");
	}

	if(input->KeyDown(olc::K, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("rotate_-z");
	}
}

inline void HandleRenderInputs(EntityAdmin* admin, Input* input, Keybinds* binds) {
	//toggle wireframe
	if(input->KeyPressed(binds->debugRenderWireframe, INPUT_NONE_HELD)) {
		admin->ExecCommand("render_wireframe");
	}

	//toggle textures
	if(input->KeyPressed(binds->debugRenderWireframe, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("render_textures");
	}

	//toggle edge numbers
	if(input->KeyPressed(binds->debugRenderEdgesNumbers, INPUT_NONE_HELD)) {
		admin->ExecCommand("render_display_edges");
	}

	//toggle edge numbers
	if(input->KeyPressed(binds->debugRenderDisplayAxis, INPUT_NONE_HELD)) {
		admin->ExecCommand("render_local_axis");
	}

	//toggle edge numbers
	if(input->KeyPressed(binds->debugRenderDisplayAxis, INPUT_SHIFT_HELD)) {
		admin->ExecCommand("render_global_axis");
	}
}

//add generic commands here
void CommandSystem::Init() {
	admin->commands["debug_global"] = new Command([](EntityAdmin* admin) {
		GLOBAL_DEBUG = !GLOBAL_DEBUG;
	}, "debug_global", "debug_global");

	admin->commands["debug_command_exec"] = new Command([](EntityAdmin* admin) {
		Command::CONSOLE_PRINT_EXEC = !Command::CONSOLE_PRINT_EXEC;
	}, "debug_command_exec", "debug_command_exec");

	AddSpawnCommands(admin);
	AddRenderCommands(admin);
}

void CommandSystem::Update() {
	Input* input = admin->singletonInput;
	Keybinds* binds = admin->currentKeybinds;

	input->mousePos = admin->p->GetMousePos();

	HandleMouseInputs(admin, input);
	HandleSelectedEntityInputs(admin, input);
	HandleRenderInputs(admin, input, binds);

	if(input->KeyPressed(olc::F1, INPUT_SHIFT_HELD)) {
		Entity* box = WorldSystem::CreateEntity(admin);
		Transform* t = new Transform(Vector3(-10, 0, 20), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateBox(box, Vector3(5, 5, 5), t->position);
		Physics* p = new Physics(t->position, t->rotation, 100.f, 0.1f); //heavy concrete cube
		AABBCollider* c = new AABBCollider(box, Vector3(5, 5, 5), p->mass);
		WorldSystem::AddComponentsToEntity(box, {t, m, p, c});

		Entity* sphere = WorldSystem::CreateEntity(admin);
		Transform* t2 = new Transform(Vector3(30, 0, 20), Vector3::ZERO, Vector3::ONE);
		Mesh* m2 = Mesh::CreateBox(sphere, Vector3(.3f, .3f, .3f), t2->position);
		Physics* p2 = new Physics(t2->position, t2->rotation, Vector3(-30, 2, 0)); //light rubber ball
		p2->elasticity = .5f;
		SphereCollider* c2 = new SphereCollider(sphere, 1, p2->mass);
		sphere->AddComponents({t2, m2, p2, c2});

		admin->singletonInput->selectedEntity = box;
	}
}