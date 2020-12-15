#pragma once
#include "System.h"

struct Camera;
struct InputSingleton;
struct Keybinds;

struct SimpleMovementSystem : public System {
	void CameraMovement(float deltaTime, Camera* camera, InputSingleton* input, Keybinds* binds, uint32 moveState);

	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};