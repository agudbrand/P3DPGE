#include "RenderCanvasSystem.h"
#include "../utils/GLOBALS.h"
#include "../ui/UI.h"
#include "../ui/UIContainer.h"

#include "../components/Canvas.h"
#include "../components/Screen.h"

inline UI* CreateDebugMenu(EntityAdmin* admin, Canvas* canvas) {
	std::vector<UI*> debug_buttons = std::vector<UI*>();
	debug_buttons.push_back(new TextButton(admin, V2ZERO, "global debug", admin->GetCommand("debug_global")));
	debug_buttons.push_back(new TextButton(admin, V2ZERO, "print commands", admin->GetCommand("debug_command_exec")));
	debug_buttons.push_back(new TextButton(admin, V2ZERO, "pause engine ", admin->GetCommand("time_pause_engine")));
	debug_buttons.push_back(new TextButton(admin, V2ZERO, "next frame", admin->GetCommand("time_next_frame")));

	return new Panel(admin, V2ZERO, "debug", debug_buttons);
}

inline UI* CreateSpawnMenu(EntityAdmin* admin, Canvas* canvas) {
	std::vector<UI*> complex_spawn_buttons;
	complex_spawn_buttons.push_back(new TextButton(admin, V2ZERO, "bmonkey", admin->GetCommand("spawn_complex")));
	complex_spawn_buttons.push_back(new TextButton(admin, V2ZERO, "whale_ship", admin->GetCommand("spawn_complex1")));
	complex_spawn_buttons.push_back(new TextButton(admin, V2ZERO, "24k_Triangles", admin->GetCommand("spawn_complex2")));

	std::vector<UI*> spawn_buttons;
	spawn_buttons.push_back(new TextButton(admin, V2ZERO, "spawn box", admin->GetCommand("spawn_box")));
	spawn_buttons.push_back(new Panel(admin, V2ZERO, "spawn complex", complex_spawn_buttons));

	return new Panel(admin,  V2ZERO, "spawn", spawn_buttons);
}

inline UI* CreateRenderMenu(EntityAdmin* admin, Canvas* canvas) {
	std::vector<UI*> render_buttons = std::vector<UI*>();
	render_buttons.push_back(new TextButton(admin, V2ZERO, "reset camera", admin->GetCommand("reset_camera")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "wireframe", admin->GetCommand("render_wireframe")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "textures", admin->GetCommand("render_textures")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "edge numbers", admin->GetCommand("render_display_edges")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "local axis", admin->GetCommand("render_local_axis")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "global axis", admin->GetCommand("render_global_axis")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "transforms", admin->GetCommand("render_transforms")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "physics vectors", admin->GetCommand("render_physics")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "screen aabb", admin->GetCommand("render_screen_bounding_box")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "mesh vertices", admin->GetCommand("render_mesh_vertices")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "mesh normals", admin->GetCommand("render_mesh_normals")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "grid", admin->GetCommand("render_grid")));
	render_buttons.push_back(new TextButton(admin, V2ZERO, "light rays", admin->GetCommand("render_light_rays")));

	return new Panel(admin, V2ZERO, "render", render_buttons);
}

//TODO(,delle) update this to blender-ish style menu with editable values
inline UI* CreatePropertiesMenu(EntityAdmin* admin, Canvas* canvas) {
	std::vector<UI*> entity_buttons = std::vector<UI*>();
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset position", admin->GetCommand("reset_position")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset x position", admin->GetCommand("reset_position_x")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset y position", admin->GetCommand("reset_position_y")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset z position", admin->GetCommand("reset_position_z")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset velocity", admin->GetCommand("reset_velocity")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset rotation", admin->GetCommand("reset_rotation")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset x rotation", admin->GetCommand("reset_rotation_x")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset y rotation", admin->GetCommand("reset_rotation_y")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset z rotation", admin->GetCommand("reset_rotation_z")));
	entity_buttons.push_back(new TextButton(admin, V2ZERO, "reset rotation velocity", admin->GetCommand("reset_rotation_velocity")));
	
	return new Panel(admin, V2ZERO, "entity", entity_buttons);
}

inline void AddTopMenus(EntityAdmin* admin, Canvas* canvas) {
	UIRow* topMenu = new UIRow(V2ZERO, UIAnchor::NONE, true, 0, 20);
	topMenu->Add(CreateDebugMenu(admin, canvas));
	topMenu->Add(CreateSpawnMenu(admin, canvas));
	topMenu->Add(CreateRenderMenu(admin, canvas));
	topMenu->Add(CreatePropertiesMenu(admin, canvas));
	canvas->containers.push_back(topMenu);
}

inline void AddBufferLog(EntityAdmin* admin, Canvas* canvas) { //TODO(, delle) re-add bufferlog
	g_cBuffer.allocate_space(100);
	//canvas->elements.push_back(new Panel(admin, Vector2(10,100), "BUFFERLOG", std::vector<UI*>{}));
}

void RenderCanvasSystem::Init() {
	Canvas* canvas = admin->tempCanvas;
	AddBufferLog(admin, canvas);
	AddTopMenus(admin, canvas);
}

void RenderCanvasSystem::Update() {
	Canvas*	canvas =			admin->tempCanvas;
	Screen*	screen =			admin->singletonScreen;
	olc::PixelGameEngine* p =	admin->p;

	if(!canvas->hideAll) {
		for(UIContainer* con : canvas->containers) {
			for(UI* ui : con->container) {
				ui->Draw(p);
			}
		}
	}

	p->DrawStringDecal(olc::vf2d(screen->width - 300, screen->height - 20), "Mouse: " + screen->mousePosV3.str2f());
}