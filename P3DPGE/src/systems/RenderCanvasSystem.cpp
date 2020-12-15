#include "RenderCanvasSystem.h"
#include "../internal/olcPixelGameEngine.h"
#include "../EntityAdmin.h"

#include "../components/Canvas.h"
#include "../components/ScreenSingleton.h"

void RenderCanvasSystem::Update(float deltaTime, olc::PixelGameEngine* p) {
	Canvas* canvas = admin->singletonCanvas;
	ScreenSingleton* screen = admin->singletonScreen;

	if(!canvas->hideAll) {
		for(UI* ui : canvas->elements) {
			if(Menu* m = dynamic_cast<Menu*>(ui)) {
				if(m->title == "BUFFERLOG") {
					//m->update_dyn_strings(); //TODO(,sushi) idk this stuff
				}
			}
			ui->Draw(p);
		}
	}

	//DEBUGR p->DrawStringDecal(olc::vf2d(screenWidth - 300, screenHeight - 20), "Mouse: " + Vector3(p->GetMousePos()).str2f());
	p->DrawStringDecal(olc::vf2d(screen->width - 300, screen->height - 20), "Mouse: " + screen->mousePosV3.str2f());
}