#include "CommandSystem.h"

#include "../utils/Command.h"
#include "../systems/WorldSystem.h"

#include "../components/InputSingleton.h"
#include "../components/Keybinds.h"
#include "../components/Canvas.h"
#include "../components/Transform.h"
#include "../components/Mesh.h"
#include "../components/Scene.h"
#include "../components/Physics.h"

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
	}, "spawn_complex", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex1"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/whale_ship.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
	}, "spawn_complex1", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex2"] = new Command([](EntityAdmin* admin) {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/24K_Triangles.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
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
}

inline void HandleMouseInputs(EntityAdmin* admin, InputSingleton* input) {
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

inline void HandleSelectedEntityInputs(EntityAdmin* admin, InputSingleton* input) {
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
}

inline void HandleRenderInputs(EntityAdmin* admin, InputSingleton* input, Keybinds* binds) {
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

	AddSpawnCommands(admin);
	AddRenderCommands(admin);
}

void CommandSystem::Update() {
	InputSingleton* input = admin->singletonInput;
	Keybinds* binds = admin->currentKeybinds;

	input->mousePos = admin->p->GetMousePos();

	HandleMouseInputs(admin, input);
	HandleSelectedEntityInputs(admin, input);
	HandleRenderInputs(admin, input, binds);
}