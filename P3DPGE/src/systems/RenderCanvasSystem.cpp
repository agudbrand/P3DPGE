#include "RenderCanvasSystem.h"
#include "../internal/olcPixelGameEngine.h"
#include "../EntityAdmin.h"

#include "../components/Canvas.h"
#include "../components/ScreenSingleton.h"

void RenderCanvasSystem::Init() {
	Canvas* canvas = admin->tempCanvas;
	
	//debug menu
	std::vector<Button*> debug_buttons = std::vector<Button*>();
	debug_buttons.push_back(new Button(admin, admin->GetCommand("debug_global"), "global debug toggle"));
	debug_buttons.push_back(new Button(admin, admin->GetCommand("time_pause_engine"), "pause engine toggle"));
	debug_buttons.push_back(new Button(admin, admin->GetCommand("time_next_frame"), "next frame"));

	canvas->elements.push_back(new Menu(admin, Vector2(5, 5), "debug_menu", "debug", debug_buttons));

	//spawn menu
	std::vector<Button*> spawn_buttons;
	spawn_buttons.push_back(new Button(admin, admin->GetCommand("spawn_box"), "spawn_box"));
	spawn_buttons.push_back(new Button(admin, 0, "spawn_complex", "", true, 0));

	std::vector<Button*> complex_spawn_buttons;
	complex_spawn_buttons.push_back(new Button(admin, admin->GetCommand("spawn_complex"), "bmonkey", ""));
	complex_spawn_buttons.push_back(new Button(admin, admin->GetCommand("spawn_complex1"), "whale_ship", ""));
	complex_spawn_buttons.push_back(new Button(admin, admin->GetCommand("spawn_complex2"), "24k_Triangles", ""));

	canvas->elements.push_back(new Menu(admin, Vector2(canvas->elements[0]->size.x + 10, 5), "spawn_menu", "spawn", spawn_buttons,
			std::vector<Menu*>{new Menu(admin, V2ZERO, "complex_spawn", "", complex_spawn_buttons)}));

	//render menu
	std::vector<Button*> render_buttons = std::vector<Button*>();
	render_buttons.push_back(new Button(admin, admin->GetCommand("reset_camera"), "reset camera"));
	render_buttons.push_back(new Button(admin, admin->GetCommand("render_wireframe"), "wireframe"));
	render_buttons.push_back(new Button(admin, admin->GetCommand("render_wireframe_notexture"), "wireframe only"));
	render_buttons.push_back(new Button(admin, admin->GetCommand("render_display_edges"), "edge numbers"));
	render_buttons.push_back(new Button(admin, admin->GetCommand("render_global_axis"), "global axis"));

	canvas->elements.push_back(new Menu(admin, Vector2(canvas->elements[1]->pos.x + canvas->elements[1]->size.x + 10, 5), "render_menu", "render", render_buttons));
}

void RenderCanvasSystem::Update() {
	Canvas*					canvas = admin->tempCanvas;
	ScreenSingleton*		screen = admin->singletonScreen;
	olc::PixelGameEngine*	p = admin->p;

	if(!canvas->hideAll) {
		for(UI* ui : canvas->elements) {
			if(Menu* m = dynamic_cast<Menu*>(ui)) {
				if(m->title == "BUFFERLOG") {
					//m->update_dyn_strings(); //TODO(,sushi) idk this bufferlog stuff
				}
			}
			ui->Draw(p);
		}
	}

	p->DrawStringDecal(olc::vf2d(screen->width - 300, screen->height - 20), "Mouse: " + screen->mousePosV3.str2f());
}