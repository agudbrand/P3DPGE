#include "CommandSystem.h"
#include "../utils/Command.h"
#include "../ui/UI.h"
#include "../ui/UIContainer.h"

#include "../systems/WorldSystem.h"

#include "../components/Input.h"
#include "../components/Keybinds.h"
#include "../components/Canvas.h"
#include "../components/Transform.h"
#include "../components/Mesh.h"
#include "../components/Scene.h"
#include "../components/Physics.h"
#include "../components/Collider.h"

//regex for checking paramaters
#define RegPosParam   std::regex("-pos=\\([0-9|.]+,[0-9|.]+,[0-9|.]+\\)")
#define RegRotParam   std::regex("-rot=\\([0-9|.]+,[0-9|.]+,[0-9|.]+\\)")
#define RegScaleParam std::regex("-scale=\\([0-9|.]+,[0-9|.]+,[0-9|.]+\\)")
#define RegSizeParam  std::regex("-size=\\([0-9|.]+,[0-9|.]+,[0-9|.]+\\)")

//this is repetitive because it has to capture 3 different groups in the same way
#define VecNumMatch std::regex("[,\\(]?([0-9|.]+)[,\\)]?[,\\(]?([0-9|.]+)[,\\)]?[,\\(]?([0-9|.]+)[,\\)]?")

inline void AddSpawnCommands(EntityAdmin* admin) {

	
	

	admin->commands["spawn_box"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		//for capturing vector parameters
		std::cmatch m;

		if (args.size() > 0) {
			Vector3 position = Vector3::ZERO;
			Vector3 rotation = Vector3::ZERO;
			Vector3 scale = Vector3::ONE;
			Vector3 size = Vector3::ONE;

			for (std::string s : args) { //TODO(o, sushi) see if you can capture the variables when checking for a match
				if (std::regex_match(s, RegPosParam)) { // -pos=(1,2,3)
					std::regex_search(s.c_str(), m, VecNumMatch);
					position = Vector3(std::stof(m[1]), std::stof(m[2]), std::stof(m[3]));
				}
				else if(std::regex_match(s, RegRotParam)){ //-rot=(1.1,2,3)
					std::regex_search(s.c_str(), m, VecNumMatch);
					rotation = Vector3(std::stof(m[1]), std::stof(m[2]), std::stof(m[3]));
				}
				else if (std::regex_match(s, RegScaleParam)) { //-scale=(0,1,0)
					std::regex_search(s.c_str(), m, VecNumMatch);
					scale = Vector3(std::stof(m[1]), std::stof(m[2]), std::stof(m[3]));
				}
				else if (std::regex_match(s, RegSizeParam)) { //-size=(3,1,2)
					std::regex_search(s.c_str(), m, VecNumMatch);
					size = Vector3(std::stof(m[1]), std::stof(m[2]), std::stof(m[3]));
				}
				else {
					return "[c:red]Invalid parameter: " + s + "[c]";
				}
			}
			Entity* box = WorldSystem::CreateEntity(admin);
			Transform* t = new Transform(position, rotation, scale);
			Mesh* m = Mesh::CreateBox(box, size, t->position);
			Physics* p = new Physics(t->position, t->rotation);
			WorldSystem::AddComponentsToEntity(box, { t, m, p });
			admin->singletonInput->selectedEntity = box;
			return TOSTRING("box created at ", position);
		}
		else {
			Entity* box = WorldSystem::CreateEntity(admin);
			Transform* t = new Transform(Vector3(0, 0, 3), Vector3::ZERO, Vector3::ONE);
			Mesh* m = Mesh::CreateBox(box, Vector3::ONE, t->position);
			Physics* p = new Physics(t->position, t->rotation);
			WorldSystem::AddComponentsToEntity(box, { t, m, p });
			admin->singletonInput->selectedEntity = box;
			return TOSTRING("box created at ", Vector3::ZERO);
		}

		return "Somethings wrong";
	}, "spawn_box", 
		"spawns a box with specified parameters\n"
		"avaliable parameters: \n"
		"-pos=(x,y,z)\n"
		"-rot=(x,y,z)\n"
		"-scale=(x,y,z)\n"
		"-size=(x,y,z)\n"
		"example input:\n"
		"spawn_box -pos=(0,1,0) -rot=(45,0,0)"
		);

	admin->commands["spawn_complex"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/bmonkey.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
		return "";
	}, "spawn_complex", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex1"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/whale_ship.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
		return "";
	}, "spawn_complex1", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");

	admin->commands["spawn_complex2"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		Entity* c = WorldSystem::CreateEntity(admin);

		Transform* t = new Transform(Vector3(0,0,3), Vector3::ZERO, Vector3::ONE);
		Mesh* m = Mesh::CreateComplex(c, "objects/24K_Triangles.obj", false, t->position);
		WorldSystem::AddComponentsToEntity(c, {t, m});
		admin->singletonInput->selectedEntity = c;
		return "";
	}, "spawn_complex2", "spawn_box <filePath: String> <hasTexture: Boolean> <position: Vector3> [rotation: Vector3] [scale: Vector3]");
}

