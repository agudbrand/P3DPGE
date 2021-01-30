#pragma once
#define OLC_GFX_VULKAN1
#define __GLFW__
#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION
#include "internal/olcPixelGameEngine.h"

#define KEYBOARD_LAYOUT_US_UK
#define DEBUG_P3DPGE
#include "EntityAdmin.h"

/* Include Requirements
C++ Includes:
  C:\src\tinyobjloader-master;
  C:\src\boost_1_74_0
  C:\src\stb-master;
  C:\VulkanSDK\1.2.162.1\Include;
  C:\src\glm;
  C:\src\glfw-3.3.2.bin.WIN64\include;

Linker Libraries:
  C:\VulkanSDK\1.2.162.1\Lib;
  C:\src\glfw-3.3.2.bin.WIN64\lib-vc2015;
*/

/* Changes to olcPixelGameEngine.h
1. Changed variables to public starting with pDrawTarget
2. Add additional mouse functions from https://github.com/OneLoneCoder/olcPixelGameEngine/pull/142
3. Add GLFW platform from https://github.com/OneLoneCoder/olcPixelGameEngine/pull/181
*/

class P3DPGE : public olc::PixelGameEngine {
public:
	EntityAdmin entityAdmin;
	int gameLayer;

	P3DPGE() { sAppName = "P3DPGE"; }

	bool OnUserCreate() override {
		//Create a new Layer which will be used for the game
		gameLayer = CreateLayer();
		//The layer is not enabled by default,  so we need to enable it
		EnableLayer(gameLayer, true);

		entityAdmin.Init(this);
		return true;
	}

	bool OnUserUpdate(float deltaTime) override {
		//Change the Draw Target to not be Layer 0
		SetDrawTarget((uint8_t)gameLayer);

		entityAdmin.Update();
		return true;
	}

	bool OnUserDestroy() {
		entityAdmin.Cleanup();
		return true;
	}
};

int main() {
	srand(time(0));
	
	P3DPGE game;
	if(game.Construct(1280, 720, 1, 1, false, false)) { game.Start(); }
}