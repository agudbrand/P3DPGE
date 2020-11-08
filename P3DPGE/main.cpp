#define OLC_PGE_APPLICATION

#include <iostream>
#include <vector>
#include "olcPixelGameEngine.h"
#include "Moveables.h"
#include "Entities.h"

using namespace olc;

class P3DPGE : public PixelGameEngine {
private:
	float time;

	Sphere s = Sphere(10, 1, Vector3(100, 100, 0));
	Box b = Box(Vector3(14, 30, 0), 2, Vector3(150, 150, 0));

public:
	P3DPGE() { sAppName = "P3DPGE"; }

	bool OnUserCreate() override {
		time = 0;
		return true;
	}
	
	bool OnUserUpdate(float dTime) {
		time += dTime;
		
		if (GetKey(G).bHeld) {
			dTime = 0;
		}
		
		Clear(olc::BLACK);

		s.Draw(this);
		b.Draw(this);
		b.AddForce(nullptr, Vector3(1, 2, 0));
		b.Update(dTime);

		return true;
	}
	
};

int main() {
	P3DPGE game;
	if (game.Construct(256, 240, 4, 4)) { game.Start(); }
}