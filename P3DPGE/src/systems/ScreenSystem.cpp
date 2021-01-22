#include "ScreenSystem.h"

#include "../components/Screen.h"

void ScreenSystem::Update() {
	Screen*		screen = admin->screen;
	olc::PixelGameEngine*	p = admin->p;

	//clear the screen to black
	p->Clear(olc::BLACK);

	if(p->ScreenWidth() == screen->width && p->ScreenHeight() == screen->height) { //TODO check for pixel size change
		if(screen->changedResolution){ screen->changedResolution = false; }
		return;
	}

	screen->changedResolution = true;
	screen->width = p->ScreenWidth();
	screen->height = p->ScreenHeight();
	screen->width = screen->width * screen->height;
	screen->dimensions = Vector2(screen->width, screen->height);
	//SUCCESS(__FILENAME__, ": Changing resolution from ", screen->dimensionsV3, " to ", Vector3(screen->width, screen->height));
	Debug::ToString(2, __FILENAME__ + std::string(": Changing resolution from ") + screen->dimensionsV3.str2f() + std::string(" to ") + Vector3(screen->width, screen->height).str2f());
	screen->dimensionsV3 = Vector3(screen->width, screen->height);
}