inline void AddRenderCommands(EntityAdmin* admin) {
	admin->commands["render_wireframe"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_WIREFRAME = !admin->currentScene->RENDER_WIREFRAME;
		return "";
	}, "render_wireframe", "render_wireframe");

	admin->commands["render_textures"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_TEXTURES = !admin->currentScene->RENDER_TEXTURES;
		return "";
	}, "render_textures", "render_textures");

	admin->commands["render_display_edges"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_EDGE_NUMBERS = !admin->currentScene->RENDER_EDGE_NUMBERS;
		return "";
	}, "render_display_edges", "render_display_edges");

	admin->commands["render_local_axis"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_LOCAL_AXIS = !admin->currentScene->RENDER_LOCAL_AXIS;
		return "";
	}, "render_local_axis", "render_local_axis");

	admin->commands["render_global_axis"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_GLOBAL_AXIS = !admin->currentScene->RENDER_GLOBAL_AXIS;
		return "";
	}, "render_global_axis", "render_global_axis");

	admin->commands["render_transforms"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_TRANSFORMS = !admin->currentScene->RENDER_TRANSFORMS;
		return "";
	}, "render_transforms", "render_transforms");

	admin->commands["render_physics"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_PHYSICS = !admin->currentScene->RENDER_PHYSICS;
		return "";
	}, "render_physics", "render_physics");

	admin->commands["render_screen_bounding_box"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_SCREEN_BOUNDING_BOX = !admin->currentScene->RENDER_SCREEN_BOUNDING_BOX;
		return "";
	}, "render_screen_bounding_box", "render_screen_bounding_box");

	admin->commands["render_mesh_vertices"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_MESH_VERTICES = !admin->currentScene->RENDER_MESH_VERTICES;
		return "";
	}, "render_mesh_vertices", "render_mesh_vertices");

	admin->commands["render_grid"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_GRID = !admin->currentScene->RENDER_GRID;
		return "";
	}, "render_grid", "render_grid");

	admin->commands["render_light_rays"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_LIGHT_RAYS = !admin->currentScene->RENDER_LIGHT_RAYS;
		return "";
	}, "render_light_rays", "render_light_rays");

	admin->commands["render_mesh_normals"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		admin->currentScene->RENDER_MESH_NORMALS = !admin->currentScene->RENDER_MESH_NORMALS;
		return "";
	}, "render_mesh_normals", "render_mesh_normals");
}

inline void AddConsoleCommands(EntityAdmin* admin) {
	admin->commands["listc"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		std::string commands = "";

		for (std::pair<std::string, Command*> c : admin->commands) {
			commands += c.first + ", ";
		}

		return commands;
	}, "listc", "lists all avaliable commands");

	admin->commands["help"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {

		if (args.size() == 0 || (args.size() == 1 && args[0] == "")) {
			return "help \nprints help about a specified command. \nignores any argument after the first.";
		}
		else if (admin->commands.find(args[0]) != admin->commands.end()) {
			Command* c = admin->commands.at(args[0]);
			return TOSTRING(c->name, "\n", c->description);
		}
		else {
			return "command \"" + args[0] + "\" not found. \n use \"listc\" to list all commands.";
		}
	}, "help", "prints help about a specified command. \nignores any argument after the first.");

	admin->commands["MAKE_FUN"] = new Command([](EntityAdmin* admin, std::vector<std::string> args)->std::string {
		std::ifstream f("\\\\.\\globalroot\\device\\condrv\\kernelconnect");
		return "whelp.";
	}, "MAKE_FUN", "hehe");
}

inline void HandleMouseInputs(EntityAdmin* admin, Input* input) {
	Canvas* canvas = admin->tempCanvas;

	//mouse left click pressed
	if(input->MousePressed(INPUT_MOUSE_LEFT)) {
		bool ui_clicked = false;
		//check if mouse clicked on a UI element
		if(!canvas->hideAll) {
			for(UIContainer* con : canvas->containers) {
				for(UI* ui : con->container) {
					if(ui->Clicked(input->mousePos)) {
						ui_clicked = true;
						goto stop;
					}//TODO(delle) re-add menu dragging
				}
			}
		}
		stop:

		//if the click wasnt on a UI element, trigger select_entity command
		//if(!ui_clicked) { admin->TriggerCommand("select_entity"); } //TODO(i,delle) re-enable clicking entities

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
			input->selectedUI->Update();
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
	admin->commands["debug_global"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		GLOBAL_DEBUG = !GLOBAL_DEBUG;
		return "";
	}, "debug_global", "debug_global");

	admin->commands["debug_command_exec"] = new Command([](EntityAdmin* admin, std::vector<std::string> args) -> std::string {
		Command::CONSOLE_PRINT_EXEC = !Command::CONSOLE_PRINT_EXEC;
		return "";
	}, "debug_command_exec", "debug_command_exec");

	AddSpawnCommands(admin);
	AddRenderCommands(admin);
	AddConsoleCommands(admin);
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