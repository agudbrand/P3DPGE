#pragma once
#include "Component.h"
#include <time.h>

struct TimeSingleton : public Component {
	float totalTime;
	uint64 updateCount;

	float physicsTimeStep;
	float physicsDeltaTime;
	float physicsCounter;

	TimeSingleton() {
		totalTime = 0.f;
		updateCount = 0.f;

		physicsTimeStep = 300.f;
		physicsDeltaTime = 1.f / physicsTimeStep;
		physicsCounter = 0.f;
	}

	TimeSingleton(float physicsTicksPerSecond) {
		totalTime = 0.f;
		updateCount = 0.f;

		physicsTimeStep = physicsTicksPerSecond;
		physicsDeltaTime = 1.f / physicsTimeStep;
		physicsCounter = 0.f;
	}
};